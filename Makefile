CC = gcc
CFLAGS = -Wall -g -I./include

objects = main.o list.o echo.o concatenate.o make_dir.o \
		remove_dir.o remove_file.o change_dir.o change_mode.o \
		print_working_dir.o word_count.o date.o signal_kill.o \
		who.o history.o help.o shell_exit.o

.PHONY: all
all: shelldon clean

shelldon: $(objects)
	$(CC) $(CFLAGS) -o shelldon $(objects)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(objects)