all: src/libstring.c
	gcc -c src/libstring.c -o libstring.o -Iinclude/ -Wall -Werror -pedantic
	ar rcs libstring.a libstring.o

clean:
	rm -rf libstring.o libstring.a docs/

test: tests/runner.c clean all
	@gcc -c tests/runner.c -o tests/runner.o -Iinclude/ -Wall -Werror -g
	@gcc tests/runner.o -L. -lstring -o tests/runner
	@echo "Running test..."
	@tests/runner

testv:
	@echo "Running test with valgrind"
	@valgrind --track-origins=yes tests/runner 

docs:
	doxygen Doxyfile
	@echo "Docs generated to docs/"
