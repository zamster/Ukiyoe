UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	INCS = -I/usr/include/bullet
	LIBS = -lGL -lGLEW -lglfw -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -O2
endif
ifeq ($(UNAME_S),Darwin)
	LIBS = -lglew -lglfw3 -framework OpenGL -O2
endif

all:
	g++ -o ./bin/main ./src/*.cpp $(INCS) $(LIBS)

# ifeq ($(UNAME_S),Darwin)
# 	INCS = -I/usr/local/Cellar/bullet/2.82/include/bullet
# 	LIBS = -lglew -lglfw3 -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -framework OpenGL -O2
# endif