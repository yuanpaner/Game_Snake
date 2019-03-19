SOURCES = snake.cpp
# LIBS = -framework OpenGL -framework GLUT -framework Foundation
# LIBS += -lm -lGLEW 
# LIBS += -lm -lGL -lglut -lGLEW 
# LIBS += -lm -lGL -L/usr/X11R6/lib -lGLU -lglut -lGLEW -lXi -lXmu
# LIBS +=-Wdeprecated-declarations
# LIBS=--framework OpenGL -framework GLUT

LIBS = -lncurses

all:
	g++ -std=c++11 ${SOURCES} ${LIBS} -o snakegame
	

# g++ -std=c++11 $(LIBS) ${SOURCES} -o snakegame

clean:
	rm -f snakegame