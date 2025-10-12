CC = gcc
CFLAGS = `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm
TARGET = mandelbrot
SRC = mandelbrot.c
BIN = bin
OUT = $(BIN)/$(TARGET)

all: $(BIN) $(OUT)

$(BIN):
	mkdir -p $(BIN)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
