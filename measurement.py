# Must have bwm-ng installed

import subprocess


def launch(ip: str, port: int, nbClient: int, threads: int, fileSize: int, keySize: int, requestRate: int, time: int):
    # Prepare environment
    subprocess.call(['make', 'clean'])
    subprocess.call(['make'])

    # Launch server
    server = subprocess.Popen(
        ['./server', '-j', str(threads), '-s', str(fileSize), '-p', str(port)])

    # Record traffic on localhost
    bwm = subprocess.Popen(['bwm-ng', '-I', 'lo0', '-o', 'csv', '-F', 'bwm.csv'])

    # Launch client(s) and wait for them to finish
    clients = []
    for i in range(nbClient):
        clients.append(subprocess.Popen(['./client', '-k', str(keySize), '-r',
                                         str(requestRate), '-t', str(time), f'{ip}:{port}']))
    for client in clients:
        client.wait()

    # Free resources
    bwm.kill
    server.kill


launch('localhost', 2241, 4, 4, 4, 4, 1000, 10)
