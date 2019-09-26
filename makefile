CC=gcc
CFLAGS= -lreadline
DEPS = main.h do.h pwd.h cd.h echo.h history.h ls.h pinfo.h nightswatch.h
OBJ = main.o do.o pwd.o cd.o echo.o history.o ls.o pinfo.c nightswatch.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

k7_shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)