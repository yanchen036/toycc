VPATH = include src
OBJ = main.o
INCLUDE = 'include'

toycc : $(OBJ)
	g++ $^ -o $@

main.o : main.cc
	g++ -c $< -o $@

.PHONY : clean
clean:
	rm $(OBJ) toycc
