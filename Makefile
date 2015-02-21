TARGET=PlayEmulator
SOURCE=$(wildcard src/*.c)
OBJECT=$(patsubst %.c, %.o, $(SOURCE))

CFLAGS= -std=c99 -Wall -g -Isrc

check: $(TARGET)
	./$(TARGET) rom/bootstrap.bin

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECT)

.PHONY: all check clean

$(TARGET): $(OBJECT)
	$(CC) $^ -o $@
