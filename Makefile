CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude

TARGET = minifs

SOURCES = main.cpp \
          src/VirtualDisk.cpp \
          src/BlockManager.cpp \
          src/InodeManager.cpp \
          src/Directory.cpp \
          src/FileSystem.cpp \
          src/Shell.cpp \
          src/WebServer.cpp

ifeq ($(OS),Windows_NT)
	LIBS = -lws2_32
else
	LIBS = 
endif

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

reset:
	rm -f disk/mini_fs.disk

.PHONY: all run clean reset
