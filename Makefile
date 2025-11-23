CXX = g++-14
CXXFLAGS = -std=c++20 -fmodules-ts -Wall -g
PRECOMPILE_FLAGS = -std=c++20 -fmodules-ts -c -x c++-system-header

# Directories
TEST_DIR = tests
OBJ_DIR = objects

##################################################################
# GOON0: ONLY EDIT THIS SECTION TO ADD NEW STANDARD LIBRARY
##################################################################
SYSTEM_HEADERS = iostream utility algorithm  vector functional cstdlib \
				 fstream filesystem map unordered_map stdexcept string

##################################################################

# Main program
TARGET = biq
MAIN_SOURCE = main.cc

##################################################################
# GOON1: ONLY EDIT THIS SECTION TO ADD NEW MODULES
##################################################################
# List your module interface files here (in dependency order)
MODULE_INTERFACES = blocks.cc levels.cc renderers.cc

# List your module implementation files here (same order as interfaces)
MODULE_IMPLS =  blocks-impl.cc levels-impl.cc renderers-impl.cc
#################################################################

# Automatically generate object file names from source files (in objects/ dir)
MODULE_INTERFACE_OBJECTS = $(addprefix $(OBJ_DIR)/, $(MODULE_INTERFACES:.cc=.o))
MODULE_IMPL_OBJECTS = $(addprefix $(OBJ_DIR)/, $(MODULE_IMPLS:.cc=.o))
MODULE_OBJECTS = $(MODULE_INTERFACE_OBJECTS) $(MODULE_IMPL_OBJECTS)
MAIN_OBJECT = $(OBJ_DIR)/$(MAIN_SOURCE:.cc=.o)
MAIN_OBJECTS = $(MODULE_OBJECTS) $(MAIN_OBJECT)

# Test files (automatically find all test_*.cc files)
TEST_SOURCES = $(filter-out $(TEST_DIR)/test_runner.cc $(TEST_DIR)/test_main.cc, $(wildcard $(TEST_DIR)/test_*.cc))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cc, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Test framework files
TEST_RUNNER_SOURCE = $(TEST_DIR)/test_runner.cc
TEST_RUNNER_OBJECT = $(OBJ_DIR)/test_runner.o
TEST_MAIN_SOURCE = $(TEST_DIR)/test_main.cc
TEST_MAIN_OBJECT = $(OBJ_DIR)/test_main.o

# Test executable
TEST_EXEC = test_runner

# Header compilation marker
HEADERS_COMPILED = .headers_compiled

.PHONY: all test clean help

# Default: build main program
all: $(TARGET)

# Create objects directory
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Precompile system headers
$(HEADERS_COMPILED): | gcm.cache
	$(CXX) $(PRECOMPILE_FLAGS) $(SYSTEM_HEADERS)
	@touch $(HEADERS_COMPILED)

gcm.cache:
	@mkdir -p gcm.cache

# Build main program: -lX11 for linking X lib
$(TARGET): $(MAIN_OBJECTS)
	@echo "Building main program..."
	$(CXX) $(CXXFLAGS) $(MAIN_OBJECTS) -lX11 -o $(TARGET)

##################################################################
# GOON2: MODULE DEPENDENCIES: Add rules for your modules here
##################################################################
# Pattern: each module compiles to objects/ directory
$(OBJ_DIR)/%.o: %.cc $(HEADERS_COMPILED) | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Specific dependencies for implementation files (depend on their interface)
$(OBJ_DIR)/blocks-impl.o: blocks-impl.cc $(OBJ_DIR)/blocks.o $(HEADERS_COMPILED) | $(OBJ_DIR)
$(OBJ_DIR)/levels.o: levels.cc $(OBJ_DIR)/blocks.o $(HEADERS_COMPILED) | $(OBJ_DIR)
$(OBJ_DIR)/levels-impl.o: levels-impl.cc $(OBJ_DIR)/levels.o $(HEADERS_COMPILED) | $(OBJ_DIR)
$(OBJ_DIR)/renderers.o: renderers.cc $(OBJ_DIR)/blocks.o $(HEADERS_COMPILED) | $(OBJ_DIR)
$(OBJ_DIR)/renderers-impl.o: renderers-impl.cc $(OBJ_DIR)/renderers.o $(HEADERS_COMPILED) | $(OBJ_DIR)


# Main depends on all module interfaces
$(MAIN_OBJECT): $(MAIN_SOURCE) $(MODULE_INTERFACE_OBJECTS) $(HEADERS_COMPILED) | $(OBJ_DIR)

##################################################################

# Build and run tests
test: $(TEST_EXEC)
	@echo "Running tests..."
	@./$(TEST_EXEC)

# Build test executable (uses same module objects as main program)
$(TEST_EXEC): $(MODULE_OBJECTS) $(TEST_RUNNER_OBJECT) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT) $(HEADERS_COMPILED)
	@echo "Linking test executable..."
	$(CXX) $(CXXFLAGS) $(MODULE_OBJECTS) $(TEST_RUNNER_OBJECT) $(TEST_OBJECTS) $(TEST_MAIN_OBJECT) -lX11 -o $(TEST_EXEC)

# Compile test runner implementation
$(TEST_RUNNER_OBJECT): $(TEST_RUNNER_SOURCE) $(HEADERS_COMPILED) | $(OBJ_DIR)
	@echo "Compiling test runner..."
	$(CXX) $(CXXFLAGS) -c $(TEST_RUNNER_SOURCE) -o $(TEST_RUNNER_OBJECT)

# Compile test main
$(TEST_MAIN_OBJECT): $(TEST_MAIN_SOURCE) $(HEADERS_COMPILED) | $(OBJ_DIR)
	@echo "Compiling test main..."
	$(CXX) $(CXXFLAGS) -c $(TEST_MAIN_SOURCE) -o $(TEST_MAIN_OBJECT)

# Compile individual test files (depend on all module objects)
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.cc $(MODULE_OBJECTS) $(HEADERS_COMPILED) | $(OBJ_DIR)
	@echo "Compiling test: $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJ_DIR) $(TARGET) $(HEADERS_COMPILED) $(TEST_EXEC)
	@rm -rf gcm.cache

# Help message
help:
	@echo "Available targets:"
	@echo "  make        - Build main program ($(TARGET))"
	@echo "  make test   - Build and run tests"
	@echo "  make clean  - Remove all build artifacts"
	@echo "  make help   - Show this message"
	@echo ""
	@echo "To add a new module:"
	@echo "  0. If needed, add new standard library module (e.g. iostream) <- search GOON0"
	@echo "  1. Add interface file to MODULE_INTERFACES (e.g., list.cc) <- search GOON1"
	@echo "  2. Add implementation file to MODULE_IMPLS (e.g., list-impl.cc) <- search GOON1"
	@echo "  3. Add dependency rule: \$$(OBJ_DIR)/list-impl.o: list-impl.cc \$$(OBJ_DIR)/list.o <- search GOON2"
	@echo ""
	@echo "To add tests: create tests/test_<module>.cc files"
	@echo "They will be automatically discovered and compiled"
