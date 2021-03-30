NAME=edit

all: edit 

run: edit 
	./$(NAME).exe

edit:
	g++ -std=c++11 *.cpp -o $(NAME).exe

gdb:
	g++ -g -std=c++11 *.cpp -o $(NAME).exe

graphs: all	
	./$(NAME).exe graphs/*-10-*

clean:
	rm *.o *.exe
