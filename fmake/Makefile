names := a b c d
src=$(wildcard *.c ./sub/*.c)
dir=$(notdir $(src))
obj=$(patsubst %.c, %.o, $(dir))
obj1=$(dir:%.c=%.o)
files := $(foreach n, $(names), $(n).o)
localfiles = $(shell ls -f *.c)
localfolder = $(shell ls -d */ | sed 's/\///')
all:
	@echo $(src)
	@echo $(dir)
	@echo $(obj)
	@echo $(obj1)
	@echo $(files)
	@echo $(localfiles)
	@echo $(localfolder)
	@echo "end"
