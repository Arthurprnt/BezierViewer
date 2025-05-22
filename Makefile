CC ="g++"
PROJECT = BezierViewer
SRC = main.cpp

LIBS = `pkg-config opencv --cflags --libs`
CFLAGS =
$(PROJECT) : $(SRC)
	$(CC) $(SRC) -fsanitize=address -o $(CFLAGS) $(PROJECT) $(LIBS)