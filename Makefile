source_files := main.c cps.c third_party/tiny-bignum-c/bn.c

DEFS :=         -DBN_ARRAY_SIZE=64
DEFS := $(DEFS) -DWORD_SIZE=4
CFLAGS := -I third_party/tiny-bignum-c $(DEFS)

all: cps
debug: CFLAGS := $(CFLAGS) -D DBG -g3
debug: all

bn.o: third_party/tiny-bignum-c/bn.c
	$(CC) -c $(CFLAGS) -o $@ $^
facs.h: export_data
	./export_data
cps.o: cps.c facs.h
xps.o: xps.c cps.o facs.h
main.c: cps.o xps.o
cps: main.c cps.o xps.o bn.o
	$(CC) $(CFLAGS) -o $@ $^
export_data: export_data.c bn.o
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm cps export_data bn.o xps.o cps.o facs.h 
