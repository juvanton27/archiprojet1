import sys
import subprocess

import matplotlib.pyplot as plt
import matplotlib.cbook as cbook

import numpy as np
import pandas as pd

rate_to_measure = [2, 10]
mean = {}

for val in rate_to_measure:
  with open(f'data/response_time_{val}.log') as logs:
    counter = 0
    response_time = 0
    for log in logs.readlines():
      response_time += int(log)
      counter+=1
    mean[val] = 1/response_time/counter

lists = sorted(mean.items())
x, y = zip(*lists)

plt.plot(x, y)
plt.xlabel("Request rate (req/sec)")
plt.ylabel("Mean of response time (ns/10)")
plt.show()