CXX 		:= g++
CXXFLAGS 	:= -Wall -std=c++17

# Release build flags
RELEASE_FLAGS := -s -Wall -DNDEBUG -mwindows -std=c++17

SRC_DIR     := app/src
TEST_DIR    := app/tests
OBJ_DIR     := out/build
BIN_DIR     := out/bin
INCLUDE_DIR	:= external/include
LIBS_DIR 	:= external/lib

APP_NAME	:= AsteroidGame
EXE 		:= $(BIN_DIR)/$(APP_NAME).exe

# Put all src sub directories in SRC_DIRS
SRC_MK_DIRS := engine scene
SRC_DIRS 	:= $(SRC_DIR) $(SRC_DIR)/engine $(SRC_DIR)/scene
SRC 		:= $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJ 		:= $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(subst $(SRC_DIR)/,,$(SRC)))


# All sub folder names under the "external/include" directory
# SDL3 SDL3_image Eigen imgui 								<- example on INCLUDE_DIRS
# $(foreach dir,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)/$(dir)) 	<- add this on INCLUDE
INCLUDE_DIRS := SDL3 SDL3_image imgui
INCLUDE      := -I$(INCLUDE_DIR) $(foreach dir,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)/$(dir))
LIB	         := -L$(LIBS_DIR) -lSDL3 -lSDL3_image

# Auto-create build subfolders as needed
# For Powershell

.PHONY: clean run

create_dirs:
	for dir in $(foreach source, $(SRC_MK_DIRS), $(OBJ_DIR)/$(source)); do \
		mkdir $$dir; \
	done

all: $(EXE)

$(EXE) : $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(INCLUDE) $(LIB)

# Release build
release: CXXFLAGS := $(RELEASE_FLAGS)
release: clean $(OBJ)
	$(CXX) $(RELEASE_FLAGS) $(OBJ) -o $(EXE) $(INCLUDE) $(LIB)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE) $(LIB)

run: all
	$(EXE)

# Install: make install DEST=path\\to\\folder
# Install: make install DEST=path/to/folder
install: release
	@if [ -z "$(DEST)" ]; then \
		echo "Please specify DEST, e.g. make install DEST=dist"; \
		exit 1; \
	fi
	mkdir -p $(DEST)
	cp -r $(BIN_DIR)/* $(DEST)/

# Deletes .exe and Object files
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o $(EXE)