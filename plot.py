import numpy as np
import matplotlib.pyplot as plt


data1 = np.loadtxt("build/output_1.txt")

data1 = np.float64(data1)


data = np.loadtxt("build/output.txt")

data = np.float64(data)

a = plt.hist(data, bins=500)
a1 = plt.hist(data1, bins=500)
plt.yscale("log")
plt.show()

print(np.sum(a1[0][92:]) / np.sum(a[0][92:]))
