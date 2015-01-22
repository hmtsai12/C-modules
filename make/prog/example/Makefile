CC := $(CROSS_COMPILE)gcc
src = $(wildcard src/*.c)
src_obj := $(subst .c,.o,$(src))
TARGET := obj/example_prog
HEADPATH := -Iinc/ -I../../lib/example/inc
CFLAGS += -Wall $(HEADPATH)
LDFLAGS = -L../../lib/example/obj
LDLIBS = -lexample
$(TARGET) : $(subst .c,.o,$(src)) 
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^
	
%.d: %.c
	$(CC) $(HEADPATH)  -MM $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(src))
.PHONY: clean install uninstall

install :
	cp $(TARGET) $(ROOTFSPATH)/usr/bin
clean :
	rm -f $(TARGET) $(src_obj) $(subst .c,.d,$(src))
uninstall :
	rm $(ROOTFSPATH)/usr/bin/example_prog

