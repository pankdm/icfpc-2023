import tempfile
from PIL import Image, ImageDraw
from .draw import point, rect, to_floats_color

def draw_rect(preview_size, draw_ctx: ImageDraw.Draw, room, xy, color='#fff', yflip=True):
    _rect = rect(preview_size, room, xy, yflip=yflip)
    print('>>>>> png', _rect)
    draw_ctx.rectangle(_rect, fill=color)

def draw_circle(preview_size, draw_ctx: ImageDraw.Draw, room, pos, radius, color='#fff', yflip=True):
    x, y = pos
    draw_ctx.ellipse(rect(preview_size, room, (x-radius/2, y-radius/2, x+radius/2, y+radius/2), yflip=yflip), fill=color)

def draw_image(preview_size, room, objects, yflip=True):
    file = tempfile.NamedTemporaryFile(delete=False, suffix='.png')
    image = Image.new('RGBA', (preview_size, preview_size))
    draw_ctx = ImageDraw.Draw(image, mode='RGBA')
    for name, _type, *args in objects:
        if _type == 'rect':
            draw_fn = draw_rect
        if _type == 'circle':
            draw_fn = draw_circle
        # print('>>>>> drawing:', name, _type, args)
        draw_fn(preview_size, draw_ctx, room, *args)
    image.save(file)
    return file, None, None
