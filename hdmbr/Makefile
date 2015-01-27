#CC := mipsel-linux-gcc
CC := gcc

SOURCES:=mbr.c main.c
CFLAGS += -Wall -O
#CFLAGS += -c
#CCFLAGS := $(CFLAGS) -O2
#SOURCES_OBJ:=test.o test1.o
SOURCES_OBJ := $(subst .c,.o,$(SOURCES))
TARGET := run

#$(warning target) $(TARGET) : $(SOURCES_OBJ) 
#$(TARGET) : $(SOURCES_OBJ) 
$(TARGET) : $(subst .c,.o,$(SOURCES)) 
	$(CC) -o $@ $^
#%.o: %.c
#	$(CC) -o $@ $(CCFLAGS) $<

#include $(subst .c,.d,$(SOURCES))

%.d: %.c
	$(CC) -M $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(SOURCES))
.PHONY: clean
clean :
	rm -f $(TARGET) $(SOURCES_OBJ) $(subst .c,.d,$(SOURCES))
