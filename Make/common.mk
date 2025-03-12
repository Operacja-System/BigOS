$(if $(value BUILD_DIR),,$(error "BUILD_DIR is not set"))
TPATH := $(BUILD_DIR)/$(NAME)
BPATH := $(BUILD_DIR)/$(NAME)/obj

LIB_OBJS := $(foreach l, $(DEPS), $(BUILD_DIR)/lib/$(l)/$(l).o)
LDFLAGS := -o $(TPATH)/$(NAME).elf $(OBJ_C) $(OBJ_S) $(LIB_OBJS)

SRC_C := $(shell find . -name '*.c')
SRC_S := $(shell find . -name '*.s')
OBJ_C := $(patsubst %.c, $(BPATH)/%_C.o, $(SRC_C))
OBJ_S := $(patsubst %.s, $(BPATH)/%_S.o, $(SRC_S))

all: $(NAME)

always:
	mkdir -p $(BPATH)
	mkdir -p $(TPATH)

clean:
	rm -rf $(BPATH)
	rm -rf $(TPATH)

$(NAME): $(OBJ_C) $(OBJ_S)
	$(RV_LD) $(LDFLAGS) $(OBJ_S) $(OBJ_C)

$(BPATH)/%_C.o : %.c always
	$(RV_CC) $(CFLAGS) -c $< -o $@

$(BPATH)/%_S.o : %.s always
	$(RV_AS) $(AFLAGS) -c $< -o $@

.PHONY: all clean always
