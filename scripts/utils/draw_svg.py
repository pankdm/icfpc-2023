import tempfile
import subprocess
import os
import math
from cairo import SVGSurface, Context
from .draw import point, rect, to_scale, to_floats_color
from uuid import uuid4

def draw_rect(preview_size, draw_ctx: Context, room, xy, color='#fff', yflip=True):
    _rect = rect(preview_size, room, xy, yflip=yflip)
    _x0, _y0, _x1, _y1 = _rect
    # print('>>>>> svg', _rect)
    draw_ctx.set_source_rgba(*to_floats_color(color, add_alpha=True))
    draw_ctx.rectangle(_x0, _y0, _x1-_x0, _y1-_y0)
    draw_ctx.fill()

def draw_circle(preview_size, draw_ctx: Context, room, pos, radius, color='#fff', yflip=True):
    _pos = point(preview_size, room, pos, yflip=yflip)
    r = to_scale(preview_size, room, radius)
    draw_ctx.set_source_rgba(*to_floats_color(color, add_alpha=True))
    draw_ctx.arc(*_pos, r, 0, 2*math.pi)
    draw_ctx.fill()

def draw_image(preview_size, room, objects, yflip=True):
    file = tempfile.NamedTemporaryFile(delete=False, suffix='.svg')
    surface = SVGSurface(file, preview_size, preview_size)
    draw_ctx = Context(surface)
    for name, _type, *args in objects:
        if _type == 'rect':
            draw_fn = draw_rect
        if _type == 'circle':
            draw_fn = draw_circle
        # print('>>>>> drawing:', name, _type, args)
        draw_fn(preview_size, draw_ctx, room, *args)
    # draw_ctx.save()
    surface.finish()
    file.flush()
    file.close()
    return file, surface, draw_ctx

def to_png_file(svg_surface: SVGSurface):
    png_file = tempfile.NamedTemporaryFile(delete=False, suffix='.png')
    svg_surface.write_to_png(png_file)
    return png_file

def draw_png_image(preview_size, room, objects, yflip=True):
    file, svg_surface, draw_ctx = draw_image(preview_size, room, objects, yflip=True)
    png_file = to_png_file(svg_surface)
    return png_file, svg_surface, draw_ctx
