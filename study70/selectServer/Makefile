CC = g++
CFLAGS = -std=c++17 -Wall
TARGET = Epollserver
SRC = Main.cc

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: clean