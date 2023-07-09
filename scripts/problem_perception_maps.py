import json
import os
import sys
import numpy as np
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw
from matplotlib import pyplot as plt
from dotenv import load_dotenv

GLOBAL_SCORE_MULTIPLIER = 1_000_000

def _sq_intensity_lut(radius:int):
    diameter = 2*radius+1
    grid = np.mgrid[-radius:radius+.1:1., -radius:radius+.1:1.].reshape(2,-1).T
    sq_dists = np.sum(np.square(grid), 1).reshape(diameter,diameter)
    sq_dists[radius,radius] = 1.
    sq_intensity_map = 1. / sq_dists
    return np.array(sq_intensity_map)

cached_intensity_maps = {}
def sq_intensity_lut(radius:int):
    if radius in cached_intensity_maps:
        return cached_intensity_maps[radius]
    data = _sq_intensity_lut(radius)
    cached_intensity_maps[radius] = data
    return data

def get_preview_subfolder(room_size, compute_radius):
    top_compute_radius = get_top_compute_radius(room_size)
    if compute_radius == top_compute_radius:
        return ''
    return f'{compute_radius}px/'

def get_top_compute_radius(room_size):
    return int(max(*room_size))

def get_compute_radius(room_size, max_compute_radius=None):
    top_compute_radius = get_top_compute_radius(room_size)
    if max_compute_radius:
        return min(max_compute_radius, top_compute_radius)
    return top_compute_radius

def get_perception_map(room_size, stage_xy, instrument_id, attendees, compute_radius=None):
    x0, y0, x1, y1 = stage_xy
    w, h = int(x1-x0), int(y1-y0)
    compute_diameter = 2*compute_radius+1
    intensity_lut = sq_intensity_lut(compute_radius)
    perception_map = np.zeros((w, h), dtype='float64')
    # print('>>>>>', 'intensity map:', perception_map.shape)
    # print('>>>>>', 'compute_radius:', compute_radius)
    for aid, a in enumerate(attendees):
        x = a['x']
        y = a['y']
        taste = a['tastes'][instrument_id]
        if x - x1 >= compute_radius and \
            x - x0 <= compute_radius and \
            y - y1 >= compute_radius and \
            y - y0 <= compute_radius:
            print(f'>>> attendee {aid} too far from the stage. skipping intensity compute')
            continue
        lut_scene_x0 = compute_radius + int(x0-x)
        lut_scene_x1 = lut_scene_x0 + w
        lut_scene_y0 = compute_radius + int(y0-y)
        lut_scene_y1 = lut_scene_y0 + h

        attendee_lut_x0 = max(lut_scene_x0, 0)
        attendee_lut_x1 = min(lut_scene_x1, compute_diameter)
        attendee_lut_y0 = max(lut_scene_y0, 0)
        attendee_lut_y1 = min(lut_scene_y1, compute_diameter)
        attendee_lut = intensity_lut[attendee_lut_x0:attendee_lut_x1, attendee_lut_y0:attendee_lut_y1]

        vpm_x0 = max(0, -lut_scene_x0)
        vpm_x1 = min(-lut_scene_x0 + compute_diameter, w)
        vpm_y0 = max(0, -lut_scene_y0)
        vpm_y1 = min(-lut_scene_y0 + compute_diameter, h)
        visible_perception_map = perception_map[vpm_x0:vpm_x1, vpm_y0:vpm_y1]

        # print('>>>  ', 'attendee:', aid)
        # print('>>>>>', 'stage:', stage_xy)
        # print('>>>>>', 'attendee at:', (x, y))
        # print('>>>>>', 'scene rect relative to attendee lut:', (lut_scene_x0, lut_scene_y0, lut_scene_x1, lut_scene_y1))
        # print('>>>>>', 'attendee lut rect:', (attendee_lut_x0, attendee_lut_y0, attendee_lut_x1, attendee_lut_y1))
        # print('>>>>>', 'attendee lut size:', attendee_lut.shape)
        # print('>>>>>', 'visible perception map rect:', (vpm_x0, vpm_y0, vpm_x1, vpm_y1))
        # print('>>>>>', 'visible perception map shape:', visible_perception_map.shape)
        visible_perception_map += attendee_lut * taste
    return perception_map

def get_perception_log10p_map(perception_map):
    lovers = np.clip(perception_map, 0., np.max(perception_map))
    # print('>>>> lovers', lovers)
    # print('>>>> log lovers', np.log1p(lovers,))
    haters = -(perception_map-lovers)
    # print('>>>> haters', haters)
    # print('>>>> log haters', np.log1p(haters))
    return np.log(lovers + 1.) - np.log(haters + 1.)

