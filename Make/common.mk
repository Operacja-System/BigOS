include $(MAKE_INCLUDE_PATH)/toolchain.mk

$(if $(value BUILD_DIR),,$(error "BUILD_DIR is not set"))
TPATH := $(BUILD_DIR)/$(NAME)
BPATH := $(BUILD_DIR)/$(NAME)/obj

LIB_OBJS := $(foreach l, $(DEPS), $(BUILD_DIR)/lib/$(l)/$(l).o)
LDFLAGS := -o $(TPATH)/$(NAME).elf

SRC_C := $(shell find . -name '*.c')
SRC_S := $(shell find . -name '*.s')
OBJ_C := $(patsubst %.c, $(BPATH)/%.c.o, $(SRC_C))
OBJ_S := $(patsubst %.s, $(BPATH)/%.s.o, $(SRC_S))

all: $(NAME)

always:
	mkdir -p $(BPATH)
	mkdir -p $(TPATH)

clean:
	rm -rf $(BPATH)
	rm -rf $(TPATH)

$(NAME): $(OBJ_C) $(OBJ_S) $(LIB_OBJS)
	$(RV_LD) $(LDFLAGS) $(OBJ_S) $(OBJ_C) $(LIB_OBJS)

$(BPATH)/%.c.o : %.c always
	$(RV_CC) $(CFLAGS) -c $< -o $@

$(BPATH)/%.s.o : %.s always
	$(RV_AS) $(AFLAGS) -c $< -o $@

.PHONY: all clean always
