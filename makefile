all: vk

clean:
	rm -rf *.o

vk: main.o webcam.o edge.o haar.o keys.o colorEdge.o contrast.o ocr.o handdetect.o
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -L/usr/local/lib/ -lboost_signals -o vk main.o haar.o edge.o keys.o webcam.o colorEdge.o contrast.o ocr.o handdetect.o

main.o: main.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c main.cpp
	
edge.o: edge.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c edge.cpp
	
haar.o: haar.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c haar.cpp

keys.o: keys.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c keys.cpp

colorEdge.o: colorEdge.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c colorEdge.cpp

contrast.o: contrast.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c contrast.cpp

ocr.o: ocr.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c ocr.cpp

handdetect.o: handdetect.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c handdetect.cpp

webcam.o: webcam.cpp
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -I /usr/local/include/ -c webcam.cpp
	
