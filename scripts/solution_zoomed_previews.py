import json
import sys
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw
import shutil
import subprocess
from utils.draw_svg import draw_image as draw_svg_image, draw_png_image
import os

def generate_problem_preview(solution, id, preview_size=512, attendee_size=5, attendee_color='#ffffff', pillar_color='#e72626', stage_color='#3163aa', bg_color='#444444', image_type='png'):
    with open(f'problems/{id}.json', 'r') as file:
        problem = json.loads(file.read())

    solution_path = f'./solutions/{solution}/{id}.json'
    if os.path.exists(solution_path):
        with open(solution_path, 'r') as file:
            solution = json.loads(file.read())
            placements = solution['placements']
    else:
        return None, None, None
        placements = []

    

    musician_size = 5
    musician_color = '#ffd700'

    room_width = problem['room_width']
    room_height = problem['room_height']
    room = [room_width, room_height]
    stage_width = problem['stage_width']
    stage_height = problem['stage_height']
    stage_x0, stage_y0 = problem['stage_bottom_left']
    stage_x1, stage_y1 = stage_x0 + stage_width, stage_y0 + stage_height

    offset_x0 = max(0, stage_x0 - 0.1 * stage_width)
    offset_y0 = max(0, stage_y0 - 0.1 * stage_height)

    view_width = (stage_x0 + 1.1 * stage_width) - offset_x0
    view_height = (stage_y0 + 1.1 * stage_height) - offset_y0

    if image_type == 'svg':
        draw_image = draw_svg_image
    elif image_type == 'png':
        draw_image = draw_png_image

    def apply_offset(pt):
        x, y = pt
        return [x - offset_x0, y - offset_y0]

    image = draw_image(preview_size, [view_width, view_height], [
        ['bg', 'rect', (0, 0, view_width, view_height), bg_color],
        ['stage', 'rect', (stage_x0 - offset_x0, stage_y0 - offset_y0, stage_x1 - offset_x0, stage_y1 - offset_y0), stage_color],
        *[
            ['pillar', 'circle', apply_offset(p['center']), p['radius'], pillar_color]
            for p in problem['pillars']
        ],
        *[
            ['attendee', 'circle', apply_offset([a['x'], a['y']]), attendee_size, attendee_color]
            for a in problem['attendees']
        ],
        *[
            ['musician', 'circle', apply_offset([m['x'], m['y']]), musician_size, musician_color]
            for m in placements
        ],
    ])
    return image

if __name__ == "__main__":
    solution = sys.argv[1]
    solution_path = f"./solutions/{solution}"
    if not os.path.exists(solution_path):
        print ("Solution path not found: ", solution_path)
    image_type = 'svg'
    problem_ids = get_problem_ids()
    print(f'generating {len(problem_ids)} previews for solution {solution}...')

    path = f"./previews_zoomed/{solution}"
    if not os.path.exists(path):
        os.mkdir(path)

    for i in problem_ids:
        file, _, _ = generate_problem_preview(solution, i, image_type=image_type)
        # Skip non-existing solutions
        if file is None:
            continue
        shutil.copy(file.name, f'{path}/{i}.{image_type}')
        print(f'#{i}')
    print('all done.')
