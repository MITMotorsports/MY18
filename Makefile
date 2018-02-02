all: prepare

prepare:
	@echo "Running all preparatory steps..."
	@(for x in scripts/prepare/*.sh; do ./$$x; done)

pull:
	scripts/pull.sh
