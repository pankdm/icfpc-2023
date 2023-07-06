import logging
import subprocess
from concurrent.futures import ThreadPoolExecutor
import os
from dotenv import load_dotenv
from flask_cors import CORS
from flask import Flask, request, send_from_directory

from solvers.python import hello_json
from .api import icfpc as ICFPC
from .utils import get_sanitized_args
import numpy as np

logger = logging.getLogger(__name__)


load_dotenv()

app = Flask(__name__)
CORS(app)

os.path.basename(__file__)

IMG_CACHE = {}


def open_image_as_np(n):
    global IMG_CACHE

    if n in IMG_CACHE:
        return IMG_CACHE[n]

    img = Image.open(f"./problems/{n}.png")
    a = np.asarray(img)
    result = a[::-1, :].swapaxes(0, 1)
    IMG_CACHE[n] = result
    return result


@app.route("/")
def ping():
    return "pong"


@app.post("/check-auth")
def post_check_auth():
    return ICFPC.check_auth()


@app.get("/problems/<id>")
def get_problem(id):
    return send_from_directory('../problems', id+'.json')

@app.post("/run_solver")
def post_run_solver():
    payload = request.get_json()
    problem_id = payload["problem_id"]
    lang = payload["lang"]
    solver = payload["solver"]

    if lang == 'cpp':
        done = subprocess.run(
            [f'solvers/cpp/{solver}/solver', str(problem_id)],
            capture_output=True,
            text=True
        )
        return { "output": str(done.stdout), "code": done.returncode }
    elif lang == 'python':
        done = subprocess.run(
            ['python', f'solvers/python/{solver}/solver.py', str(problem_id)],
            capture_output=True,
            text=True
        )
        return { "output": str(done.stdout), "code": done.returncode }


@app.get("/problems")
@app.get("/problems/")
def get_problems():
    problems_dir = os.path.dirname(__file__)+'/../problems'
    problems_files = os.listdir(problems_dir)
    problems = sorted([int(p.rstrip('.json'))
                      for p in problems_files if '.json' in p])
    return { 'problems': problems }

def get_all_solutions(nickname=None):
    solutions_dir = os.path.dirname(__file__)+'/../solutions'
    solutions_folders_with_files = []
    for path, folders, files in os.walk(solutions_dir):
        solutions_folders_with_files.append(
            (path[len(solutions_dir + '/'):], files))
    solutions = []
    for path, files in solutions_folders_with_files:
        for file in files:
            solutions.append(path+'/'+file if path else file)
    if nickname:
        return [s for s in sorted(solutions) if nickname in s]
    return sorted(solutions)

@app.get("/solutions")
@app.get("/solutions/")
def get_solutions():
    return {'solutions': sorted(get_all_solutions())}


@app.get("/solutions/<nickname>")
@app.get("/solutions/<nickname>/")
def get_user_solutions(nickname):
    return {'solutions': sorted(get_all_solutions(nickname))}

@app.get("/solutions/<path:path>")
def get_solution(path):
    print(path)
    return send_from_directory('../solutions', path)

@app.post("/solutions/<problem_id>")
def post_solution(problem_id):
    payload = request.get_json()
    solution = payload.get('solution')
    assert solution, 'Required input: solution'
    return ICFPC.submit(problem_id, solution)


@app.get("/best_solutions/<path:path>")
def get_best_solution(path):
    print('get_best_solution::', path)
    solutions_dir = os.path.dirname(__file__)+'/../solutions_best'
    timestamps = os.listdir(solutions_dir)
    last_ts = max(timestamps)
    print (last_ts)

    for solution in os.listdir(f"{solutions_dir}/{last_ts}"):
        if solution.startswith(f"{path}_"):
            print ('Best = ', solution)
            return send_from_directory(f"{solutions_dir}/{last_ts}", solution)
    return ""


@app.get('/icfpc/<path:path>')
def get_icfpc_endpoint(path):
    result = ICFPC.proxy_get_endpoint(path)
    return result

@app.errorhandler(Exception)
def unhandled_error(error):
    logging.exception(error)
    return str(error), getattr(error, 'code', 500)