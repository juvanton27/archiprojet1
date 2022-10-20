import sys
import subprocess

import matplotlib.pyplot as plt
import matplotlib.cbook as cbook

import numpy as np
import pandas as pd

def launch(ip: str, port: int, nbClient: int, keySize: int, requestRate: int, time: int):
    clients = []
    for i in range(nbClient):
        clients.append(subprocess.Popen(['./client', '-k', str(keySize), '-r',
                                         str(requestRate), '-t', str(time), f'{ip}:{port}']))
    for client in clients:
        client.wait()

def mean():
    with open("data/response_time.log", "r") as response_time:
        total = 0
        counter = 0
        for line in response_time.readlines():
            if any(char.isdigit() for char in line):
                total+=int(line)
                counter+=1
    return total/counter


if __name__ == "__main__":
    # Select what to test
    choice = input("Number of (c)lients | Number of (r)equests per second\n")

    # Prepare environment
    subprocess.call(["make", "clean"])
    subprocess.call(["make"])

    # Calculate mean response time / number of clients
    if choice == 'c':
        nbClients = [1, 10, 25]
        responseTime = []

        for i in nbClients:
            # Launch client
            launch(sys.argv[1], int(sys.argv[2]), i,
                int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
            print("fni")
            responseTime.append(mean())

        plt.plot(nbClients, responseTime)
        plt.xlabel("Number of users")
        plt.ylabel("Mean of response time (ns/10)")
        plt.show()
        
    # Calculate mean response time / request rate
    elif choice == 'r':
        requestRates = [10, 100, 1000]
        responseTime = []

        for i in requestRates:
            # Launch client
            launch(sys.argv[1], int(sys.argv[2]), 1,
                int(sys.argv[3]), i, int(sys.argv[5]))
            
            responseTime.append(mean())

        plt.plot(requestRates, responseTime)
        plt.xlabel("Request rate per second")
        plt.ylabel("Mean of response time (ns/10)")
        plt.show()
