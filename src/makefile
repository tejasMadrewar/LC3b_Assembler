CXX := clang++

OBJDIR := obj
DEPDIR := $(OBJDIR)\deps

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
CXXFLAGS += --std=c++14 -g
LDFLAGS +=

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

EXE := main.exe
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPFILES := $(SRCS:%.cpp=$(DEPDIR)/%.d)

all : $(EXE)

$(EXE): $(OBJS)
	@echo Linking $@
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o : %.cpp
$(OBJDIR)/%.o : %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	@echo Compiling $*.o
	@$(COMPILE.cpp) $(OUTPUT_OPTION) $<

db:
	python -m compiledb make clean all

clean:
	if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	if exist .cache rmdir /s /q .cache
	del *.pdb *.ilk examples\*.bin compile_commands.json $(EXE)
	del examples\*.hex examples\*.debug examples\*.symtab examples\*.obj

$(DEPDIR): ; mkdir $@

$(DEPFILES):
include $(wildcard $(DEPFILES))
