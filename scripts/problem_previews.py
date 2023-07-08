import json
import sys
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw

def generate_problem_preview(id, preview_size=512, attendee_size=10, attendee_color='white', scene_color='#3163aa'):
    with open(f'problems/{id}.json', 'r') as file:
        problem = json.loads(file.read())

    room_width = problem['room_width']
    room_height = problem['room_height']
    rmax = max(room_width, room_height)
    scale = preview_size / rmax
    def rect(x0, y0, x1, y1, yflip=True):
        _x = (rmax - room_width) / 2
        _y = (rmax - room_height) / 2
        if not yflip:
            return (scale*(_x+x0), scale*(_y+y0), scale*(_x+x1), scale*(_y+y1))
        else:
            return (scale*(_x+x0), preview_size-scale*(_y+y1), scale*(_x+x1), preview_size-scale*(_y+y0))
    def point(x, y, yflip=True):
        _x = (rmax - room_width) / 2
        _y = (rmax - room_height) / 2
        if not yflip:
            return (scale*(_x+x), scale*(_y+y))
        else:
            return (scale*(_x+x), preview_size-scale*(_y+y))
    stage_width = problem['stage_width']
    stage_height = problem['stage_height']
    stage_x0, stage_y0 = problem['stage_bottom_left']
    stage_x1, stage_y1 = stage_x0 + stage_width, stage_y0 + stage_height
    image = Image.new('RGBA', (preview_size, preview_size))
    draw = ImageDraw.Draw(image)
    # draw.rectangle(rect(0, 0, 0, 0), fill='black')
    draw.rectangle(rect(0, 0, room_width, room_height), fill='#444')
    draw.rectangle(rect(stage_x0, stage_y0, stage_x1, stage_y1), fill=scene_color)
    attendees = problem['attendees']
    for a in attendees:
        draw.ellipse(rect(a['x']-attendee_size/2, a['y']-attendee_size/2, a['x']+attendee_size/2, a['y']+attendee_size/2), fill=attendee_color, width=attendee_size)
    return image



if __name__ == "__main__":
    if len(sys.argv) > 1:
        image = generate_problem_preview(sys.argv[1])
        image.show()
    else:
        for i in get_problem_ids():
            image = generate_problem_preview(i)
            image.save(f'previews/{i}.png', quality=100)
