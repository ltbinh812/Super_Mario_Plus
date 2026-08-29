import json

def get_keys(d, path=''):
    res = {}
    if isinstance(d, dict):
        for k, v in d.items():
            res[path + '.' + k] = type(v).__name__
            res.update(get_keys(v, path + '.' + k))
    elif isinstance(d, list):
        if len(d) > 0:
            res.update(get_keys(d[0], path + '[]'))
    return res

d1 = json.load(open('assets/maps/map01/world01.ldtk', encoding='utf-8'))
d2 = json.load(open('saves/custom_map_0.ldtk', encoding='utf-8'))

k1 = get_keys(d1)
k2 = get_keys(d2)

missing = set(k1.keys()) - set(k2.keys())
print("Missing keys and types:")
for p in sorted(missing):
    print(f"{p}: {k1[p]}")
