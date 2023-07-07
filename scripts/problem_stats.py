import json

print("problem muscicians attendees tastes [w x h]")

for i in range(1, 45):
    path = "./problems/{}.json".format(i)
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data)
        # print(js.keys())
        tastes = len(set(js["musicians"]))
        print(
            "{:2} -> {:5} {:5} {:5} [{} x {}] ".format(
                i,
                len(js["musicians"]),
                len(js["attendees"]),
                tastes,
                int(js["stage_width"]),
                int(js["stage_height"]),
                # len(js["pillars"]),
            )
        )
