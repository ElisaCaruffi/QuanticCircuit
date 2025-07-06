COMP = gcc -std=c90 -Wall
LDFLAGS = -lm

SRCS = main.c data.c result.c
OBJS = $(SRCS:.c=.o)

TARGET = eseguito.out

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(COMP) -o $@ $^ $(LDFLAGS)
%.o: %.c
	$(COMP) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean