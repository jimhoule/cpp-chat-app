# Defines source files
SRCS = \
	$(wildcard core/src/*.cpp) \
	$(wildcard core/include/*.h) \
	$(wildcard client/src.*.cpp) \
	$(wildcard client/include/*.h) \
	$(wildcard server/src/*.cpp) \
	$(wildcard server/include/*.h)

build:
	@echo "Building project"
	cmake -S . -B build -D CMAKE_EXPORT_COMPILE_COMMANDS=1 && cmake --build build
	@echo "Building complete"

docker_up:
	@echo "Starting docker"
	docker-compose up -d
	@echo "Starting complete"

docker_down:
	@echo "Stopping docker"
	docker-compose down --remove-orphans
	@echo "Stopping complete"

# Formats files
format:
	@echo "Formatting project"
	@for file in $(SRCS); do \
		clang-format -i "$$file"; \
	done
	@echo "Formatting complete"

# Lints files
lint:
	@echo "Linting project"
	@for file in $(SRCS); do \
		clang-tidy "$$file" -p build; \
	done
	@echo "Linting complete"

setup:
	@echo "Setting up project"
	cp .githooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Setting up complete"
