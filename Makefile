CC = cc

concurrent_tree: main.o
		$(CC) -o concurrent_tree -pthread main.o

main.o: main.c
		$(CC) -c -pthread main.c

clean: 
		rm -f *.o
		rm -f concurrent_tree