# Graham-Häggkvist conjecture
Various code related to the Graham-Häggkvist conjecture.

Code is released under MIT License, except for freetree.h, which should have it's own license,
but I don't know which one. Probably you should contact Frank Ruskey about it. A wrapped version of freetree is called gentreeg.c in the current version of nauty, which is distributed with the Apache licence.

TODO: Write smth about:
* what has been coded
* what is the relation between Häggkvist conjecture and Graham-Häggkvist conjecture (we can combine them into 1 conjecture which also orients edges in the tree - every vertex is either a sink or a source)
* and that in the code i also use the restriction that we have a corresponding 2-factorization of the regular graph
* Häggkvist's bold proposal about decomposition into random set of trees of the same size (code is not finished yet)
* And about possible combination with 2-factorization (but now without orientation) (also no code, just a probable experiment, which could easily fail)
* also mention that there is no universal 2-factorization, which works for all trees already when the number of vertices is 4
