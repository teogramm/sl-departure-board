# Find which container program is available
ifneq (, $(shell which podman))
	CONTAINER_CMD := podman
	else
	ifneq (, $(shell which docker))
	CONTAINER_CMD := docker
	else
		$(error "Docker or podman not found in PATH")
	endif
endif

BRANCH_NAME := $(shell git rev-parse --abbrev-ref HEAD)
build-dev-container:
	$(CONTAINER_CMD) build -t ghcr.io/teogramm/sl-pi-cpp-dev:latest .
	$(CONTAINER_CMD) image tag ghcr.io/teogramm/sl-pi-cpp-dev:latest ghcr.io/teogramm/sl-pi-cpp-dev:$(BRANCH_NAME)

config:
	$(CROSS_COMPILE_CMD) cmake -DCMAKE_BUILD_TYPE=Release -S . -B cmake-cross

build: config
	$(CROSS_COMPILE_CMD) cmake --build cmake-cross --target sl_pi_cpp

TOP_WORKDIR := $(dir $(firstword $(MAKEFILE_LIST)))

cross-config cross-build: CROSS_COMPILE_CMD = $(CONTAINER_CMD) run --rm -it -v $(TOP_WORKDIR):/work ghcr.io/teogramm/sl-pi-cpp-dev:latest
cross-config: config
cross-build: build
