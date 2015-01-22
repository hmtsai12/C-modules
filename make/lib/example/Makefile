CC := $(CROSS_COMPILE)gcc
src = $(wildcard src/*.c)
src_obj := $(subst .c,.o,$(src))
TARGET := obj/libexample.so
CFLAGS += -Wall -Iinc/ -fPIC

$(TARGET) : $(subst .c,.o,$(src)) 
	$(CC) -shared -o $@ $^
	
%.d: %.c
	$(CC) -Iinc/ -MM $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(src))
.PHONY: clean install uninstall

install :
	cp $(TARGET) $(ROOTFSPATH)/usr/lib
	cp inc/*.h $(ROOTFSPATH)/usr/include
clean :
	rm -f $(TARGET) $(src_obj) $(subst .c,.d,$(src))
uninstall :
	rm $(ROOTFSPATH)/usr/lib/libexample.so
