import subprocess

def launch(ip:str, port:int, nbClient: int, threads:int, fileSize:int, keySize:int, requestRate:int, time:int):
  # Prepare environment
  subprocess.call(['make', 'clean'])
  subprocess.call(['make'])

  # Launch client/server
  server = subprocess.Popen(['./server', '-j', '4', '-s', '4', '-p', '2241'])
  for i in range(nbClient):
    subprocess.call(['./client', '-k', '4', '-r', '1000', '-t', '10', '127.0.0.1:2241'])

  # Do measurement
  

  # Free resources
  server.kill

launch('localhost', 2241, 1, 4, 4, 4, 1000, 10)