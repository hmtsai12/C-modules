CC := $(CROSS_COMPILE)gcc
src = eeprom.c
src_obj := $(subst .c,.o,$(src))
TARGET := eeprom
#HEADPATH := -Iinc/ -I$(ROOTFSPATH)/usr/include
CFLAGS += -Wall $(HEADPATH)
#LDFLAGS = -L$(ROOTFSPATH)/usr/lib/arm-linux-gnueabihf
LDLIBS = -lusb -lftdi
$(TARGET) : $(subst .c,.o,$(src)) 
	$(CC)  -o $@ $^ $(LDFLAGS) $(LDLIBS)
	
%.d: %.c
	$(CC) $(HEADPATH)  -MM $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(src))

.PHONY: clean

clean:
	rm -f $(TARGET) $(src_obj) $(subst .c,.d,$(src))
