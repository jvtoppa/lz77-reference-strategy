import subprocess
import os

if(os.path.exists("data/banana.txt.sa")):
    os.remove("data/banana.txt.sa")
    print("Deleted banana.txt.sa")
subprocess.run(["./build/gensa data/banana.txt data/banana.txt.sa"], shell=True)
subprocess.run(["./build/count data/banana.txt"], shell=True)