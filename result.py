import sys
import subprocess

import matplotlib.pyplot as plt
import matplotlib.cbook as cbook

import numpy as np
import pandas as pd

x, y = np.loadtxt("data/result.csv", unpack=True, delimiter=';')

plt.plot(x, y)
plt.xlabel("Number of clients")
plt.ylabel("Mean of response time (ns/10)")
plt.show()