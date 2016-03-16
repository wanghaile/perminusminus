The binary format for each graph is a sequence of integers:
```
remaining_int_length node_count
(node_type)+ (predecessors -1)+ (label)+ (features -1)+
```
  * he first integer indicates the length of the remaining sequence.
  * he second integer indicates the number of the nodes in this graph.
  * hen there is an array of length `node_count` that indicate the type of each node.
  * hen there is a sequence of `node_count` blocks. The i-th block contains the predecessors of the i-th nodes followed by `-1`.
  * hen there is an array of length `node_count` that indicate the label of each node.
  * inally the features for the nodes are stored like the predecessors.

This format is suitable for training and maybe test. When we want to release a program for specific purpose, we may need some changes.