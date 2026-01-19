CC := gcc
TARGET := popen_impl

all: $(TARGET)

$(TARGET): main.o cpopen.o
	$(CC) $^ -o $@

main.o: main.c  
	$(CC) $^ -o $@ -c

cpopen.o: cpopen.c
	$(CC) $^ -o $@ -c

clean:
	rm -f *.o 