# tinyply 2.3

[![Release is 2.3.2](http://img.shields.io/badge/release-2.3.2-blue.svg?style=flat)](https://raw.githubusercontent.com/ddiakopoulos/tinyply/master/source/tinyply.h)
[![License is Unlicense](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)

Platform | Build Status |
-------- | ------------ |
GCC + Clang | [![Build status](http://travis-ci.org/ddiakopoulos/tinyply.svg?branch=master)](https://travis-ci.org/ddiakopoulos/tinyply) |
VS2019 | [![Build status](https://ci.appveyor.com/api/projects/status/kgcy3oec0cnhyht4/branch/master?svg=true)](https://ci.appveyor.com/project/ddiakopoulos/tinyply/branch/master) |


A single-header, zero-dependency (except the C++ STL) __public domain__ implementation of the PLY mesh file format. An overview and definition of the file format is available [here](http://paulbourke.net/dataformats/ply/). This format is often used in the computer vision and graphics communities for its relative simplicity, ability to support arbitrary mesh attributes, and binary modes. Famously, PLY is used to distribute 3D models in the [Stanford 3D Scanning Repository](http://graphics.stanford.edu/data/3Dscanrep/), including the bunny. 

The library is written in C++11 and requires a recent compiler (GCC 4.8+ / VS2015+ / Clang 2.9+). Tinyply supports exporting and importing PLY files in both binary and ascii formats. Tinyply supports filesizes >= 4gb and can read big-endian binary files (but not write them). 

## Getting Started

The project comes with a simple example program demonstrating a circular write / read and all of the major API functionality. 

## In The Wild

Since 2015, tinyply has been used in hundreds of open-source projects including pointcloud tools, raytracers, synthetic data renderers, computational geometry libraries, and more. A few notable projects are highlighted below: 

* [Maplab](https://github.com/ethz-asl/maplab) from ETH Zürich, a research-oriented visual-inertial mapping framework. 
* [glChAoS.P](https://github.com/BrutPitt/glChAoS.P) from Michele Morrone, a rendering sandbox for 3D strange attractors.
* [Cilantro](https://github.com/kzampog/cilantro), a robust and featureful C++ library for working with pointcloud data. 
* [HabitatAI](https://aihabitat.org/), Facebook's 3D simulator for training AI agents in photorealistic environments.
* [ScanNet](http://www.scan-net.org/), an RGB+D dataset of 2.5 million views across 1500 scans.
* [PlaneRCNN](https://github.com/NVlabs/planercnn), 3D plane detection via single-shot images from NVIDIA Research
* [KNOSSOS](https://knossos.app/), a framework to visualize and annotate 3D image data (neural morphology and connectivity). 

tinyply not what you're looking for? tinyply trades some performance for simplicity and flexibility. For domain-specific uses (e.g. where your application does not need to handle arbitrary user-fed PLY files), there are other speedier alternatives. For more, please check out the following benchmarks:

* Vilya Harvey's [ply-parsing-perf](https://github.com/vilya/ply-parsing-perf)
* Maciej Halber's [ply_io_benchmark](https://github.com/mhalber/ply_io_benchmark)

## Past Versions

* `version 2.0` is an API re-write to support later improvements towards variable length lists. One notable change is that tinyply now produces and consumes structured byte arrays, with type information held as metadata.
* `version 2.1` contained minor bugfixes and speed improvements.
* `version 2.2` is a rewrite of the inner read/write loop. Compared to `version 2.0`, this version reads and writes binary about five times faster. When a list size hint is given for reading, the performance is approximately comparable to rply. 
* `version 2.3` contains minor bugfixes and performance improvements. A feature of the example application includes the ability to pre-load files into memory for improved runtime parsing performance on many files. 

## License

This software is in the public domain. Where that dedication is not recognized, you are granted a perpetual, irrevocable license to copy, distribute, and modify this file as you see fit. If these terms are not suitable to your organization, you may choose to license it under the terms of the 2-clause simplified BSD. 
