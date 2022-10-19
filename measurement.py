import sys
import subprocess

import matplotlib.pyplot as plt
import matplotlib.cbook as cbook

import numpy as np
import pandas as pd

# Must have bwm-ng installed
# The CSV output format of bwm-ng is designed as
# timestamp;iface_name;bytes_out/s;bytes_in/s;bytes_total/s;bytes_in;bytes_out;packets_out/s;packets_in/s;packets_total/s;packets_in;packets_out;errors_out/s;errors_in/s;errors_in;errors_out;bits_out/s;bits_in/s;bits_total/s;bits_in;bits_out\n

# Launch server/client and generate CSV file of trafic recorded


def launch(ip: str, port: int, nbClient: int, threads: int, fileSize: int, keySize: int, requestRate: int, time: int):
    # Prepare environment
    subprocess.call(['make', 'clean'])
    subprocess.call(['make'])

    # Launch server
    server = subprocess.Popen(
        ['./server', '-j', str(threads), '-s', str(fileSize), '-p', str(port)])

    # Record traffic on localhost
    bwm = subprocess.Popen(
        ['bwm-ng', '-I', 'lo0', '-t', '1000', '-o', 'csv', '-F', 'bwm.csv'])

    # Launch client(s) and wait for them to finish
    clients = []
    for i in range(nbClient):
        clients.append(subprocess.Popen(['./client', '-k', str(keySize), '-r',
                                         str(requestRate), '-t', str(time), f'{ip}:{port}']))
    for client in clients:
        client.wait()

    # Free resources
    bwm.send_signal(9)
    server.send_signal(9)


if __name__ == "__main__":
    # launch(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(
    #     sys.argv[5]), int(sys.argv[6]), int(sys.argv[7]), int(sys.argv[8]))


    timestamp, bytes_out_per_s, packets_out_per_sec = np.loadtxt(
        'bwm.csv', unpack=True, delimiter=';', usecols=(0, 2, 7))

    plt.plot(timestamp, packets_out_per_sec)
    plt.xlabel("Time")
    plt.ylabel("Packets out/s")
    plt.show()