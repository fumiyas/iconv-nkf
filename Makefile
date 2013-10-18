BUILD_TARGETS = iconv-nkf iconv-nkf.so libiconv-nkf.so

CC = gcc
CFLAGS = -O2 -g -Wall -fPIC
CFLAGS_PRELOADLIB = -DICONV_NKF_PRELOADLIB
LDSHARED = $(CC) -shared
LDFLAGS =

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib

default: build

clean:
	rm -f $(BUILD_TARGETS)
	rm -f *.o *.so *.tmp

install:
	mkdir -m 0755 -p $(DESTDIR)$(bindir)
	rm -f $(DESTDIR)$(bindir)/iconv-nkf
	cp iconv-nkf $(DESTDIR)$(bindir)/
	mkdir -m 0755 -p $(DESTDIR)$(libdir)
	rm -f $(DESTDIR)$(libdir)/iconv-nkf.so
	cp iconv-nkf.so $(DESTDIR)$(libdir)/

test: iconv-nkf.so
	./test-iconv-nkf.sh

build: $(BUILD_TARGETS)

iconv-nkf: iconv-nkf.sh
	sed 's#@libdir@#$(libdir)#g' iconv-nkf.sh >$@.tmp
	chmod +x $@.tmp
	mv $@.tmp $@

iconv-nkf-preloadlib.o: iconv-nkf.c
	$(CC) $(CFLAGS) $(CFLAGS_PRELOADLIB) -o $@ -c iconv-nkf.c

iconv-nkf.so: iconv-nkf-preloadlib.o iconv-real.o
	$(LDSHARED) $(LDFLAGS) -o $@ iconv-nkf-preloadlib.o iconv-real.o

libiconv-nkf.so: iconv-nkf.o
	$(LDSHARED) $(LDFLAGS) -o $@ iconv-nkf.o

