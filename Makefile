animate:
	g++ -g -shared -o AnimationGeneration -fPIC src/AnimationGeneration.cpp src/modeldata.pb.cc -Iinclude -Lsrc/* `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl

clean:
	rm AnimationGeneration
