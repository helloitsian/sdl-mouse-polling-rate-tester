OBJS = main.c
OBJ_NAME = main.out

all : $(OBJS)
	gcc $(OBJS) -w -lSDL2 -o $(OBJ_NAME)