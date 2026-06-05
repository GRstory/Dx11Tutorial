import math, random, re

r = 98

avoid_base = [
    (0,0,25),(180,0,25),(45,67.5,18),
    (40,67.5,7),(40,337.5,7),(40,247.5,7),(40,157.5,7),
    (75,45,7),(75,315,7),(75,225,7),(75,135,7),
    (110,67.5,7),(110,337.5,7),(110,247.5,7),(110,157.5,7),
    (60,180,22),(75,300,22),(95,90,22),
    (55,240,22),(110,150,22),(38,300,22),
    (125,270,22),(82,30,22),
    (114.1,26.1,10),(126.7,103.7,10),(132.3,206.3,10),(144.4,340.1,10),
    (113.5,357.5,10),(50.4,125.1,10),(142.6,58.1,10),(144.5,172.6,10),
]

def dist(c1,a1,c2,a2):
    c1,a1,c2,a2=map(math.radians,[c1,a1,c2,a2])
    d=math.sin(c1)*math.sin(c2)*math.cos(a1-a2)+math.cos(c1)*math.cos(c2)
    return math.degrees(math.acos(max(-1,min(1,d))))

def ok(c,a,avoid,buf=8):
    return all(dist(c,a,zc,za)>=zr+buf for zc,za,zr in avoid)

def pos(c,a):
    x=round(r*math.sin(math.radians(c))*math.sin(math.radians(a)),1)
    y=round(r*math.cos(math.radians(c)),1)
    z=round(r*math.sin(math.radians(c))*math.cos(math.radians(a)),1)
    return x,y,z

# Find wind farm cluster center
farm_center=None
for cc in range(20,160,5):
    for ca in range(0,360,5):
        if not ok(float(cc),float(ca),avoid_base,buf=8): continue
        avt=list(avoid_base)+[(float(cc),float(ca),0)]
        random.seed(13)
        test=[]
        for _ in range(2000):
            if len(test)>=5: break
            dc=random.uniform(-15,15); da=random.uniform(-15,15)
            c2=cc+dc; a2=(ca+da)%360
            if ok(c2,a2,avt,buf=3):
                test.append((c2,a2)); avt.append((c2,a2,3))
        if len(test)>=5:
            farm_center=(float(cc),float(ca)); break
    if farm_center: break

print(f"Wind farm center: colat={farm_center[0]}, az={farm_center[1]}")

avt=list(avoid_base)+[(farm_center[0],farm_center[1],0)]
random.seed(13)
turbines=[]
for _ in range(2000):
    if len(turbines)>=5: break
    dc=random.uniform(-15,15); da=random.uniform(-15,15)
    c=farm_center[0]+dc; a=(farm_center[1]+da)%360
    if ok(c,a,avt,buf=3):
        x,y,z=pos(c,a)
        turbines.append((x,y,z,round(c,1),round(a,1)))
        avt.append((c,a,3))

print(f"Turbines: {[f'({t[3]},{t[4]})' for t in turbines]}")

def fv(v): return f'{float(v):.1f}f'

new_vals = {
    'xPositions': [fv(t[0]) for t in turbines],
    'yPositions': [fv(t[1]) for t in turbines],
    'zPositions': [fv(t[2]) for t in turbines],
    'xRotations': [fv(t[3]) for t in turbines],
    'yRotations': [fv(t[4]) for t in turbines],
}

with open('Framework/graphicsclass.cpp','r',encoding='utf-8') as f:
    src=f.read()

def replace_last_n(src, vec_name, new_vals_list):
    """Replace the last N float values inside a vector initializer by position."""
    pat = rf'(vector<\S+> {vec_name} = \{{)(.*?)(\n\t\}};)'
    m = re.search(pat, src, re.DOTALL)
    if not m:
        print(f"  WARNING: {vec_name} not found")
        return src
    inner = m.group(2)
    # Find all float token positions (index and span)
    tokens = list(re.finditer(r'-?\d+\.\d+f', inner))
    n = len(new_vals_list)
    if len(tokens) < n:
        print(f"  WARNING: {vec_name} has only {len(tokens)} floats, need {n}")
        return src
    # Replace last n tokens back-to-front to preserve positions
    result = inner
    for tok, nv in zip(reversed(tokens[-n:]), reversed(new_vals_list)):
        result = result[:tok.start()] + nv + result[tok.end():]
    print(f"OK: {vec_name}  old_last={[t.group() for t in tokens[-n:]]}  new={new_vals_list}")
    return src[:m.start(2)] + result + src[m.end(2):]

for vec_name, vals in new_vals.items():
    src = replace_last_n(src, vec_name, vals)

with open('Framework/graphicsclass.cpp','w',encoding='utf-8') as f:
    f.write(src)
print("Done")