def generate_problem_perception_map(problem, instrument_id, compute_radius=None):
    room_width = problem['room_width']
    room_height = problem['room_height']
    stage_width = problem['stage_width']
    stage_height = problem['stage_height']
    attendees = problem['attendees']
    stage_x0, stage_y0 = problem['stage_bottom_left']
    stage_x1, stage_y1 = stage_x0 + stage_width, stage_y0 + stage_height
    perception_map = GLOBAL_SCORE_MULTIPLIER * get_perception_map(
        (int(room_width), int(room_height)),
        (stage_x0, stage_y0, stage_x1, stage_y1),
        instrument_id,
        attendees,
        compute_radius=compute_radius
    )
    return perception_map

def show_perception_map(lut):
    plt.figure('Linear')
    plt.imshow(lut.swapaxes(0, 1), interpolation="nearest", origin="upper")
    plt.colorbar()
    plt.gca().invert_yaxis()
    lut_log10p = get_perception_log10p_map(lut)
    plt.figure('Log 10')
    plt.imshow(lut_log10p.swapaxes(0, 1), interpolation="nearest", origin="upper")
    plt.colorbar()
    plt.gca().invert_yaxis()
    plt.show()

def save_perception_map_preview(save_path, problem_id, instrument_id, data):
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    w, h = data.shape
    plt.figure(f'{problem_id}_{instrument_id}', frameon=False, figsize=(w*2, h*2), dpi=1)
    plt.imshow(data.swapaxes(0, 1), interpolation="nearest", origin="upper")
    plt.gca().invert_yaxis()
    plt.axis('off')
    plt.savefig(save_path, bbox_inches='tight', pad_inches=0)
    plt.close()

def generate_and_save_perception_map_by_instrument_id(problem, instrument_id, max_compute_radius=None):
    room_size = (problem['room_width'], problem['room_height'])
    compute_radius = get_compute_radius(room_size, max_compute_radius=max_compute_radius)
    luts_folder = f'perception_luts/{problem_id}'
    file_suffix = get_preview_subfolder(room_size, compute_radius)
    lut = generate_problem_perception_map(problem, instrument_id, compute_radius=compute_radius)
    print(f'  ok.', end='')
    save_perception_map_preview(f'{luts_folder}/{file_suffix}/{instrument_id}.png', problem_id, instrument_id, lut)
    print(f' saved preview linear', end='')
    lut_log10p = get_perception_log10p_map(lut)
    save_perception_map_preview(f'{luts_folder}/{file_suffix}/{instrument_id}.log10.png', problem_id, instrument_id, lut_log10p)
    print(f' and log10p.')
    with open(f'{luts_folder}/{file_suffix}/{instrument_id}.lut.bin', 'wb') as bin_file:
        bin_file.write(lut.tobytes())
    print(f' ...saved bin LUT.')
    return lut

def generate_and_save_perception_maps(problem_id, max_compute_radius=None):
    print(f'generating perceptions for problem {problem_id}')
    output_folder = f'perception_luts/{problem_id}'
    os.makedirs(output_folder, exist_ok=True)
    with open(f'problems/{problem_id}.json', 'r') as file:
        problem = json.loads(file.read())
    instruments = len(problem['attendees'][0]['tastes'])
    print(f' ...saving meta')
    with open(f'{output_folder}/meta.json', 'w') as meta_file:
        meta = {
            'problem_id': problem_id,
            'width': problem['stage_width'],
            'height': problem['stage_height'],
            'instruments': instruments
        }
        meta_file.write(json.dumps(meta, indent=2))
    compute_radius = get_compute_radius((problem['room_width'], problem['room_height']), max_compute_radius=max_compute_radius)
    print(f' ...generating LUTs with compute radius {compute_radius}')
    luts = []
    for instrument_id in range(instruments):
        print(f'   ...instrument {instrument_id}...', end='')
        lut = generate_and_save_perception_map_by_instrument_id(problem, instrument_id, max_compute_radius=max_compute_radius)
    print(f' ...saved maps.')
    print(f'done for #{problem_id}')

if __name__ == "__main__":
    load_dotenv()
    max_compute_radius = int(os.environ.get('MAX_PERCEPTION_COMPUTE_RADIUS') or 10_000)
    if len(sys.argv) >= 3:
        problem_id = sys.argv[1]
        instrument_id = int(sys.argv[2])
        with open(f'problems/{problem_id}.json', 'r') as file:
            problem = json.loads(file.read())
        print(f'Generating perception maps for instrument {instrument_id}...')
        lut = generate_and_save_perception_map_by_instrument_id(problem, instrument_id, max_compute_radius=max_compute_radius)
        if len(sys.argv) >= 4 and sys.argv[3] == 'inspect':
            show_perception_map(lut)
    elif len(sys.argv) == 2:
        problem_id = sys.argv[1]
        generate_and_save_perception_maps(problem_id, max_compute_radius=max_compute_radius)
    else:
        problem_ids = get_problem_ids()
        print(f'generating {len(problem_ids)} perception maps...')
        for problem_id in problem_ids:
            generate_and_save_perception_maps(problem_id, max_compute_radius=max_compute_radius)
        print('all done.')
