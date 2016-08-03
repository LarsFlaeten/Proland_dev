# Proland_dev

My development clone of Inria's Proland:
[Proland home page](http://proland.imag.fr/)
   
##Introduction
Proland is a C++/OpenGL library for the real-time rendering of multi-resolution terrains (up to whole planets), the real-time management and edition of vector data (representing for instance roads or rivers), the rendering of atmosphere and clouds, the rendering and animation of the oceans, and the rendering of forests. All data are loaded or generated on the fly according to the viewpoint, and can be combined procedurally. For instance the shape and texture of a terrain can be modified using vector data representing roads and rivers.

Proland uses the Ork rendering library. You can find my version of ork here:
[My Ork on github](https://github.com/LarsFlaeten/ork)

##Background
This clone of the Proland librabry is mainly meant to be able to build and run Proland libraries and examples on my Linux computer. The code hierarchy is largely maintained, but the build system is changed to CMake.

##Overview of the library
Proland is made of a core library and of 7 plugins:
* Core library
* Terrain plugin
* Atmo plugin
* Ocean plugin
* Forrest plugin
* Graph plugin
* River plugin
* Edit plugin

##Screen Shots

![alt text](https://raw.githubusercontent.com/LarsFlaeten/Proland_dev/master/web/Procedural_landscape_test.png "Procedural landscape test")

![alt text](https://raw.githubusercontent.com/LarsFlaeten/Proland_dev/master/web/first_procedural_planet.png "First procedural planet")

![alt text](https://raw.githubusercontent.com/LarsFlaeten/Proland_dev/master/web/blue_marble_test.png "First earth test (with low res texture)")

## Current status
* The core librabry with examples is fully tested and included.
* The Terrain plugin with examples is fully tested and included.
* The Graph and Edit plugins are under testing/development.

