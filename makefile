program: main.cpp videohandler.cpp
	g++ -o video main.cpp videohandler.cpp `pkg-config opencv4 --libs`

run: program
	./video

release: main.cpp videohandler.cpp
	g++ -O3 -o video main.cpp videohandler.cpp -lopencv_videoio -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lsfml-window -lsfml-audio -lsfml-graphics -lsfml-system

fast: release
	./video

noot:
	./video
