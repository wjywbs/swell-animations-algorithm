#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <fstream>
#include "swell-integration.h"
#include "AnimationGeneration.cpp"
#include "modeldata.pb.h"

using namespace swellanimations;

struct DataStuff {
	char a;
	int size;
	unsigned int responseSize;
};

Animation* generateTestData(ModelData* modelData) {
    Node model = modelData->model();
    Animation* animation = new Animation();
    for (int x = 0; x < modelData->controlpoints_size(); x++) {
        Node* node = animation->add_frames();
        node->CopyFrom(model);
        node->set_allocated_position(modelData->mutable_controlpoints(x));
    }
    return animation;
}

void save(char* a, int size, unsigned int& responseSize) {
	printf("WHATDEBUG %s %d %d\n", a, size, responseSize);

	ofstream o;
	o.open("sarah.a", ios::binary);
	o << a;
	o.close();

	o.open("sarah.a_size", ios::binary);
	o << string(a).length();
	o.close();

	o.open("sarah.size", ios::binary);
	o << size;
	o.close();

	o.open("sarah.responseSize", ios::binary);
	o << responseSize;
	o.close();
}

void load() {
	struct DataStuff d;
	ifstream i;

	int a_size;
	i.open("sarah.a_size", ios::binary);
	i >> a_size;
	i.close();

	char a[a_size];
	i.open("sarah.a", ios::binary);
	i.read(a, a_size);
	i.close();

	int size;
	i.open("sarah.size", ios::binary);
	i >> size;
	i.close();

	unsigned int responseSize;
	i.open("sarah.responseSize", ios::binary);
	i >> responseSize;
	i.close();

	printf("WHATDEBUG %s \n and: %d %d\n", a, size, responseSize);
	generateAnimation(a, size, responseSize);
}

void* generateAnimation(char* a, int size, unsigned int& responseSize) {
	save(a, size, responseSize);

    ModelData* modelData = new ModelData();
    modelData->ParseFromArray(a, size);
    //Animation* animation = generateTestData(modelData);
    Animation* animation = getFrames(modelData);
    responseSize = animation->ByteSize();
    void* response = malloc(responseSize);
    animation->SerializeToArray(response, responseSize);
    return response;
}

int main() {
	printf("yay\n");
	load();
	return 0;
}
