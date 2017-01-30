all: src/libstring.c
	$(CC) -c src/libstring.c -o libstring.o -Iinclude/ -Wall -Werror -pedantic
	ar rcs libstring.a libstring.o

clean:
	rm -rf libstring.o libstring.a docs/ tests/runner

test: tests/runner.c clean all
	@$(CC) -c tests/runner.c -o tests/runner.o -Iinclude/ -Wall -Werror
	@$(CC) tests/runner.o -L. -lstring -o tests/runner
	@echo "Running test..."
	@tests/runner

testv:
	@echo "Running test with valgrind"
	@valgrind --track-origins=yes --leak-check=full tests/runner 

docs:
	doxygen Doxyfile
	@echo "Docs generated to docs/"
