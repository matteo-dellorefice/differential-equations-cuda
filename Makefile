##########################################################

## Project file structure ##

# Source file directory:
SRC_DIR= src

# Object file directory:
BUILD_DIR= build

# Include header file diretory:
INC_DIR= include

##########################################################

## NVCC ##

# CUDA directory:
CUDA_ROOT_DIR=/usr/local/cuda
# CUDA library directory:
CUDA_LIB_DIR= -L$(CUDA_ROOT_DIR)/lib64
# CUDA include directory:
CUDA_INC_DIR= -I$(CUDA_ROOT_DIR)/include
# CUDA linking libraries:
CUDA_LINK_LIBS= -lcudart

###########################################################

## Include flags ##

INCL_FLAGS= $(CUDA_INC_DIR) -Iinclude -Iimgui -Iimgui/backends

# NVCC compiler options:
NVCC= nvcc
NVCC_FLAGS=
NVCC_LIBS=

###########################################################

## CC COMPILER OPTIONS ##

# CC compiler options:
CC= g++
CC_FLAGS= $(INCL_FLAGS)
CC_LIBS= -ldl -lglfw -lGL

##########################################################

TARGET = diffeq

CU_SOURCES := $(subst ./, , $(shell find . -name "*.cu"))
CU_OBJECTS := $(patsubst %.cu, $(BUILD_DIR)/%.co, $(CU_SOURCES))

CPP_SOURCES := $(subst ./, , $(shell find . -name "*.cpp"))
CPP_OBJECTS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))

TARGET_CU_OBJ := $(patsubst %.co, $(BUILD_DIR)/%.co, $(notdir $(CU_OBJECTS)))
TARGET_CPP_OBJ := $(patsubst %.o, $(BUILD_DIR)/%.o, $(notdir $(CPP_OBJECTS)))

# Link c++ and CUDA compiled object files to target executable:
$(TARGET) : $(CU_OBJECTS) $(CPP_OBJECTS)
	$(CC) $(CC_FLAGS) -o $@ $(TARGET_CU_OBJ) $(TARGET_CPP_OBJ) $(CUDA_INC_DIR) $(CUDA_LIB_DIR) $(CUDA_LINK_LIBS) $(CC_LIBS)

# Compile main .cpp file to object files:
$(BUILD_DIR)/%.o : %.cpp
	$(CC) $(CC_FLAGS) -c $< -o $(BUILD_DIR)/$(notdir $@)

# Compile CUDA source files to object files:
$(BUILD_DIR)/%.co : %.cu
	$(NVCC) $(NVCC_FLAGS) -c $< -o $(BUILD_DIR)/$(notdir $@) $(NVCC_LIBS) $(CUDA_INC_DIR)

# Clean objects in object directory.
clean:
	$(RM) build/* *.o $(TARGET)


show:
	@echo $(find ./build/ -name "*") 
	@echo CPP_OBJECTS $(CPP_OBJECTS)
	@echo CU_OBJECTS $(CU_OBJECTS)
	@echo OBJECTS $(OBJECTS)