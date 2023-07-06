import sys
import json
from src.utils import meaning_of_life

def solve(problem_id=None):
    problem_id = problem_id or (len(sys.argv) >= 2 and sys.argv[1])
    if not problem_id:
        print('Usage: python solver.py PROBLEM_ID')
        exit(1)

    with open(f'problems/{sys.argv[1]}.json', 'r') as file:
        problem = json.loads(file.read())

    print("Hello json!")
    print(json.dumps(problem, indent=2))

if __name__ == '__main__':
    solve()
