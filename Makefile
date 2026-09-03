CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: mini_fs

mini_fs: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o mini_fs

run: mini_fs
	./mini_fs

clean:
	rm -f mini_fs
