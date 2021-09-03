sourcefile = main.c
outputfile = psh
cc = clang-12
cstd = c18
cflags = -pthread -lreadline -std=$(cstd) -O3

all: compile execute

compile:
	$(cc) $(cflags) -o $(outputfile) $(sourcefile)

execute:
	./$(outputfile)
