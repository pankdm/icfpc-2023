import json


def meaning_of_life():
    return 42


def load_problem(i):
    path = "./problems/{}.json".format(i)
    with open(path, "r") as f:
        data = f.read()
        js_data = json.loads(data)
        return js_data
