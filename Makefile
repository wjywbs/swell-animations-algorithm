build: src/AnimationGeneration.cpp src/AnimationGenerationWithPrinting.cpp src/diff_hermite.c src/hermite.cpp src/modeldata.pb.cc include/modeldata.pb.h include/point.h createBin
	g++ -g -shared -o bin/swell-animations.so -fPIC src/swell-integration.cpp src/modeldata.pb.cc -Iinclude -Lsrc/* `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl

createBin:
	mkdir -p bin

protobuf-cpp: .other/protobuf/modeldata.proto
	protoc -I ./ --cpp_out=. .other/protobuf/modeldata.proto
	mv .other/protobuf/modeldata.pb.cc src/
	mv .other/protobuf/modeldata.pb.h include/
clean:
	rm AnimationGeneration
