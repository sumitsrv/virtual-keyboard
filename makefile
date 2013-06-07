all: main

clean:
	rm -rf *.o

main: main.o webcam.o edge.o haar.o keys.o colorEdge.o contrast.o ocr.o
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -o main main.o haar.o edge.o keys.o webcam.o colorEdge.o contrast.o ocr.o

main.o: main.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c main.c
	
webcam.o: webcam.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c webcam.c
	
edge.o: edge.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c edge.c
	
haar.o: haar.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c haar.c

keys.o: keys.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c keys.c

colorEdge.o: colorEdge.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c colorEdge.c

contrast.o: contrast.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c contrast.c

ocr.o: ocr.c
	g++ -Wall -ansi `pkg-config opencv --cflags --libs` -c ocr.c

