import sys
import json

if len(sys.argv) < 2:
    print('Usage: python ./solver.py PROBLEM_ID')
    exit(1)

with open(f'problems/{sys.argv[1]}.json', 'r') as file:
    problem = json.loads(file.read())

print("Hello json!")
print(json.dumps(problem, indent=2))
