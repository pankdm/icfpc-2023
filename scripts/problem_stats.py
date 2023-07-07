import json

print("problem muscicians attendees tastes pillars")

for i in range(1, 45):
    path = "./problems/{}.json".format(i)
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data)
        # print(js.keys())
        tastes = len(set(js["musicians"]))
        print(
            "{}\t-> {}\t{}\t{}\t{}".format(
                i,
                len(js["musicians"]),
                len(js["attendees"]),
                tastes,
                len(js["pillars"]),
            )
        )
