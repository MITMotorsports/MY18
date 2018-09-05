all: prepare

prepare:
	@echo "Running all preparatory steps..."
	@(for x in scripts/prepare/*.sh; do ./$$x && echo "$$x DONE!\n" || echo "$$x FAILED!\n" ; done)

canlib:
	scripts/prepare/canlib.sh

pull:
	scripts/pull.sh

upgrade:
	scripts/upgrade.sh

toolchain:
	scripts/toolchain/all.sh
