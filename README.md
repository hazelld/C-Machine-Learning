# C-Neural-Network 
This program is a very basic implementation of a feedforward neural network in C. It is more or less a proof of concept, as the plan is to port the NN to a functional language in the future. It uses a half-baked matrix library that was quickly thrown together as the basis for the operations performed. It uses a sigmoid activation function for the neuron's activation, and gradient descent backpropagation to train the network.  
  
# Install
    git clone git@github.com:whazell/C-Neural-Network.git  
    cd C-Neural-Network  
    mkdir build && cd build
    cmake ../
    make
    
# Usage  
    ./build/bin/examples/xor_test
    ./build/bin/examples/sin_test
    
Currently there are two basic examples for this neural network. The first is xor_test, which trains the network to predict the xor function. The second is the sin_test which predicts values from the sin() function.  

## xor_test
This uses a basic 2 input, 8 hidden, 1 output node structure. It can accuratly predict the function after a few hundred epochs. It loads the data from the file tests/xor.csv.

## sin_test
The is used to predict the sin() function, by feeding in normalized results from the sin() function (tests/sin-gen.c builds the data). It uses a single input node, 2 hidden layers of 5 nodes, and a single output layer. The data has to be normalized as the sigmoid function only produces values on the range (0,1) while the sin function produces on the range (-1, 1). By applying the transformation 0.5 * (sin(x) + 1), we can now train the neural network on this function, and undo the transformation after the value is predicted (ie (2 * result) - 1). This is very successful at predicting the values of sin().  


# API  
Using this net is fairly simple, as it has a small interface. There is a function provided in src/builder.c to load data from a csv file in the form: i1, i2, ... , in; o1, o2, ... , on -- where the i's are inputs and the o's are outputs. The main functions provided in the net are:  

init_net() -> Initializes the net with the given parameters.  
train() -> Train the net on the data.  
predict() -> Predict a specific value in the net.  

For example usage see either tests/xor_test.c or tests/sin_test.c
