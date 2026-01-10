flags=-O2 -std=c2x -I.
ldflags=-ldl -shared -fPIC
objects = api.o disk.o stdoslib.o fs.o
CFLAGS = 
.PHONY: all clean

all: clean api.so util

api.so: $(objects)
	gcc $(flags) $(CFLAGS) $(ldflags)  $^ -o $@

api.o: api.c api.h os.h
	gcc $(flags) $(CFLAGS) -fPIC -c $<

disk.o: disk.c disk.h
	gcc $(flags) $(CFLAGS) -fPIC -c $<

stdoslib.o: stdoslib.c stdoslib.h
	gcc $(flags) $(CFLAGS) -fPIC -c $<

fs.o: fs.c fs4disk.h fs.h
	gcc $(flags) $(CFLAGS) -fPIC -c $<

util.o : util.c
	gcc $(flags) $(CFLAGS) -fPIC -c $<

util: util.o api.o disk.o stdoslib.o fs.o
	gcc $(flags) $(CFLAGS) $^ -o $@
	mv -f util ../util

clean:
	rm -rf *.o *.so

