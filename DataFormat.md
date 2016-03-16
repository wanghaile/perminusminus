# Training & Test Data #

Training data and test data have uniform form. The per-- toolkit now can directly read data in the binary files. We also provide a Python3 script to transform JSON file to binary file.

## Binary File ##
The binary file linearly consists of binary data of graphs.
The binary data for each graph is a sequence of integers:

```
<remaining_int_length> <node_count> (<node_type>)+ (<predecessors> -1)+ (<label>)+ (<features> -1)+
```

  * The first integer indicates the length of the remaining sequence.
  * The second integer indicates the number of the nodes in this graph.
  * Then there is an array of length `<node_count>` that indicate the type of each node.
  * Then there is a sequence of `<node_count>` blocks. The i-th block contains the predecessors of the i-th nodes followed by `-1`.
  * Then there is an array of length `<node_count>` that indicate the label of each node.
  * Finally the features for the nodes are stored like the predecessors.

This format is suitable for training and maybe test. When we want to release a program for specific purpose, we may need some changes.

Here is the explanations for the `<node_type>`:
  * 0 indicates that this node is neither an initial node nor a terminal node.
  * 1 indicates that this node is an initial node but not a terminal node.
  * 2 indicates that this node is an terminal node but not a initial node.
  * 3 indicates that this node is both an initial node and a terminal node.

## JSON File ##

Each line of the file is a JSON string represents a graph:
```
[node, ...]
```
Each node is a list with 4 elements:
```
[node_type, [predecessor, ...], label, [feature_id, ...]]
```
We provide a Python3 script to transform JSON file to binary file.

# Model #
The header of the model has two integers to indicate the size of the label set (`l_size`) and the size of the feature set (`f_size`), respectively.

Then two linearly stored matrix is following. One is the weight matrix for label pairs. The other is the weight matrix for feature-label pairs.

So the total size of model file is `1 + 1 + l_size * l_size + f_size * l_size`.

# Result #