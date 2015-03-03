CC := gcc

SOURCES1:=udpser.c
SOURCES2:=udpcli.c
CFLAGS += -Wall -O

SOURCES1_OBJ := $(subst .c,.o,$(SOURCES1))
SOURCES2_OBJ := $(subst .c,.o,$(SOURCES2))

TARGET1 := udp_svr
TARGET2 := udp_cli

all : $(TARGET1) $(TARGET2)

$(TARGET1) : $(SOURCES1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET2) : $(SOURCES2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.d: %.c
	$(CC) -M $< > $@.$$$$;					\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

-include $(subst .c,.d,$(SOURCES1))
.PHONY: clean
clean :
	rm -f $(TARGET1) $(SOURCES1_OBJ) $(subst .c,.d,$(SOURCES1))
	rm -f $(TARGET2) $(SOURCES2_OBJ) $(subst .c,.d,$(SOURCES2))
