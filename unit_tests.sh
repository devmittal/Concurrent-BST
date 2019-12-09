#!/bin/bash

echo -e "\n\033[4mTest Case 1: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 12; Iterations - 100\033[0m"
./concurrent_tree --lock=fg --test=low --print=no -t12 -i100

echo -e "\n\033[4mTest Case 2: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 12; Iterations - 100\033[0m"
./concurrent_tree --lock=fg --test=high --print=no -t12 -i100

echo -e "\n\033[4mTest Case 3: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 12; Iterations - 5000\033[0m"
./concurrent_tree --lock=fg --test=low --print=no -t12 -i5000

echo -e "\n\033[4mTest Case 4: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 12; Iterations - 5000\033[0m"
./concurrent_tree --lock=fg --test=high --print=no -t12 -i5000

echo -e "\n\033[4mTest Case 5: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 30; Iterations - 5000\033[0m"
./concurrent_tree --lock=fg --test=low --print=no -t30 -i5000

echo -e "\n\033[4mTest Case 6: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 30; Iterations - 5000\033[0m"
./concurrent_tree --lock=fg --test=high --print=no -t30 -i5000

echo -e "\n\033[4mTest Case 7: Lock - Reader-Writer lock; Test - low contention; Threads - 12; Iterations - 100\033[0m"
./concurrent_tree --lock=rw --test=low --print=no -t12 -i100

echo -e "\n\033[4mTest Case 8: Lock - Reader-Writer lock; Test - high contention; Threads - 12; Iterations - 100\033[0m"
./concurrent_tree --lock=rw --test=high --print=no -t12 -i100

echo -e "\n\033[4mTest Case 9: Lock - Reader-Writer lock; Test - low contention; Threads - 12; Iterations - 5000\033[0m"
./concurrent_tree --lock=rw --test=low --print=no -t12 -i5000

echo -e "\n\033[4mTest Case 10: Lock - Reader-Writer lock; Test - high contention; Threads - 12; Iterations - 5000\033[0m"
./concurrent_tree --lock=rw --test=high --print=no -t12 -i5000

echo -e "\n\033[4mTest Case 11: Lock - Reader-Writer lock; Test - low contention; Threads - 30; Iterations - 5000\033[0m"
./concurrent_tree --lock=rw --test=low --print=no -t30 -i5000

echo -e "\n\033[4mTest Case 12: Lock - Reader-Writer lock; Test - high contention; Threads - 30; Iterations - 5000\033[0m"
./concurrent_tree --lock=rw --test=high --print=no -t30 -i5000