/*****************************************************************************
​ ​* ​ ​ @file​ ​  		config.c
​ * ​ ​ @brief​ ​ 		Contains function which prints the execution instructions
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include <stdio.h>
#include "config.h"

void help(void)
{
	printf("\nThe executable is ""concurrent_tree"". The command line takes in 5 arguments.\n\n"
			"1. --test=high – testing high contention scenario (This case takes time depending on the number of threads and iterations)\n"
			"   --test=low – testing low contention scenario\n"
			"2. --lock=fg – Using fine grained locking with normal pthread locks\n"
			"   --lock=rw – Using fine grained locking with reader-writer locks\n"
			"3. -t – Number of threads. Minimum number of threads is 3. If less than 3 threads are specified, then the program automatically assumes 3 threads.\n"
			"4. -i – Number of iterations for each thread.\n"
			"5. --print=no - Print statements stating output of get operations and range query operations are omitted. Only the end results are displayed\n"
			"   --print=yes - All the print statements stating output of get and range query operations are included.\n"
			"\nIn case no argument are specified, the default values assumed are\n\n"
			"1. --test=low\n"
			"2. --lock=fg\n"
			"3. --t12\n"
			"4. -i1\n"
			"5. --print=yes\n"
			"\nRun “./unit_tests.sh” to run 12 tests with different arguments. None of these commands include print statements, therefore only the end summary results are displayed. The high contention commands take some time; therefore, patience is highly advised. If you wish to see the print outputs for each run, run the commands individually with no print argument.  Examples are provided below. These test cases present the following three results: \n"
			"\n 1. Verifying if the number of nodes inserted correspond to the number of nodes returned by an inorder traversal.\n"
			" 2. Verifying that each get operation either successfully returns the correct value corresponding to the key to be searched or successfully returns null if the key specified is not present. This operation is a success if number of successful get operations is equal to the number of get operations carried out.\n"
			" 3. Verifying that a range query function between the minimum key possible and maximum key possible returns as many number of nodes as that returned by an inorder traversal operation.\n"
			"\nTest cases corresponding to the unit tests script\n"
			"\nTest Case 1: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 12; Iterations - 100\n"
			"Test Case 2: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 12; Iterations - 100"
			"\nTest Case 3: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 12; Iterations - 5000"
			"\nTest Case 4: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 12; Iterations - 5000"
			"\nTest Case 5: Lock - Fine Grained pthread mutex; Test - low contention; Threads - 30; Iterations - 5000"
			"\nTest Case 6: Lock - Fine Grained pthread mutex; Test - high contention; Threads - 30; Iterations - 5000"
			"\nTest Case 7: Lock - Reader-Writer lock; Test - low contention; Threads - 12; Iterations - 100"
			"\nTest Case 8: Lock - Reader-Writer lock; Test - high contention; Threads - 12; Iterations - 100"
			"\nTest Case 9: Lock - Reader-Writer lock; Test - low contention; Threads - 12; Iterations - 5000"
			"\nTest Case 10: Lock - Reader-Writer lock; Test - high contention; Threads - 12; Iterations - 5000"
			"\nTest Case 11: Lock - Reader-Writer lock; Test - low contention; Threads - 30; Iterations - 5000"
			"\nTest Case 12: Lock - Reader-Writer lock; Test - high contention; Threads - 30; Iterations - 5000\n"
			"\nNote: Please refer to the project report for the commands corresponding to the results presented\n"
			"\n\nExamples:\n"
			"\n./concurrent_tree --test=low --lock=fg -t6 -i5\n"
			"\n./concurrent_tree --test=high --lock=fg -t12 -i2\n"
			"\n./concurrent_tree --test=low --lock=rw -t9 -i10\n"
			"\n./concurrent_tree --test=high --lock=rw -t24 -i1\n"

			"\nBased on the number of threads inputted, 1/3rd of threads are involved in the put operation, 1/3rd in get and the rest in the range queries operation."
			" Each thread involved with the get and put operation implement the get and put operation equal to the number of iterations."
			" The number of operations for range queries is equal to one-third of the number of total threads."
			" So, therefore, if the number of threads specified is 6 and number of iterations is 5, there will be a total of 10 put, 10 get and 2 range query operations.\n"
			"\n");
}