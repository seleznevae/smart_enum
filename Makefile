
####### Compiler, tools and options

ERROR_CHECKING_POLICY = #-Werror

CC            = gcc
CXX           = g++6
DEFINES       =
CFLAGS        = -pipe  -Wall -W -D_REENTRANT -fPIC -MMD $(ERROR_CHECKING_POLICY) $(DEFINES) 
CXXFLAGS      = -pipe  -Wall -W -D_REENTRANT -fPIC -MMD $(ERROR_CHECKING_POLICY) -std=c++14 $(DEFINES)
INCPATH       = -I./src -I. -I./src/ext/ecolib/src
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
LFLAGS        =
LIBS          = -lpthread


####### Dirs and modules

SRC_DIR   	   = ./example
TARGET             = smart_enum

BUILD_DIR          = ./build
BIN_DIR            = ./bin
UTILITY_DIRS       = $(BUILD_DIR) $(BIN_DIR)

TEST_SRC_DIR       = ./test
TEST_TARGET        = test_smart_enum

####### Output


CPP_FILES         = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
C_FILES           = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
TEST_CPP_FILES    = $(wildcard $(TEST_SRC_DIR)/*.cpp) $(wildcard $(TEST_SRC_DIR)/*/*.cpp)

OBJCPP_FILES      = $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_FILES:.cpp=.cpp.o)))
TEST_OBJCPP_FILES = $(addprefix $(BUILD_DIR)/,$(notdir $(TEST_CPP_FILES:.cpp=.cpp.o)))
OBJC_FILES        = $(addprefix $(BUILD_DIR)/,$(notdir $(C_FILES:.c=.c.o)))

VPATH =  $(sort $(dir $(CPP_FILES) $(C_FILES) $(TEST_CPP_FILES))) # get dir-names of all src files (sort to remove duplicates)

####### Actions

.PHONY: clean checkdirs test

all: CXXFLAGS +=  -g #(temporary) for development purposes
all:  $(BIN_DIR)/$(TARGET) 

debug: CXXFLAGS +=  -g -DDEBUG
debug: $(TARGET)
test: $(BIN_DIR)/$(TEST_TARGET)
test: CXXFLAGS +=  -g -DDEBUG
#test: INCPATH += -I../../src
#test: INCPATH += -I./src

$(BIN_DIR)/$(TARGET): $(OBJCPP_FILES) $(OBJC_FILES) 
	$(CXX)   $(LFLAGS)  -o $@ $^ $(LIBS)


$(BIN_DIR)/$(TEST_TARGET): $(filter-out $(BUILD_DIR)/main.cpp.o,$(OBJCPP_FILES)) $(OBJC_FILES) $(TEST_OBJCPP_FILES) 
	$(CXX)   $(LFLAGS)  -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.cpp.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCPATH) -c -o $@ $<

$(BUILD_DIR)/%.c.o: %.c 
	$(CC) $(CXXFLAGS) $(INCPATH) -c -o $@ $<


clean:
	@rm -rf $(BUILD_DIR)/*.o
	@rm -rf $(BUILD_DIR)/*.d
	@rm -rf $(BIN_DIR)/*

-include $(OBJCPP_FILES:.o=.d)
-include $(OBJC_FILES:.o=.d)
-include $(TEST_OBJCPP_FILES:.o=.d)
