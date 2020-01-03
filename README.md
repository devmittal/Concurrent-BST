# Concurrent Binary Search Tree

A key-value store binary search tree which is MT-safe, protected using fine grained locking, i.e, hand-over-hand locking. Using command-line arguments, user can use pthread mutex locks or reader/writer locks.

This tree supports 3 operations:
- Adding a node
- Viewing the value of a node corresponding to a key
- Viewing the range of values and keys present in the tree within a high and low key. 

Refer [this](https://github.com/devmittal/Concurrent-BST/blob/master/Mittal_Report.pdf) for additional details on the algorithm, user inputs and how to compile and run the program.
