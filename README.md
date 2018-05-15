# Heroes’ Path Source Code
This repository holds the source-code of the Heroes’ Path game.


## About the Game
Heroes' Path is a free (non-commercial), open-source (beerware), simple, 2D game in the classic RPG turn-based style.  I intend to implement a few hours worth of playtime, and then leave it as a fully-playable, all be it short, running game.  The license is intentionally (sarcastically) permissive, allowing anyone to finish the game or to make their own spin-off versions.


## About the Code
The code is written in C++ (C++14 but moving to C++17).  I have endeavored to keep the code simple and straightforward, staying away from the dark corners of the type system and templates.  I relied heavily on SFML (“Simple Fast Multimedia Library”, www.sfml-dev.org) for 2D graphics and sound, while using the Boost (boost.org) libraries sparingly, mostly for serialization and xml parsing.

This is a CMake project, and I maintained three build environments that compile cleanly at high warning levels:
 * Windows running Visual Studio 2017
 * MacOS 10.12 running Clang
 * and Linux running GCC

Since I was working alone on this project, I relied on static and runtime analysis tools to be my code reviewers:
 * [Valgrind](http://valgrind.org)
 * [PVS-Studio](http://viva64.com)
 * [CPPCheck](http://cppcheck.sourceforge.net)
 * [FLint++](http://github.com/L2Program/FlintPlusPlus)
 * [CPPLint](http://github.com/google/styleguide/tree/gh-pages/cpplint)


## How To
0. Install [CMake](http://cmake.org)
1. Install Visual Studio, GCC, or the Clang compiler
2. Install [Boost](http://boost.org)
3. Install [SFML](http://sfml-dev.org)
4. `mkdir heroespath`
5. `cd heroespath`  
6. `git clone https://github.com/tilnewman/heroespath-media.git` *creates heroespath-media dir*
7. `git clone https://github.com/tilnewman/heroespath-src.git` *creates heroespath-src dir*
8. `mkdir build`
9. `cd build`
10. `cmake -G <generator name here> ../heroespath-src/src`
11. Compile (i.e. `make`)
12. `cd HeroesPath`
13. Run (i.e. `./HeroesPath`)
