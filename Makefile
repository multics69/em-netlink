CC = gcc

CFLAGS = -std=gnu11 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow
CFLAGS += $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0)
ifeq ("$(DEBUG)","1")
  CFLAGS += -g -fsanitize=address -fsanitize=leak -static-libasan
endif

LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0)
TARGET = em-netlink

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,${SRCS})

include $(wildcard *.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

clean:
	rm -f *.o *.d *~ $(TARGET)

distclean: clean

.PHONY: all clean distclean
.DEFAULT_GOAL=all

