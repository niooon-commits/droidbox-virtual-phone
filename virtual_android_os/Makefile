# Makefile for DroidBox Virtual Android OS Engine (C++)

CXX ?= g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread -fPIC -Iinclude -O2
LDFLAGS = -pthread

SRCDIR = src
INCDIR = include
BUILDDIR = build
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))
TARGET = $(BINDIR)/vphone_daemon
LIB_TARGET = $(BINDIR)/libvphone_engine.so

.PHONY: all clean run dirs

all: dirs $(TARGET) $(LIB_TARGET)

dirs:
	@mkdir -p $(BUILDDIR) $(BINDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful: $@"

$(LIB_TARGET): $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))
	$(CXX) -shared $^ -o $@ $(LDFLAGS)
	@echo "Shared library built: $@"

run: $(TARGET)
	@./$(TARGET)

clean:
	rm -rf $(BUILDDIR) $(BINDIR)
