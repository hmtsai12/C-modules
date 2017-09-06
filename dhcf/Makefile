CC := gcc

SRCFILES=$(shell ls -f *.c)
CFLAGS += -Wall -O

OBJFILES := $(subst .c,.o,$(SRCFILES))

TARGET :=dhcf

all : $(TARGET)

$(TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) -M $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(SRCFILES))
.PHONY: clean
clean :
	rm -f $(TARGET) $(OBJFILES) $(subst .c,.d,$(SRCFILES))
