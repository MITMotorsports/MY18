all: prepare

prepare:
	@echo "Running all preparatory steps..."
	@(for x in scripts/prepare/*.sh; do ./$$x && echo "$$x DONE!\n" || echo "$$x FAILED!\n" ; done)

pull:
	scripts/pull.sh

toolchain:
	scripts/toolchain.sh
