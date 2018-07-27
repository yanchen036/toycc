VPATH = include src
OBJ = main.o
INCLUDE = 'include'
CXXFLAGS=

toycc : $(OBJ)
	g++ $(CXXFLAGS) $^ -o $@

main.o : main.cc
	g++ $(CXXFLAGS) -c $< -o $@

.PHONY : clean
clean:
	rm $(OBJ) toycc
