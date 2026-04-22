CC = g++
CFLAGS = -std=c++17 -Wall
TARGET = pollserver
SRC = Main.cc

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean