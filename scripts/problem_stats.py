import json
import itertools
import os
from types import SimpleNamespace

print("problem musicians attendees tastes [w x h]")

for i in itertools.count(start=1):
    path = f"./problems/{i}.json"
    if not os.path.exists(path):
        break
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data, object_hook=lambda d: SimpleNamespace(**d))
        tastes_musicians = len(set(js.musicians))
        tastes_attendees = set(len(x.tastes) for x in js.attendees)
        assert len(tastes_attendees) == 1
        assert tastes_musicians == next(iter(tastes_attendees))
        print(
            "{:2} -> {:5} {:5} {:5} [{} x {}] ".format(
                i,
                len(js.musicians),
                len(js.attendees),
                tastes_musicians,
                int(js.stage_width),
                int(js.stage_height),
                # len(js.pillars),
            )
        )
