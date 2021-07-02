FILE = test.tig
TIGER_COMPILER = src/driver/dtiger
CLANG = clang++
LIB = src/runtime/posix/libruntime.a
all:
	$(TIGER_COMPILER) -o test.o $(FILE)
	$(CLANG) test.o $(LIB) -o test
