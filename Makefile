DESTDIR?=/usr/local/bin

all:
	cc -static -O2 redirecthttpd.c -o redirecthttpd
	strip -s redirecthttpd
clean:
	rm redirecthttpd
lint:
	lint redirecthttpd.c
install: all
	install -m 755 -o 0 -g 0 redirecthttpd $(DESTDIR)
