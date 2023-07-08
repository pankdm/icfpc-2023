import math
from dotenv import load_dotenv
from functools import lru_cache, cache
from random import random, randrange, sample

from src.utils import load_solution, Problem, solution_score, write_solution, BORDER_RADIUS


load_dotenv()

def compute_score(x, y, px, py, taste):
    d2 = (x - px) ** 2 + (y - py) ** 2
    return int(math.ceil(10**6 * taste / d2))


def simulated_annealing(problem, solution: list[tuple[int, int]]=None, T=1000, cooling_rate=0.99):
    pr = Problem(problem)
    
    @cache
    def dumb_music_score(x, y, taste_id):
        """Score for a single musician of taste at position (x, y) not accounting for blocking by other musicians"""
        return sum(compute_score(x, y, px, py, pr.att_tastes[i][taste_id]) for i, (px, py) in enumerate(pr.attendees))

    # generate a grid of fixed possible positions for musicians
    gridw = int(pr.stage_size[0]//BORDER_RADIUS)
    gridh = int(pr.stage_size[1]//BORDER_RADIUS)
    grid = [(pr.stage_bottom_left[0]+i*BORDER_RADIUS, pr.stage_bottom_left[1]+j*BORDER_RADIUS ) for j in range(1, gridh) for i in range(1, gridw) ]
    gridn = len(grid)
    if gridn<pr.nmus:
        print("Not enough grid points for all musicians")
        return None
    print(gridw, gridh, gridn)
    # generate a list of n random positions for musicians
    score_sum = 0
    score_sum_squares = 0
    for j in range(100):
      sol = sample(range(gridn), pr.nmus)
      score = sum(dumb_music_score(grid[i][0], grid[i][1], pr.mus_tastes[m]) for m,i in enumerate(sol))
      score_sum += score
      score_sum_squares += score**2
      if j%10==0:
        print(score)
    print("Average random score:", score_sum/100)
    stdev = math.sqrt(score_sum_squares/100 - (score_sum/100)**2)
    print("Average stdev", stdev)
    T = stdev**0.5
    solset = set(sol)
    for i in range(1000):
      for t in range(500):
        # generate a random neighbour solution
        if random()<0.1:
          # swap two musicians
          m1=randrange(pr.nmus)
          m2=randrange(pr.nmus)
          while m2==m1:
            m2=randrange(pr.nmus)
          scorenew = score - dumb_music_score(grid[sol[m1]][0], grid[sol[m1]][1], pr.mus_tastes[m1]) - dumb_music_score(grid[sol[m2]][0], grid[sol[m2]][1], pr.mus_tastes[m2])
          sol[m1], sol[m2] = sol[m2], sol[m1]
          scorenew += dumb_music_score(grid[sol[m1]][0], grid[sol[m1]][1], pr.mus_tastes[m1]) - dumb_music_score(grid[sol[m2]][0], grid[sol[m2]][1], pr.mus_tastes[m2])
          if scorenew>score or random()<math.exp((scorenew-score)/T):
            score = scorenew
          else:
            sol[m1], sol[m2] = sol[m2], sol[m1]
        else:
          # move a musician
          m1=randrange(pr.nmus)
          mold = sol[m1]
          for j in range(10):
            mnew = randrange(gridn)
            if mnew not in solset:
              break
          else:
            continue
          scorenew = score - dumb_music_score(grid[sol[m1]][0], grid[sol[m1]][1], pr.mus_tastes[m1])
          solset.remove(mold)
          sol[m1] = mnew
          solset.add(mnew)
          scorenew += dumb_music_score(grid[sol[m1]][0], grid[sol[m1]][1], pr.mus_tastes[m1])
          if scorenew>score or random()<math.exp((scorenew-score)/T):
            score = scorenew
          else:
            sol[m1] = mold
            solset.remove(mnew)
            solset.add(mold)

      if i%50==0:
        print(f"T: {T}", "score", score)
      T *= cooling_rate

    solution = [grid[i] for i in sol]
    return solution, solution_score(problem, solution)


if __name__ == "__main__":
    for id in range(1, 56):
      pr = Problem(id)
      if len(pr.attendees)>700:
        continue
      sol, score = simulated_annealing(id)
      print(id, score)
      write_solution(id, "xivaxy", sol)

