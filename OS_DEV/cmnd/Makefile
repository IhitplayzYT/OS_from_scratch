flags=-O2 -std=c23 -I../api
ldflags=

.PHONY: all clean clear

all:clean clear cmnd 

../api/api.so: ../api/api.c ../api/api.h ../api/disk.c ../api/disk.h
	$(MAKE) -C ../api -sk "CFLAGS=$(flags)"

cmnd: cmnd.o ../api/api.so
	gcc $(flags) $^ -o $@ $(ldflags)

cmnd.o: cmnd.c cmnd.h
	gcc $(flags) -c $<
clear:
	cd ../api
	$(MAKE) clean
clean:
	rm -rf *.o *.so cmnd

