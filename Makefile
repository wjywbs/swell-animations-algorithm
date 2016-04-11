build: src/AnimationGeneration.cpp src/AnimationGenerationWithPrinting.cpp src/diff_hermite.c src/hermite.cpp src/modeldata.pb.cc include/modeldata.pb.h include/point.h createBin
	g++ -g -shared -o bin/swell-animations.so -fPIC src/swell-integration.cpp src/modeldata.pb.cc -Iinclude -Lsrc/* `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl

createBin:
	mkdir -p bin

protobuf-cpp: .other/protobuf/modeldata.proto
	protoc -I ./ --cpp_out=. .other/protobuf/modeldata.proto
	mv .other/protobuf/modeldata.pb.cc src/
	mv .other/protobuf/modeldata.pb.h include/

maclocal:
	g++ -dynamiclib  -arch x86_64 src/swell-integration.cpp src/modeldata.pb.cc -Iinclude -Lsrc -lprotobuf -current_version 1.0 -compatibility_version 1.0 -fvisibility=hidden -o swell-animations.bundle

clean:
	@rm -f *.exe *.o *.dll *.bundle *.so* *.dylib loader

clean:
	rm AnimationGeneration
