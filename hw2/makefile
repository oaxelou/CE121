CC=gcc
CC_FLAGS=-Wall -g

all: p2crypt p2archive dirlist p2unarchive hw2

p2crypt: p2crypt.c
	$(CC) $(CC_FLAGS) p2crypt.c -o p2crypt

p2archive: p2archive.c
	$(CC) $(CC_FLAGS) p2archive.c -o p2archive

dirlist: dirlist.c
	$(CC) $(CC_FLAGS) dirlist.c -o dirlist

p2unarchive: p2unarchive.c
	$(CC) $(CC_FLAGS) p2unarchive.c -o p2unarchive

hw2: hw2.c
	$(CC) $(CC_FLAGS) hw2.c -o hw2

clean:
	rm -vf hw2 p2unarchive p2archive dirlist p2crypt
