VPATH = include src
OBJ = main.o
INCLUDE = 'include'

main : $(OBJ)
	g++ -o main $^

main.o : main.cc
	g++ -c $< -o $@

.PHONY : clean
clean:
	rm $(OBJ) main
