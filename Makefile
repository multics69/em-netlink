CC = gcc
CFLAGS = -Wall -O2 $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0)
LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0)
TARGET = em-netlink
SRC = em-netlink.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
