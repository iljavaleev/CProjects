CC := gcc
CC_FLAGS := -lpthread
TARGET := cbst


all: $(TARGET)

$(TARGET): main.o bst.o
	$(CC) $^ -o $@ $(CC_FLAGS)

main.o: main.c  
	$(CC) $^ -o $@ -Wno-pointer-to-int-cast -c

bst.o: bst.c
	$(CC) $^ -o $@ -Wno-pointer-to-int-cast -c

clean:
	rm -f *.o