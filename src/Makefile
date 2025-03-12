INCLUDES := lib
DEFINES := DEBUG

export CFLAGS := -Wall -Wextra -Werror -mcmodel=medany -g -O0 -Wno-ignored-qualifiers -ffreestanding -march=rv64g -std=c23
CFLAGS += $(addprefix -D__, $(addsuffix __,$(DEFINES)))
CFLAGS += $(patsubst %, -I$(SRC_DIR)/%, $(INCLUDES))
export AFLAGS := -march=rv64g
export LDFLAGS := -nostdlib

SRC_DIRS := $(wildcard $(SRC_DIR)/*/)
SRC_DIRS := $(filter-out $(SRC_DIR)/lib/, $(SRC_DIRS))

all:
	make -C $(SRC_DIR)/lib/ SRC_DIR=$(SRC_DIR) BUILD_DIR=$(BUILD_DIR)
	@for dir in $(SRC_DIRS); do make -C $$dir SRC_DIR=$(SRC_DIR) BUILD_DIR=$(BUILD_DIR); done

.PHONY: all

