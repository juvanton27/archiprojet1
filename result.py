import sys
import subprocess

import matplotlib.pyplot as plt
import matplotlib.cbook as cbook

import numpy as np
import pandas as pd

rate_to_measure = [4, 8, 16, 32, 64]
mean = {}

for val in rate_to_measure:
  with open(f'data/phase4/response_time_{val}.log') as logs:
    counter = 0
    response_time = 0
    for log in logs.readlines():
      response_time += int(log)
      counter+=1
    mean_interval_s = response_time/counter/1000000
    lambda_s = 1/mean_interval_s
    print(f'Mean response time for {val} : {mean_interval_s}s')
    print(f'Lambda for {val} : {lambda_s}')
    mean[val] = mean_interval_s

lists = sorted(mean.items())
x, y = zip(*lists)

plt.plot(x, y)
plt.title("Response time by request rate")
plt.xlabel("Request rate (req/sec)")
plt.ylabel("Mean of response time (1/1.000.000 s)")
plt.show()