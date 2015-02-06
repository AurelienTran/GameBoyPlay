TARGET=PlayEmulator
SOURCE=$(wildcard src/*.c)
OBJECT=$(patsubst %.c, %.o, $(SOURCE))

check: $(TARGET)
	./$(TARGET)

all: $(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECT)

.PHONY: all check clean

$(TARGET): $(OBJECT)
	$(CC) $< -o $@
