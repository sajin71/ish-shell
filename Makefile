CFLAGS = -Wall -g
CC = gcc
TARGET = ish
SRCS := $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
RM = rm -f

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS) *~
