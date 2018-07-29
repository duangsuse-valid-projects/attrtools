CC=clang

LIBE2_OUT=libe2.o
ATTRTOOLS_OUT=attrtools

attrtools: $(LIBE2_OUT) attrtools.c
	$(CC) $(CFLAGS) attrtools.c $(LIBE2_OUT) -o $(ATTRTOOLS_OUT)

libe2.o: libe2.c

.PHONY: rebuild debug clean

rebuild: clean attrtools

debug: $(LIBE2_OUT) attrtools.c
	$(CC) $(CFLAGS) -DDEBUG attrtools.c $(LIBE2_OUT) -o $(ATTRTOOLS_OUT)

clean:
	rm -f $(ATTRTOOLS_OUT) $(LIBE2_OUT)
