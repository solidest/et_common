# target marcros
BIN_PATH := bin
TARGET_NAME := a.out
TARGET := $(BIN_PATH)/$(TARGET_NAME)
MAIN_SRC := main.cpp


# compile marcros
DIRS := RKLOG RKDB
OBJS :=

# intermedia compile marcros
# NOTE: ALL_OBJS are intentionally left blank, no need to fill it
ALL_OBJS := 
CLEAN_FILES := $(TARGET) $(OBJS)
DIST_CLEAN_FILES := $(OBJS)
LIB_INCLUDE := bin/librpc.a bin/librocksdb.a
LIB_REF := -L$(BIN_PATH) -luv

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2))$(filter $(subst *,%,$(2)),$(d)))

# default target
default: show-info all

# non-phony targets
$(TARGET): build-subdirs $(OBJS) find-all-objs
	@echo -e "\tCC" -pthread $(CCFLAG) -I$(INCLUDE_PATH) $(ALL_OBJS) $(MAIN_SRC) $(LIB_INCLUDE) -o $@ #$(LIB_REF)
	@$(CC)  -pthread $(CCFLAG) -I$(INCLUDE_PATH) $(ALL_OBJS) $(MAIN_SRC)  $(LIB_INCLUDE) -o $@ #$(LIB_REF)

# phony targets
.PHONY: all
all: $(TARGET)
	@echo Target $(TARGET) build finished.

.PHONY: clean
clean: clean-subdirs
	@echo CLEAN $(CLEAN_FILES)
	@rm -f $(CLEAN_FILES)

.PHONY: distclean
distclean: clean-subdirs
	@echo CLEAN $(DIST_CLEAN_FILES)
	@rm -f $(DIST_CLEAN_FILES)

# phony funcs
.PHONY: find-all-objs
find-all-objs:
	$(eval ALL_OBJS += $(call rwildcard,$(DIRS),*.o))

.PHONY: show-info
show-info:
	@echo Building Project

# need to be placed at the end of the file
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
export PROJECT_PATH := $(patsubst %/,%,$(dir $(mkfile_path)))
export MAKE_INCLUDE=$(PROJECT_PATH)/config/make.global
export SUB_MAKE_INCLUDE=$(PROJECT_PATH)/config/submake.global
include $(MAKE_INCLUDE)