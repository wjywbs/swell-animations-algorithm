animate:
	g++ -g -shared -o AnimationGeneration -fPIC AnimationDataSerializer/modeldata.pb.cc AnimationGeneration.cpp `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl

run:
	./AnimationGeneration

clean:
	rm AnimationGeneration
