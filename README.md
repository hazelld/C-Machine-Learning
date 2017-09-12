# C-Machine-Learning
This program is a very basic implementation of a feedforward neural network in C. It is more or less a proof of concept, as the plan is to port the NN to a functional language in the future. It uses a half-baked matrix library that was quickly thrown together as the basis for the operations performed. It uses a sigmoid activation function for the neuron's activation, and gradient descent backpropagation to train the network.  
  
# Install
    git clone git@github.com:whazell/C-Machine-Learning.git  
    cd C-Machine-Learning 
    mkdir build && cd build
    cmake ../
    make
    
# Examples  
This program comes with 2 examples, sin_test and xor_test. Their usage is outlined below.  

## Usage  
    ./build/bin/examples/xor_test
    ./build/bin/examples/sin_test
    
Currently there are two basic examples for this neural network. The first is xor_test, which trains the network to predict the xor function. The second is the sin_test which predicts values from the sin() function.  

## xor_test
This uses a basic 2 input, 8 hidden, 1 output node structure. It can accuratly predict the function after a few hundred epochs. It loads the data from the file tests/xor.csv.

## sin_test
The is used to predict the sin() function, by feeding in normalized results from the sin() function (tests/sin-gen.c builds the data). It uses a single input node, 2 hidden layers of 5 nodes, and a single output layer. The data has to be normalized as the sigmoid function only produces values on the range (0,1) while the sin function produces on the range (-1, 1). By applying the transformation 0.5 * (sin(x) + 1), we can now train the neural network on this function, and undo the transformation after the value is predicted (ie (2 * result) - 1). This is very successful at predicting the values of sin().  

# Tests  
This project is built with the [munit](https://github.com/nemequ/munit) test suite. The tests are built automatically when cmake is ran. They build the tests into the directory `build/test/`. Currently ther are the test suites:  
 - `matrix_test`  
 
Run with:  `./build/test/[test_suite_name]`
 
# API  

All public interface functions are preceeded by 'gn' to ensure no namespace conflicts. Currently, the API consists of very basic functions and is not stable at all.
