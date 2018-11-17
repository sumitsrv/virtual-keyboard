all: vk

clean:
	rm -rf *.o

vk: main.o webcam.o edge.o haar.o keys.o colorEdge.o contrast.o ocr.o
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -o vk main.o haar.o edge.o keys.o webcam.o colorEdge.o contrast.o ocr.o

main.o: main.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c main.cpp
	
webcam.o: webcam.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c webcam.cpp
	
edge.o: edge.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c edge.cpp
	
haar.o: haar.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c haar.cpp

keys.o: keys.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c keys.cpp

colorEdge.o: colorEdge.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c colorEdge.cpp

contrast.o: contrast.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c contrast.cpp

ocr.o: ocr.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c ocr.cpp

