CC = g++

concurrent_tree: main.o fine_grained_lock_BST.o rw_lock_BST.o config.o
		$(CC) -o concurrent_tree -pthread main.o fine_grained_lock_BST.o rw_lock_BST.o config.o

main.o: main.c fine_grained_lock_BST.h rw_lock_BST.h config.h
		$(CC) -c -pthread main.c

fine_grained_lock_BST.o: fine_grained_lock_BST.c fine_grained_lock_BST.h config.h
		$(CC) -c -pthread fine_grained_lock_BST.c

rw_lock_BST.o: rw_lock_BST.c rw_lock_BST.h config.h
		$(CC) -c -pthread rw_lock_BST.c

config.o: config.c config.h
		$(CC) -c config.c

clean: 
		rm -f *.o
		rm -f concurrent_tree