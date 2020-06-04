CC = gcc
CFLAGS = -g -Werror -Wextra

build: tema3

tema3: interpret.o tree.o cmd.o main.o queue.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c -o $@

cmd.o: commands_utils.c
	$(CC) $(CFLAGS) $^ -c -o $@

tree.o: tree_utils.c
	$(CC) $(CFLAGS) $^ -c -o $@

interpret.o: interpret.c
	$(CC) $(CFLAGS) $^ -c -o $@

queue.o: queue.c
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm *.o tema3
