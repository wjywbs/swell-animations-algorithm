SRCFILES := src/AnimationGeneration.cpp src/hermite.cpp src/swell-integration.cpp
SRCFILES += src/modeldata.pb.cc src/UnitTests.cpp
HEADERS  := include/modeldata.pb.h include/point.h

CXXFLAGS_LINUX := -g -fPIC `pkg-config --cflags --libs protobuf` -Wl,--no-as-needed -ldl
CXXFLAGS_APPLE := -lprotobuf
CXXFLAGS_TEST  := -g `pkg-config --cflags --libs protobuf` -lgtest

SRCDIR := src/
INCDIR := include/
BLDDIR := bin

BUILD_SO   := -shared -o $(BLDDIR)/swell-animations.so
BUILD_DL   := -dynamiclib -arch x86_64 -lprotobuf -o $(BLDDIR)/swell-animations.bundle
BUILD_TEST := -o $(BLDDIR)/UnitTests

EXECTEST := bin/UnitTests

build:  createBin $(SRCFILES) $(HEADERS)
	g++ $(BUILD_SO) src/swell-integration.cpp src/modeldata.pb.cc -I$(INCDIR) $(CXXFLAGS_LINUX)

test: createBin $(SRCFILES) $(HEADERS)
	g++ $(BUILD_TEST) -I$(INCDIR) src/UnitTests.cpp $(CXXFLAGS_TEST)
	$(EXECTEST)

createBin:
	mkdir -p bin

protobuf-cpp: .other/protobuf/modeldata.proto
	protoc -I ./ --cpp_out=. .other/protobuf/modeldata.proto
	mv .other/protobuf/modeldata.pb.cc src/
	mv .other/protobuf/modeldata.pb.h include/

maclocal: createBin $(SRCFILES) $(HEADERS)
	g++ $(BUILD_DL) src/swell-integration.cpp src/modeldata.pb.cc -I$(INCDIR) $(CXXFLAGS_APPLE)

clean:
	@rm bin/*
