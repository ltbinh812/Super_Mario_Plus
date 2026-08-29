import json

def get_types(d, path=''):
    res = {}
    if isinstance(d, dict):
        for k, v in d.items():
            res[path + '.' + k] = type(v).__name__
            res.update(get_types(v, path + '.' + k))
    elif isinstance(d, list):
        if len(d) > 0:
            res.update(get_types(d[0], path + '[]'))
    return res

d1 = json.load(open('assets/maps/map01/world01.ldtk', encoding='utf-8'))
d2 = json.load(open('saves/custom_map_0.ldtk', encoding='utf-8'))

t1 = get_types(d1)
t2 = get_types(d2)

print("Type mismatches:")
for p in t1:
    if p in t2 and t1[p] != t2[p]:
        if t1[p] == 'NoneType' or t2[p] == 'NoneType':
            continue
        print(f"{p}: world01={t1[p]}, custom={t2[p]}")
