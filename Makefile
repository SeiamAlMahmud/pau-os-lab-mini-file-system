CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude

TARGET = minifs

SOURCES = main.cpp \
          src/VirtualDisk.cpp \
          src/BlockManager.cpp \
          src/InodeManager.cpp \
          src/Directory.cpp \
          src/FileSystem.cpp \
          src/Shell.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

reset:
	rm -f disk/mini_fs.disk

.PHONY: all run clean reset
