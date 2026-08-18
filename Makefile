CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -g -MMD -MP
TARGET  := mycc

SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,build/%.o,$(SRCS))
DEPS    := $(OBJS:.o=.d)

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<

build:
	mkdir -p build

test: $(TARGET)
	./test.sh

clean:
	rm -rf build $(TARGET) *.s

-include $(DEPS)