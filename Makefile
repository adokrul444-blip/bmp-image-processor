CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2
LDFLAGS = -lm

TARGET = bmp
SRCS = main.c bmp.c transformations.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

main.o: main.c bmp.h transformations.h
	$(CC) $(CFLAGS) -c main.c

bmp.o: bmp.c bmp.h
	$(CC) $(CFLAGS) -c bmp.c

transformations.o: transformations.c transformations.h bmp.h
	$(CC) $(CFLAGS) -c transformations.c

clean:
	rm -f $(OBJS) $(TARGET)
