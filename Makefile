CC=gcc
PROG=box_filter

SOURCES=main.c utils.c

FLAGS=-std=c99
FLAGS+=-I"C:\OpenCV\build\include" -L"C:\OpenCV\build\x86\mingw\lib"
EXTRA_FLAGS= -Werror
LIBS=-lOpenCL -lopencv_core243 -lopencv_imgproc243 -lopencv_calib3d243 -lopencv_video243 -lopencv_features2d243 -lopencv_ml243 -lopencv_highgui243 -lopencv_objdetect243 -lopencv_contrib243 -lopencv_legacy243

all: $(PROG)

$(PROG): $(SOURCES)
	$(CC) -o $@ $^ $(FLAGS) $(EXTRA_FLAGS) $(LIBS)

clean:
	rm $(PROG).exe