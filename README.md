# README

## About

ADiffView is a file compare tool for AmigaOS3.0+. It compares plain
ASCII text files and displays the two compared files next to each other.

It is using the linear spaced optimized Myers' Diff Algorithm. The diff
engine also uses some code from Matthias Hertel. See the file
LICENSE-3RD-PARTY for more information.

## Development environment

ADiffView is developed using these tools:

* on Amiga using NDK3.1 and StormC4 (gcc / C++ mode)
* on Linux using using cmake, bebbos gcc 6.5 toolchain and Visual 
Studio Code 


## Howto build

### Build on Linux

The Makefile to build this project must be created with cmake:

- create a directory *build* next to the *src* directory
- enter this directory

To prepare cmake release build type from within this directory

    cmake -DCMAKE_BUILD_TYPE=Release ..

Or to prepare a debug build

    cmake -DCMAKE_BUILD_TYPE=Debug ..

After cmake is finsihed the project can be build by simply typing 
    
    make 

from inside the build directory.

#### VSCode integration

### Build on Amiga
The project file src/ADiffView.¶ can be loaded with StormC4. 

Then it should compile without errors and just a few warnings..
