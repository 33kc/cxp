SOURCE = main.cpp functions.cpp
FINAL = cxp
make compile:
	g++ -o $(FINAL) $(SOURCE) -lncurses
make run:
	./$(FINAL)
