CXX = clang++

CLANG_FORMAT = clang-format

DEBUG_ARGS = -g
DEP_ARGS = -MMD -MP
CXX_STD = -std=c++14

CCFLAGS = $(CXX_STD) $(DEBUG_ARGS) $(DEP_ARGS)
LINKERFLAGS =

TESTS = $(wildcard *.cpp)
OBJTESTS = $(TESTS:.cpp=.o)

SRCFILES = $(filter-out main.cpp, $(notdir $(wildcard ../src/*.cpp)))
OBJSRCFILES = $(SRCFILES:.cpp=.o)

OBJECTS = $(OBJTESTS) $(OBJSRCFILES)

.PHONY: clean all test format

all: format test

# include dep files
-include $(OBJECTS:.o=.d)

AllTests.exe: $(OBJECTS)
	@echo Linking $@
	@$(CXX) $(CCFLAGS) $^ -o $@

# pattern rule for compiling the tests file
%.o: %.cpp
	@echo Compiling $*.o
	@$(CXX) $(CCFLAGS) -c $< -o $@ 

# pattern rule for compiling the src file
%.o: ..\src\%.cpp ..\src\%.h
	@echo Compiling $*.o
	@$(CXX) $(CCFLAGS) -c $< -o $@ 

test: AllTests.exe
	AllTests.exe -r console 

clean:
	del *.o *.exe *.ilk *.pdb *.d
