This example is to use `per~~.exe` to do Chinese word segmentation (CWS).

We model CWS as character sequence labeling like [some other researchers did](http://nlp.csai.tsinghua.edu.cn/~zkx/cws/bib.html).

The data we used is the MSR corpus in SIGHAN bakeoff 2005. You can download the data [there](http://www.sighan.org/bakeoff2005/) for free. File `msr_training.utf8` (13.9MB) is used as the training data. File `msr_test_gold.utf8` is used as the test data.

The file `cws_transform.py` is used to transform segmented Chinese text files to binary files that `per~~.exe` can read. The command lines are:

```
c:\python31\python.exe cws_transform.py l "msr_training.utf8" "training.bin" "index.json"

c:\python31\python.exe cws_transform.py p "index.json" "msr_test_gold.utf8" "test.bin"
```

These processes are illustrated as the figure bellow:

![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/cws.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/cws.png)

Then we use these command lines to learn and predict.

```
per~~.exe l "training.bin" "model.bin" 20

per~~.exe p "model.bin" "test.bin" "result.txt" 0
```
which can be illustrated as the figure bellow:

![http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/flow.png](http://i1220.photobucket.com/albums/dd455/zhangkaixu/per--/flow.png)

In the learning processing, it takes only 6 seconds for each iteration (20 iteration totally)! And the memory usage is only 83MB!

Though we model CWS as character labeling, the evaluation is based on words, not characters. Finally, the `eval.py` file is used for the evaluation.