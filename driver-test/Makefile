CC := gcc

SOURCES1:=main.c
CFLAGS += -Wall -O -I../hello-driver

SOURCES1_OBJ := $(subst .c,.o,$(SOURCES1))

TARGET1 := dTest 

all : $(TARGET1)

$(TARGET1) : $(SOURCES1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) -I../hello-driver -M $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(SOURCES1))
.PHONY: clean
clean :
	rm -f $(TARGET1) $(SOURCES1_OBJ) $(subst .c,.d,$(SOURCES1))
