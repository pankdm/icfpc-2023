import json

print("problem muscicians attendees pillars")

for i in range(1, 45):
    path = "./problems/{}.json".format(i)
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data)
        # print(js.keys())
        print(
            "{}\t-> {}\t{}\t{}".format(
                i, len(js["musicians"]), len(js["attendees"]), len(js["pillars"])
            )
        )
