
EXECUTABLE	:= main
SRCEXTS		:=.c .cpp
LIBDIR		:=/usr/lib64/mysql
LIBS		:=z pthread
SUBDIR		:= $(shell find ../public -maxdepth 3 -type d)
INCLUDES	:=.  $(SUBDIR)
SRCDIR		:=.  $(SUBDIR)


CC			:=g++ -Wall 
CFLAGS		:=-g -DNOPROTOCOLOG
CFLAGS		+=-DPEIPAI_TEST
CPPFLAGS	:= $(CFLAGS)
CPPFLAGS	+= $(addprefix -I,$(INCLUDES))
CPPFLAGS	+= #-MMD
RM-F		:= rm -f


###########################################################
# # You shouldn't need to change anything below this point.
###########################################################

#这里循环遍历目录的cpp文件
SRCS	= $(foreach d,$(SRCDIR),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS	= $(foreach x,$(SRCEXTS), \
			$(patsubst %$(x),%.o,$(filter %$(x),$(SRCS))))
DEPS	= $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))

.PHONY : all deps objs clean veryclean rebuild info

all: $(EXECUTABLE)

deps : $(DEPS)

objs : $(OBJS)

clean :
	@$(RM-F) $(OBJS)
	@$(RM-F) $(DEPS)

veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean all
ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)
$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))

info:
#	@echo $(SRCS)
	@echo $(OBJS)
#	@echo $(SRCDIR)
#	@echo $(DEPS)
#	@echo $(MISSING_DEPS)
#	@echo $(MISSING_DEPS_SOURCES)
