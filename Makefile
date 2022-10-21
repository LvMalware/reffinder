BINARY=reffinder
CFLAGS=-Ilib -Wall -O2 -lcurl -lpthread
SOURCES=main.c src/*.c

all: $(SOURCES)
	$(CC) $(SOURCES) -o $(BINARY) $(CFLAGS)

clean:
	rm -rf $(BINARY)
	find . -type f -iname '*~' -delete
