SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
TARGET = tetris
CC := gcc
CFLAGS = -Wall
CFLAGS += -Os

$(TARGET): $(patsubst %.c, %.o, $(SRC))
	$(CC) $(CFLAGS) -o $(TARGET) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	cp $(TARGET) /usr/bin/
	chown root:root /usr/bin/$(TARGET)
	chmod 0755 /usr/bin/$(TARGET)
	
clean:
	rm -f *.o *~ $(TARGET) Makefile.dep

Makefile.dep: $(SRC) $(HDR)
	$(CC) -MM *.c > Makefile.dep

-include Makefile.dep
