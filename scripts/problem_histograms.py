import json
import os
import sys
import numpy as np
from problem_ids import get_problem_ids
from PIL import Image, ImageDraw
from matplotlib import pyplot as plt
from dotenv import load_dotenv

GLOBAL_SCORE_MULTIPLIER = 1_000_000
np.set_printoptions(suppress=True, precision=4)
arr = np.array

def get_problem(problem_id):
    with open(f'problems/{problem_id}.json', 'r') as file:
        problem = json.loads(file.read())
    return problem

def get_closest_point_on_rect(x, y, minX, minY, maxX, maxY, validate=False):
    # avoid 0/0 problem
    x += 0.000001
    if (minX < x and x < maxX) and (minY < y and y < maxY):
        raise Exception("Point " + [x,y] + "cannot be inside "
            + "the rectangle: " + [minX, minY] + " - " + [maxX, maxY] + ".")
    midX = (minX + maxX) / 2
    midY = (minY + maxY) / 2
    m = (midY - y) / (midX - x)
    if x <= midX: # check "left" side
        minXy = m * (minX - x) + y
        if minY <= minXy and minXy <= maxY:
            return [minX, minXy]
    if x >= midX: # check "right" side
        maxXy = m * (maxX - x) + y
        if minY <= maxXy and maxXy <= maxY:
            return [maxX, maxXy]
    if y <= midY: # check "top" side
        minYx = (minY - y) / m + x
        if minX <= minYx and minYx <= maxX:
            return [minYx, minY]
    if y >= midY: # check "bottom" side
        maxYx = (maxY - y) / m + x
        if minX <= maxYx and maxYx <= maxX:
            return [maxYx, maxY]
    # edge case when finding midpoint intersection: m = 0/0 = NaN
    if x == midX and y == midY:
        return [x, y]

def get_sq_distance_to_rect(point, rect_xy):
    x, y = point
    rx0, ry0, rx1, ry1 = rect_xy
    dx = max(rx0 - x, 0, x - rx1)
    dy = max(ry0 - y, 0, y - ry1)
    return dx*dx + dy*dy

def get_tastes_histogram(problem_id):
    problem = get_problem(problem_id)
    attendees = problem['attendees']
    stage_x0y0 = np.array(problem['stage_bottom_left'])
    stage_x1y1 = stage_x0y0 + np.array([problem['stage_width'], problem['stage_height']])
    stage_xy = np.ravel([stage_x0y0, stage_x1y1])
    # print('>>> stage', [*stage_x0y0, *stage_x1y1])
    tastes = np.array([a['tastes'] for a in attendees], dtype='double').T
    # print('>>> tastes', tastes)
    sq_distances = arr([
        get_sq_distance_to_rect([a['x'], a['y']], [*stage_x0y0, *stage_x1y1])
        for a
        in attendees
    ])
    # print('>>> stage sq distances', sq_distances)
    weighted_tastes = tastes / sq_distances
    # print('>>> weighted_tastes', weighted_tastes)
    sum_tastes = np.sum(weighted_tastes.T, 0)
    # print('>>> sum_tastes', sum_tastes)
    return sum_tastes

def plot_hist(name, x):
    abs_max = np.max(np.abs(x))
    plt.figure(name, figsize=(800,400), dpi=1)
    plt.axhline(0., color='black', linewidth=100)
    plt.gca().set_ylim([-abs_max*1.05,abs_max*1.05])
    plt.axis('off')
    plt.bar(np.mgrid[0:x.shape[0]:1.], x, width=1.)

def plot_and_save_hist(problem_id, name, x):
    plot_hist(name, x)
    folder = f'histograms/{problem_id}/'
    os.makedirs(folder, exist_ok=True)
    plt.savefig(f'{folder}/{name}.png', bbox_inches='tight', pad_inches=5., transparent=True)
    plt.close()

def np_log10p(x):
    abs_max = np.max(np.abs(x))
    pos_vals = np.clip(x, 0., abs_max + 1.)
    neg_vals = np.clip(x, -abs_max - 1., 0.)
    return np.log(pos_vals+1.) - np.log(-neg_vals+1.)

def generate_histograms(problem_id):
    tastes_hist = get_tastes_histogram(problem_id)
    tastes_log_hist = np_log10p(tastes_hist)
    histograms = [
        [ 'Tastes', 'tastes', tastes_hist ],
        [ 'Log Tastes', 'tastes.log', tastes_log_hist ],
    ]
    return histograms

if __name__ == "__main__":
    load_dotenv()
    if len(sys.argv) >= 3:
        pass
    elif len(sys.argv) == 2:
        problem_id = sys.argv[1]
        histograms = generate_histograms(problem_id)
        # save images
        for _, filename, hist in histograms:
            plot_and_save_hist(problem_id, filename, hist)
        # preview images
        for name, _, hist in histograms:
            plot_hist(name, hist)
        plt.show()
    else:
        problem_ids = get_problem_ids()
        print(f'generating {len(problem_ids)} histograms...')
        for problem_id in problem_ids:
            print(f'problem: {problem_id}')
            histograms = generate_histograms(problem_id)
            # save images
            for _, filename, hist in histograms:
                print('  saving histogram: ' + filename)
                plot_and_save_hist(problem_id, filename, hist)
            # if problem_id == 4:
            #     break
        print('all done.')
