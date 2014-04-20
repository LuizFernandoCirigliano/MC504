CC = gcc
CFLAGS = -g -Wall -pthread -lncurses -lpanel
TARGET = the_senate_bus

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS)

clean:
	rm -f *~ *.o $(TARGET)
