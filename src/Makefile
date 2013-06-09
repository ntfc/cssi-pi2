OPT_OR_DEBUG=-O2
CFLAGS=-Wall -Wextra $(OPT_OR_DEBUG)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
LIBS=-lm
EXEC=lapin # exec file name
CC=gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -v $(EXEC) $(wildcard *.o)

