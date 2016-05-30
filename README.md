# Swell-Animations Algorithm
This repo contains the backend which will generate the animation.

[![Build Status](http://vps73020.vps.ovh.ca:8080/buildStatus/icon?job=backend)](http://vps73020.vps.ovh.ca:8080/job/backend/)

## Usage:
##### Linux
* Building
```
make
```
* Unit testing 
```
make test
```
* Build protobuf files if missing/outdated
```
make protobuf-cpp
```

## Builds:
http://vps73020.vps.ovh.ca:8080/job/backend/ws/bin/

## Jenkins unit test:
http://vps73020.vps.ovh.ca:8080/job/backend-unit-test/

## Misc
* dependencies
```
libprotobuf-c-dev libprotobuf-dev libprotobuf9v5 libprotoc-dev libprotoc9v5
libgtest-dev
```
