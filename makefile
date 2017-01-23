all: a4

TARGET = -mmacosx-version-min=10.8
#LIBS = -F/System/Library/Frameworks -framework OpenGL -framework GLUT -lm
LIBS = -lglut -lGLU -lGL -g -lm

a4: a4.c
	gcc a4.c -o a4 $(LIBS)
