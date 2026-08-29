import json

def get_ints(d, path=''):
    res = set()
    if isinstance(d, dict):
        for k, v in d.items():
            if isinstance(v, (int, float)) and not isinstance(v, bool):
                res.add(path + '.' + k)
            else:
                res.update(get_ints(v, path + '.' + k))
    elif isinstance(d, list):
        for i, v in enumerate(d):
            res.update(get_ints(v, path + '[]'))
    return res

d1 = json.load(open('assets/maps/map01/world01.ldtk', encoding='utf-8'))
d2 = json.load(open('saves/custom_map_0.ldtk', encoding='utf-8'))

i1 = get_ints(d1)
i2 = get_ints(d2)

print("Missing ints in custom:")
for p in sorted(i1 - i2):
    print(p)
