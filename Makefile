build: src/AnimationGeneration.cpp src/AnimationGenerationWithPrinting.cpp src/diff_hermite.c src/hermite.cpp src/modeldata.pb.cc include/modeldata.pb.h include/point.h createBin
	g++ -g -shared -o bin/AnimationGeneration -fPIC src/AnimationGeneration.cpp src/modeldata.pb.cc -Iinclude -Lsrc/* `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl

createBin:
	mkdir -p bin
clean:
	rm AnimationGeneration
