# Introduction #
Per-- is a toolkit of [Averaged Perceptron](http://cseweb.ucsd.edu/~yfreund/papers/LargeMarginsUsingPerceptron.pdf) (Freund and Schapire, 1999) for [path Labeling](PathLabeling.md) under the [KISS principle](http://en.wikipedia.org/wiki/KISS_principle) (Keep It Simple and Stupid).

# Features #
  * Simple and efficient
  * Multiple usage: path labeling, path finding, sequence labeling and classification
  * Can provide n-best outputs
  * Can provide "marginal score"
  * Can provide alpha (forward) values and beta (backward) values using [forward-backward algorithm](http://www.cs.brown.edu/research/ai/dynamics/tutorial/Documents/HiddenMarkovModels.html)
  * Open source

# Download #
Go [there](http://code.google.com/p/perminusminus/downloads/list) to download the binary files.
  * `per~~`: executable file for path labeling
  * `per--`: executable file for sequence labeling

Download the [source code](http://code.google.com/p/perminusminus/source/checkout) to build your own binary files on Windows or Linux.

## Requirements ##
  * gcc compiler (if you want to rebuild the toolkit)
  * python3 (optional)

# Usage #
This data flow is the same with any other machine learning framework:

![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/flow.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/flow.png)

[Data format](DataFormat.md)

[Learning and Predicting](LearningAndPredicting.md)

[An example for Chinese word segmentation](CWSExample.md)
# References #

  * Freund, Y. & Schapire, R. (1999). Large Margin Classification using the Perceptron Algorithm. In _Machine Learning_, 37(3):277-296.