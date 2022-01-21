CC = g++ -DDEBUG=1 
CFLAGS = -g -Wall -std=c++0x -std=c++11 -fPIC


SRCS =  masterDarkCreator.cpp

PROG = masterDarkCreator


LIBS =  -lopencv_core \
		-lopencv_imgcodecs \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_features2d \
		-lopencv_photo

INCLUDE = -I $(OPENCV_INC)

LIB_DIR = -L $(OPENCV_LIB)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROG) $(SRCS) $(LIB_DIR) $(LIBS) 
