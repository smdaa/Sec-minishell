SOURCES = minishell.c process.c readcmd.c

all : minishell
minishell : minishell.o process.o readcmd.o
	gcc -Wall -o $@ $^
clean : 
	rm *.o minishell
depend : 
	makedepend *.c -Y.$(SOURCES)