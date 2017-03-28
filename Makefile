.PHONY: all
all: src/libstring.c
	$(CC) -c src/libstring.c -o libstring.o -Iinclude/ -Wall -Werror -pedantic -ansi -g
	ar rcs libstring.a libstring.o

.PHONY: printf
printf: src/libstring_printf.c all
	$(CC) -c src/libstring_printf.c -o libstring_printf.o -Iinclude/ -Wall -Werror
	ar rcs libstring.a libstring_printf.o

.PHONY: unicode
unicode: src/libstring.c
	$(CC) -c src/libstring.c -o libstring.o -Iinclude/ -Wall -Werror -pedantic -ansi -DUSE_UNICODE
	ar rcs libstring.a libstring.o

.PHONY: clean
clean:
	rm -rf libstring.o libstring_printf.o libstring.a docs/ tests/runner tests/runner.o

.PHONY: test
test: tests/runner.c printf
	@$(CC) -c tests/runner.c -o tests/runner.o -Iinclude/ -Wall -Werror -g
	@$(CC) tests/runner.o -L. -lstring -o tests/runner
	@echo "Running test..."
	@tests/runner

.PHONY: testv
testv:
	@echo "Running test with valgrind"
	@valgrind --track-origins=yes --leak-check=full tests/runner 

.PHONY: docs
docs:
	@echo "Generating README"
	@echo "~~~~~" > README.md
	@cat README >> README.md
	@echo "~~~~~" >> README.md
	doxygen Doxyfile
	@rm -rf README.md
	@echo "Docs generated to docs/"
