# Define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
# Define libraries to be linked (for example -lm)
LDLIBS = -lm

# Define sets of source files and object files
SRC = data.c dict3.c dict4.c 
 
# Define the executable name(s)
EXE = dict3 dict4

# The first target:
dict3: data.o dict3.o
	gcc $(CFLAGS) -o dict3 data.o dict3.o

dict4: data.o dict4.o
	gcc $(CFLAGS) -o dict4 data.o dict4.o

# Other targets specify how to get the .o files
data.o: data.c data.h
	gcc $(CFLAGS) -c data.c 

dict3.o: dict3.c
	gcc $(CFLAGS) -c dict3.c

dict4.o: dict4.c 
	gcc $(CFLAGS) -c dict4.c

clean:
	rm -f *.o dict3 dict4

