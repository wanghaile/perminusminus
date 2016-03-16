This is the old front page for this project. Now the goal of this project is to provide a toolkit to deal with the PathLabeling problem.

Per-- is an implementation of Averaged Perceptron (Freund and Schapire, 1999) for [Sequence Labeling](http://en.wikipedia.org/wiki/Sequence_labeling) under the [KISS principle](http://en.wikipedia.org/wiki/KISS_principle) (Keep It Simple and Stupid).

The model that Per-- deals with is illustrated in the following figure. If this is a good model for your problems (Part-of-Speech tagging, Chinese word segmentation et al.), you can try Per--.

![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/model.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/model.png)

Sequence labeling is to label each element (blue box) of a sequence. Features (nodes in gray) of elements are observable and independent. Labels (nodes in white) are hidden and have linear relations. Labels are also related with features of the same elements.

## Download ##
The binary file `per--.exe` and a demo are provided [here](http://code.google.com/p/perminusminus/downloads/list). And feel free (under the New BSD License) to use the source code. Tell me if you use it on your research.

## Usage ##
### Data Format ###
There is one data format for both training data and test data, which is described as follows:

```
datafile ::= (sequence newline) +
newline ::= "\r\n" | "\n" | "\r" | ...
sequence ::= (element newline) +
element ::= label_id (white_space feature_id) *
label_id ::= nonnegative_integer
feature_id ::= nonnegative_integer
```

Here is an example:

```
0 0
1 1 2
2
3 2 3

0 0 1
1

```

Just regard it as a rotated version of the figure we showed. Note that only nonnegative integers are allowed. If you have more clever data types (like strings) as the labels or features, index them for Per-- first.

### Command Line ###

For learning, the command line is:

```
per--.exe l <trainingfile> <iterations> <modelfile>
```

Per-- does not know when to stop learning, so tell him `<iterations>` as the times of iteration.

For predicting, the command line is:

```
per--.exe p <modelfile> <testfile> <resultfile> <outputtype>
```

If `<outputtype>` is set to a positive integer n, it means that the n-best label sequences are calculated.

If `<outputtype>` is 0, only the best label sequence are calculated in `<resultfile>`. The result is the same as `<outputtype>=1`, but the speed is a little bit faster.

If `<outputtype>` is -1, the orginal label sequences in `<testfile>` are remained in `<resultfile>` (for your evaluation).

If `<outputtype>` is -2, marginal scores for labels (the best score when a specific element labeled with a specific label) are remained in `<resultfile>`.

If `<outputtype>` is -3, the alpha values, "emission probabilities" and beta values (these terms are of Hidden Markov Models. note that alpha + beta - "emission probabilities" = marginal score) for labels are remained in `<resultfile>`.

### Example ###
Suppose we have a file `train.txt` of 3 sequences as the training file. The label\_id (first column in the file) ranges from 0 to 2, while the feature\_id ranges from 0 to 3.

```
0 0 1 2 3
1 2 1
2 3
1 0
0 1 2 3

0 2
0 3
1 2
2 3 2
1 2
1 2
1 1 3
2 3

1 0
0 1
1 3
2 2

```

We also have a test file `test.txt` of one sequence:

```
0 1 2
1
2 3 2
2

```

We use the command line to train a model and test it on the test data.

```
per--.exe l train.txt 10 model.bin
per--.exe p model.bin test.txt result.txt -1
```

The `result.txt` will be:

```
0 0
1 1
2 2
2 1

```

The first column contains the label\_id provided in the `test.txt`, while the second column contains the label\_id predicted by the trained model. How simple it is!

## References ##

  * Freund, Y. & Schapire, R. (1999). Large Margin Classification using the Perceptron Algorithm. In _Machine Learning_, 37(3):277-296.