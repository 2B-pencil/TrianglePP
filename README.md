[comment]: # " ![triangle-PP's logo](triangle-PP-sm.jpg) "
<img src="triangle-PP-sm.jpg" alt="triangle-PP's logo" width="160"/> 

# Triangle++

Triangle++ (or TrianglePP) is an updated version of an C++/OO-[wrapper by Piyush Kumar]((https://bitbucket.org/piyush/triangle/overview) for the original 2005 J.P. Shevchuk's *Triangle* package that was written in old plain C.

It can create Delaunay triangulations and constrained Delaunay triangulations. 

I have ported the original impl. to Visual C++ (VC9, i.e. VisualStudio 2008), extended it for constrainied triangulations and added some bugfixes. 

This code is released under LPGL licence.

## Update:

The code is now preliminary ported to x64 Windows. The project file was updated to Visual Studio 2015/2019, the x64 target was added, and the asserts & crashes when running the example program were fixed. It **wasn't thorougly tested though**, and produces some warnings at the moment. 

*CMake* support for both the example program and the GUI demonstrator were added too recently.

## Usage:

For usage patterns see the examples in the *trpp_example.cpp* source file. The interface of the *trpp*-wrapper is defined in the *tpp_inteface.hpp* header file.

Additionally, under *testappQt* you'll find a GUI programm to play with the triangulations:

![triangle-PP's GUI test program](triangle-pp-testApp.gif)

## Theory:

For for backgroud info on the implementation see "*Triangle: Engineering a 2D Quality Mesh Generator and Delaunay Triangulator*" by JP Shewchuk: http://www.cs.cmu.edu/~quake-papers/triangle.ps.

The original *Triangle* library documentation can be found at: http://www.cs.cmu.edu/~quake/triangle.html. The library was a **winner** of the 2003 James Hardy Wilkinson Prize in Numerical Software (sic!).

## TODOs:
 - Test on 64-bit Linux
 - Remove 64-bit warnings
 - Fix: 64-bit version **freezes** for 44 degrees min. angle constraint! Float value range problem?
 - Port the Qt demo app to Emscripten
 - Add generation and visualization (in red) of Voronoi diagrams.
