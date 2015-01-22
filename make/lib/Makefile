SRCDIR=$(shell ls -d */ | sed 's/\///')
.PHONY:all clean
all:
	for i in $(SRCDIR) ; do \
		make -C $$i; \
	done
clean:
	for i in $(SRCDIR) ; do \
		make clean -C $$i; \
	done
install:
	for i in $(SRCDIR) ; do \
		make install -C $$i; \
	done
uninstall:
	for i in $(SRCDIR) ; do \
		make uninstall -C $$i; \
	done
