import json
import sys
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw
import shutil
import subprocess
from utils.draw_svg import draw_image as draw_svg_image, draw_png_image

def generate_problem_preview(id, preview_size=512, attendee_size=5, attendee_color='#ffffff', pillar_color='#e72626', stage_color='#3163aa', bg_color='#444444', image_type='png'):
    with open(f'problems/{id}.json', 'r') as file:
        problem = json.loads(file.read())
    room_width = problem['room_width']
    room_height = problem['room_height']
    room = [room_width, room_height]
    stage_width = problem['stage_width']
    stage_height = problem['stage_height']
    stage_x0, stage_y0 = problem['stage_bottom_left']
    stage_x1, stage_y1 = stage_x0 + stage_width, stage_y0 + stage_height
    if image_type == 'svg':
        draw_image = draw_svg_image
    elif image_type == 'png':
        draw_image = draw_png_image

    image = draw_image(preview_size, room, [
        ['bg', 'rect', (0, 0, room_width, room_height), bg_color],
        ['stage', 'rect', (stage_x0, stage_y0, stage_x1, stage_y1), stage_color],
        *[
            ['pillar', 'circle', p['center'], p['radius'], pillar_color]
            for p in problem['pillars']
        ],
        *[
            ['attendee', 'circle', [a['x'], a['y']], attendee_size, attendee_color]
            for a in problem['attendees']
        ],
    ])
    return image

if __name__ == "__main__":
    if len(sys.argv) > 1:
        problem_id = sys.argv[1]
        image_type = 'svg'
        if len(sys.argv) >= 3:
            image_type = sys.argv[2]
        file, img_surface, draw_ctx = generate_problem_preview(problem_id, image_type=image_type)
        subprocess.call(['open', file.name])
    else:
        image_type = 'svg'
        problem_ids = get_problem_ids()
        print(f'generating {len(problem_ids)} previews...')
        for i in problem_ids:
            file, _, _ = generate_problem_preview(i, image_type=image_type)
            shutil.copy(file.name, f'previews/{i}.{image_type}')
            print(f'#{i}')
        print('all done.')
