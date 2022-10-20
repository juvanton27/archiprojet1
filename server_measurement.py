# Handle args and UNIX cmd
import sys
import subprocess

# Handle OS signals
import signal

# Handle graphs
import matplotlib.pyplot as plt
import matplotlib.cbook as cbook
import numpy as np
import pandas as pd


server = None
bwm = None

def launch(port: int, threads: int, fileSize: int):
    # Launch server
    server = subprocess.Popen(
        ['./server', '-j', str(threads), '-s', str(fileSize), '-p', str(port)])

    # Record traffic on localhost
    with open("data/server.log", "w") as outfile:
        bwm = subprocess.Popen(
            ['tcpdump', '-i', 'lo0', '-B', '1024', 'port', '2241'], stdout=outfile)

def signal_handler(sig, frame):
    with open("data/server.log", "r") as logs:
        counter = 0
        total = 0
        for index, log in enumerate(logs.readlines()):
            if index%2==0:
                try:
                    length = int(log.split('length')[1])
                    print(length)
                    if length != 0:
                        counter+=1
                    total+=length
                except IndexError:
                    print("error")
        print(total/counter)

if __name__ == "__main__":
    # Prepare environment
    subprocess.call(['make', 'clean'])
    subprocess.call(['make'])

    # Launch server and generate log file of trafic recorded
    launch(int(sys.argv[1]), int(sys.argv[2]), sys.argv[3])

    # Wait for SIGINT
    signal.signal(signal.SIGINT, signal_handler)
    signal.pause()
