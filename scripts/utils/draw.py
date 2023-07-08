def to_scale(preview_size, room, distance):
    [room_width, room_height] = room
    rmax = max(room_width, room_height)
    scale = preview_size / rmax
    return scale*distance

def point(preview_size, room, pos, yflip=True):
    [room_width, room_height] = room
    rmax = max(room_width, room_height)
    scale = preview_size / rmax
    x, y = pos
    _x = (rmax - room_width) / 2
    _y = (rmax - room_height) / 2
    if yflip:
        return (
            round(scale*(_x+x), 3),
            round(preview_size-scale*(_y+y), 3)
        )
    else:
        return (
            round(scale*(_x+x), 3),
            round(scale*(_y+y), 3)
        )

def rect(preview_size, room, xy, yflip=True):
    x0, y0, x1, y1 = xy
    _x0, _y0 = point(preview_size, room, (x0, y0), yflip=yflip)
    _x1, _y1 = point(preview_size, room, (x1, y1), yflip=yflip)
    rect = (
      _x0, _y0 if not yflip else _y1,
      _x1, _y1 if not yflip else _y0
    )
    return rect

def to_hex_component(channel_value):
    return hex(round(255*channel_value))[2:]

def to_hex_color(floats_color):
    with_alpha = len(floats_color) == 4
    r, g, b, a = floats_color if with_alpha else [*floats_color, 1.]
    return '#'+''.join([
        to_hex_component(r),
        to_hex_component(g),
        to_hex_component(b),
    ] + ([to_hex_component(a)] if with_alpha else []))

def to_floats_color(hex_color, add_alpha=False):
    hex_color = hex_color[1:]
    if len(hex_color) < 6:
        # pad shortcut colors like #aaa and #fba4
        hex_color = ''.join([c+c for c in hex_color])
    with_alpha = len(hex_color) == 8
    hex_parts = [hex_color[0+i:2+i] for i in range(0, len(hex_color), 2)]
    channel_values = [
        round(int(h, 16) / 255., 3) for h in hex_parts[:(4 if with_alpha else 3)]]
    if add_alpha and len(channel_values) < 4:
        channel_values.append(1.)
    return channel_values
