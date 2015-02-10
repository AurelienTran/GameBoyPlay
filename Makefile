TARGET=PlayEmulator
SOURCE=$(wildcard src/*.c)
OBJECT=$(patsubst %.c, %.o, $(SOURCE))

CFLAGS= -std=c99 -Wall

check: $(TARGET)
	./$(TARGET)

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECT)

.PHONY: all check clean

$(TARGET): $(OBJECT)
	$(CC) $^ -o $@
