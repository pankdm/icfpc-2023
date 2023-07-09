import os
import itertools

def get_problem_ids():
  problem_ids = list(sorted([int(f[::-1].split('.',1)[1][::-1]) for f in os.listdir('problems/')]))
  return problem_ids
