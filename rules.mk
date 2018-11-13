# make rules for ctomat project.

INCS := -I. -I$(TOP_DIR)/include

OPT_WARN := -Wall -Werror
OPT_OPTIM := -Og
OPT_DEBUG := -g
ifeq ($(ENABLE_SANITIZER),1)
  OPT_DEBUG += -fsanitize=address -fsanitize=leak -fomit-frame-pointer
endif
OPT_DEP := -MMD -MP

OPTS := $(OPT_WARN) $(OPT_OPTIM) $(OPT_DEBUG) $(OPT_DEP)

CPPFLAGS = -DNODEBUG=$(NODEBUG) $(EXTRA_CPPFLAGS)
CFLAGS = -std=c11 $(OPTS) $(INCS) $(EXTRA_CFLAGS)
CXXFLAGS = -std=c++11 $(OPTS) $(INCS) $(EXTRA_CXXFLAGS)
LDFLAGS = -L$(TOP_DIR)/src $(EXTRA_LDFLAGS)
LIBS = -l$(NAME)
ifeq ($(ENABLE_SANITIZER),1)
  LIBS += -lasan
endif
LIBS += $(EXTRA_LIBS)

Q1 = $(V:1=)
QCC = $(Q1:0=@echo "  CC  $@";)
QCXX = $(Q1:0=@echo "  CXX $@";)
QAR = $(Q1:0=@echo "  AR  $@";)

ifneq ($(DISABLE_CCACHE),1)
  CCACHE := $(shell which ccache)
endif
CC := $(CCACHE) $(CROSS_COMPILE)gcc
CXX := $(CCACHE) $(CROSS_COMPILE)g++
AR := $(CROSS_COMPILE)ar rcs

DEPS := $(OBJS:.o=.d)

%.o: %.c
	$(QCC)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(QCXX)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

.PHONY: all $(TARGET) $(EXAMPLE) $(TEST) clean

all: $(LIBRARY) $(EXAMPLE) $(TEST)

$(LIBRARY): $(OBJS)
	$(QAR)$(AR) $@ $^

$(EXAMPLE): $(OBJS)
	$(QCC)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TEST): $(OBJS)
	$(QCXX)$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf $(LIBRARY) $(EXAMPLE) $(TEST) $(OBJS) $(DEPS)

-include $(DEPS)
