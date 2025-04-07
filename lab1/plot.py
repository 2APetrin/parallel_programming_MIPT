import matplotlib.pyplot as plt

def read_arrays(path):
    ret = []
    with open(str(path), 'r') as f:
        lines = f.readlines()
        for line in lines:
            ret.append([float(x) for x in line.strip().split()])
    return ret

data = read_arrays('.tmp/out.data')

fig, ax = plt.subplots(figsize=(10,8))

for arr in data:
    plt.plot(arr)


ax.grid(which='major')
ax.minorticks_on()
ax.grid(which='minor', linestyle=':')

plt.savefig("png.png")
plt.show()