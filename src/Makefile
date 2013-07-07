OPT_OR_DEBUG=-O2
CFLAGS=-Wall -Wextra $(OPT_OR_DEBUG) $(DEF)
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
LIBS=-lm
EXEC=lapin # exec file name
#DEF=-DURANDOM -DDEBUG
#DEF=-DPERFORMANCE
CC=gcc

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -v $(wildcard *.o) $(EXEC) 

