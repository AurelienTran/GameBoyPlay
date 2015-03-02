TARGET=PlayEmulator
SOURCE=$(wildcard src/*.c)
OBJECT=$(patsubst %.c, %.o, $(SOURCE))

CFLAGS= -std=c99 -Wall -Wextra -g -Isrc

all: $(TARGET)

check: $(TARGET)
	./$(TARGET) rom/bootstrap.bin

clean:
	$(RM) $(TARGET) $(OBJECT)

.PHONY: all check clean

$(TARGET): $(OBJECT)
	$(CC) $^ -o $@
