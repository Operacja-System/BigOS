LIBS = debug stdbigos

LIB = lib

all: always
	@for dir in $(LIBS); do \
		make -C $$dir DPATH=$(abspath $(BUILD_DIR)/$(LIB)); \
		done

always:
	mkdir -p $(BUILD_DIR)/$(LIB)

clean:
	rm -rf $(BUILD_DIR)/$(LIB)

.PHONY: all always clean
