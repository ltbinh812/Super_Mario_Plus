import json

d1 = json.load(open('assets/maps/map01/world01.ldtk', encoding='utf-8'))
d2 = json.load(open('saves/custom_map_0.ldtk', encoding='utf-8'))

def get_arrays(d, path=''):
    res = set()
    if isinstance(d, dict):
        for k, v in d.items():
            if isinstance(v, list):
                res.add(path + '.' + k)
                res.update(get_arrays(v, path + '.' + k))
            else:
                res.update(get_arrays(v, path + '.' + k))
    elif isinstance(d, list):
        for i, v in enumerate(d):
            res.update(get_arrays(v, path + '[]'))
    return res

a1 = get_arrays(d1)
a2 = get_arrays(d2)
print("Missing arrays in custom:", sorted(a1 - a2))
