import os
import itertools

def get_problem_ids():
  problem_ids = []
  for i in itertools.count(start=1):
      path = f"./problems/{i}.json"
      if not os.path.exists(path):
          break
      problem_ids.append(i)
  return problem_ids
