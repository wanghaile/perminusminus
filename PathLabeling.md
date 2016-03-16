# Path Labeling #
Path labeling is a generalized model for many problems. This model is illustrated as the figure bellow.

![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/pl.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/pl.png)

Suppose we have a DAG (Directed Acyclic Graph) with nodes (in blue boxes) shown above. The directions of the edges are determined by the indexes of the nodes.

Each node may have some features (in gray) which are observable. We can also label (in white) some of the nodes.

Path labeling task is to find a path from a initial node (diamond shape) to a terminal node (double circle) and label each node on this path.

# Special Cases #
This is the hierarchy of the models that this toolkit will deal with.
![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/models.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/models.png)

  * The most complex model in this hierarchy is "**path labeling**", which is to find a path in a DAG and give each node on this path a label.
  * If the set size of the possible labels is one, the model degrades to "**path finding**", which is just to find a path in a DAG.
  * If there is only one possible path for each DAG (in such case, we call this DAG a sequence), the "path labeling" degrades to "**sequence labeling**" which is a popular model for many NLP tasks.
  * If there is only one node for each sequence for the "sequence labeling" model, it further degrades to a "**classification**" model.