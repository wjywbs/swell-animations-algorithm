#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include "swell-integration.h"
#include "AnimationGeneration.cpp"
#include "AnimationLayering.cpp"
#include "modeldata.pb.h"

using namespace swellanimations;

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

void* generateAnimation(char* a, int size, unsigned int& responseSize) {
    ModelData* modelData = new ModelData();
    modelData->ParseFromArray(a, size);
    //Animation* animation = generateTestData(modelData);
    Animation* animation = getFrames(modelData);

    //Call layering
    AddLayering(modelData, animation);

    responseSize = animation->ByteSize();
    void* response = malloc(responseSize);
    animation->SerializeToArray(response, responseSize);
    return response;
}

