all: prepare
	cmake --build build

prepare:
	cmake --preset=debug

clean:
	@rm -rf build

run-example_sbi: prepare
	cmake --build build --target run-example_sbi

.PHONY: prepare all clean run-example_sbi
