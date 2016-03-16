# Learning #

```
per~~.exe l <trainingfile> <modelfile> <iterations>
```

`per~~` does not know when to stop learning, so tell him the `<iterations>` as the times of iteration.

# Predicting #

```
per~~.exe p <modelfile> <testfile> <resultfile> <outputtype>
```

  * If `<outputtype>` is set to a positive integer n, it means that the n-best outputs are calculated.
  * If `<outputtype>` is 0, only the 1-best output are calculated.
  * If `<outputtype>` is -1, marginal scores for labels (the best score when a specific element labeled with a specific label) are contained in `<resultfile>`.
  * If `<outputtype>` is -2, the alpha values, "emission probabilities" and beta values (these terms are of Hidden Markov Models. note that alpha + beta - "emission probabilities" = marginal score) for labels are contained in `<resultfile>`.