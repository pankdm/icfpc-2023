import json
import sys
import numpya as np
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw
from scripts.utils.draw import draw_rect, draw_circle

def generate_problem_perception_map(id):
    with open(f'problems/{id}.json', 'r') as file:
        problem = json.loads(file.read())
    room_width = problem['room_width']
    room_height = problem['room_height']
    rmax = max(room_width, room_height)
    scale = preview_size / rmax
    stage_width = problem['stage_width']
    stage_height = problem['stage_height']
    stage_x0, stage_y0 = problem['stage_bottom_left']
    stage_x1, stage_y1 = stage_x0 + stage_width, stage_y0 + stage_height
    image = Image.new('RGBA', (preview_size, preview_size))
    draw = ImageDraw.Draw(image)
    draw.rectangle(rect(0, 0, room_width, room_height), fill='#444')
    draw.rectangle(rect(stage_x0, stage_y0, stage_x1, stage_y1), fill=scene_color)
    pillars = problem['pillars']
    for p in pillars:
        draw_circle(*p['center'], p['radius'], color=pillar_color)
    attendees = problem['attendees']
    for a in attendees:
        draw_circle(a['x'], a['y'], attendee_size, color=attendee_color)
    return image



if __name__ == "__main__":
    if len(sys.argv) > 1:
        image = generate_problem_perception_map(sys.argv[1])
        image.show()
    else:
        problem_ids = get_problem_ids()
        print(f'generating {len(problem_ids)} previews...')
        for i in problem_ids:
            image = generate_problem_perception_map(i)
            image.save(f'previews/{i}.png', quality=100)
            print(f'#{i}')
        print('all done.')
