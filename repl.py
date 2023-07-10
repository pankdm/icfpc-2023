import importlib

from dotenv import load_dotenv
load_dotenv()

from server import server
from server.api import icfpc as ICFPC


def reload_modules():
  importlib.reload(server)
  importlib.reload(ICFPC)

print('Welcome to ICFPC-2023!')
print('Team: Snakes, Monkeys and Two Smoking Lambdas')
print('')
