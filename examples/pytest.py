#!/usr/bin/python3

'''
    Super quick script to test the python API.

    Performs the same tests that sin_test.c does.


    Ensure the script is being run from the root project directory. A check is done 
    to make sure that the cnn.so library exists in the following location:

    build/lib/
'''

import sys
import math
import os

cnn_lib_path = 'build/lib/'
lib_folder = os.getcwd() + '/' + cnn_lib_path

# Add the library to the path
if lib_folder not in sys.path:
    sys.path.insert(0, lib_folder)

# Should be able to import the library now
import cnn as cnn

# Some basic setup values
topology = [1, 5, 5, 1]
learning_rate = 0.1
success = 0

def die_on_failure (code, statement):
    if ret != success:
        print(statement)
        sys.exit(1)


ret = cnn.init_net(topology, learning_rate)
die_on_failure(ret, "cnn.init_net failed")

ret = cnn.test_train('examples/data/sin.csv')
die_on_failure(ret, "cnn.test_train failed")

testval = math.pi
ret = cnn.test_predict(testval)

# Need to transform the ret value (see sin_test.c for explination)
ret = 2 * ret - 1

print("Actual value: ")
print("sin(" + str(testval) + ") = " + str(math.sin(math.pi)) + "\n")
print("python cnn sin_test:")
print("sin(" + str(testval) + ") = " + str(ret))

