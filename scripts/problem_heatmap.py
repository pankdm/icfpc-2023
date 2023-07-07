import sys
import json
from types import SimpleNamespace
from math import *
import matplotlib.pyplot as plt
import numpy as np

def show_heatmap(problem_id=None, taste=None):
    with open(f'problems/{problem_id}.json', 'r') as file:
        problem = json.loads(file.read(), object_hook=lambda d: SimpleNamespace(**d))

    points = np.zeros(shape=(int(problem.room_height) + 1, int(problem.room_width) + 1), dtype=float)
    dx, dy = problem.stage_bottom_left
    dx, dy = int(dx), int(dy)
    w, h = int(problem.stage_width), int(problem.stage_height)

    for y in range(dy, dy+h+1):
        for x in range(dx, dx+w+1):
            for at in problem.attendees:
                d = dist([x, y], [at.x, at.y])
                points[y][x] += ceil(1_000_000 * at.tastes[taste] / (d * d))

    plt.imshow(points[dy:dy+h+1, dx:dx+w+1], cmap='hot', interpolation='bicubic', norm='log')
    plt.show()

if __name__ == "__main__":
    if (len(sys.argv) != 3):
        print('Usage: python problem_heatmap.py PROBLEM_ID')
        exit(1)

    problem_id = int(sys.argv[1])
    taste = int(sys.argv[2])
    show_heatmap(problem_id=problem_id, taste=taste)

