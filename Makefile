all:
	gcc -o test main.c json_utils.c -I.
clean:
	rm test
