# Volumetric ray tracer framework in C++ for _Introduction to Computer Graphics and Visualization_

## Compiling the code

We provided a [CMakeLists.txt](CMakeLists.txt) file to be used with `CMake`.
This can be used in the following way (on the command line/shell):

```
# cd into the main directory of the framework
mkdir build   # create a new build directory
cd build      # cd into the directory
cmake ..      # creates a Makefile. Alternatively, use "cmake -DCMAKE_BUILD_TYPE=Release .." to do an optimized build.
# Compiling the code can then be done with:
make
# or
make -j4      # replacing 4 with the number of cores of your computer
```

A simple build script has also been provided in `build.sh`.

**Note:** After adding new `.cpp` files, `cmake ..` needs to be called again or you may get linker errors.

## Running the ray tracer

After compilation, the build directory should contain the `ray` executable.
It can be used as follows:

```
./ray <path to .json file> [output .png file]
# when in the build directory:
./ray ../scenes/other/scene01.json
```

Specifying an output is optional, and by default an image will be created in the same directory as the source scene file, with the `.json` extension replaced by `.png`.

## Description of the included files

### Scene files
* `scenes/*.json`: Scene files are structured in JSON. If you have never worked with JSON, please see [here](https://en.wikipedia.org/wiki/JSON#Syntax) or [here](https://www.json.org).
 Take a look at the provided example scenes for the general structure. You are encouraged to define your own scene files for testing your application and for participating in the competition.

### The ray tracer source files

* `main.cpp`: Contains main(), starting point. Responsible for parsing command-line arguments.
* `raytracer.cpp/.h`: Raytracer class. Responsible for reading the scene description, starting the ray tracer and writing the result to an image file.
* `scene.cpp/.h`: Scene class. Contains all objects, volumes and lights in the scene, as well as code for the actual ray tracing. Some variables are qualified as `[[maybe_unused]]` to suppress the compiler emitting warnings when these variables are (initially) unused. You may remove these qualifiers as you progress through the assignment.
* `image.cpp/.h`: Image class, includes code for reading from and writing to PNG files.
* `light.h`: Light class. Plain Old Data (POD) class. A colored light at a position in the scene.
* `ray.h`: Ray class. POD class. Ray from an origin point in a direction.
* `objects/object.h`: virtual `Object` class. Represents a geometric object (as opposed to a volume) in the scene. The Sphere shape derives from this class. Additional shapes can easily be added by making these derive from the Object class as well.
# `objects/material.h`: A POD class used by Objects to store its color and shading parameters. The Raytracer class uses it to read Material properties from the scene description.
* `objects/hit.h`: Hit class. A POD class storing the intersection between a `Ray` and an `Object`.
* `objects/shapes/sphere.cpp/.h`: Sphere class, which is a subclass of the `Object` class. Represents a sphere in the scene.
* `triple.cpp/.h`: Triple class. Represents a three-dimensional vector which is used for colors, points and vectors. Includes a number of useful functions and operators, see the comments in `triple.h`. Classes of `Color`, `Vector`, `Point` are all aliases of `Triple`.
* `volumes/volume.cpp/.h`: This class stores data related to rendering volumes. Volumes do not derive from Objects, but are a separate entity.
* `volumes/*`: The classes DensityField, TransferFunction and Segment relate to Volumes. The DensityField contains the actual data in the volume to be sampled, a Segment contains the entry and exit t-values for ray-volume intersection, and TransferFunction is used to transform the sampled data into colors.

### Supporting source files

* `datraw/*`: Library for reading raw volume data. [Source](https://github.com/UniStuttgart-VISUS/datraw).

* `lode/*`: Library for reading from and writing to PNG files, used by the `Image` class. Created by Lode Vandevenne and can be found on [GitHub](https://github.com/lvandeve/lodepng).

* `json/*`: Code for parsing JSON documents. Created by Niels Lohmann and available under the MIT license on [GitHub](https://github.com/nlohmann/json).
