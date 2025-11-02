# Defines source files
SRCS = \
	$(wildcard core/src/*.cpp) \
	$(wildcard core/include/*.h) \
	$(wildcard client/src.*.cpp) \
	$(wildcard client/include/*.h) \
	$(wildcard server/src/*.cpp) \
	$(wildcard server/include/*.h) \
	$(wildcard test/src/*.cpp)

# Forces command to run every time (even if target already exits)
.PHONY: build test start_client

start_client:
	@echo "Starting client"
	cd build/client && ./Client
	@echo "Starting complete"

build:
	@echo "Building project"
	cmake -S . -B build -D CMAKE_EXPORT_COMPILE_COMMANDS=1 && cmake --build build
	@echo "Building complete"

test:
	@echo "Testing project"
	cd build/test && ctest
	@echo "Testing complete"

format:
	@echo "Formatting project"
	@for file in $(SRCS); do \
		clang-format -i "$$file"; \
	done
	@echo "Formatting complete"

lint:
	@echo "Linting project"
	@for file in $(SRCS); do \
		clang-tidy "$$file" -p build; \
	done
	@echo "Linting complete"

# Docker
docker_up:
	@echo "Starting docker"
	docker-compose up -d
	@echo "Starting complete"

docker_down:
	@echo "Stopping docker"
	docker-compose down --remove-orphans
	@echo "Stopping complete"

# Git
delete_git_submodule:
	@echo "Deleting submodule $(submodule_path)"
	git submodule deinit $(submodule_path)
	git rm --cached $(submodule_path)
	rm -rf .git/modules/$(submodule_path)
	rm -rf $(submodule_path)
	@echo "Deleting complete"

setup_git_hooks:
	@echo "Setting up project"
	cp .githooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Setting up complete"
