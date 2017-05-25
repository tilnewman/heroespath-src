# Heroes’ Path Source Code
This repository holds the source-code of the Heroes’ Path game.

Heroes' Path is an open-source, non-commercial, simple, 2D game in the classic RPG turn-based style.  It was created more as a learning exercise than as an attempt to finish a full game.  I’ll probably implement a few hours worth of playtime (as in resources such as maps, spells, creatures, etc.) and then leave it as a full-playable, all be it short, running game that others can finish if they wish.

The code is written in C++ (C++14) and I endeavored to keep it simple and straightforward. staying away from the dark corners of the type system and templates.  I relied heavily on SFML for 2D graphics and sound, and a couple of Boost libraries. (SFML = “Simple Fast Multimedia Library”, www.sfml-dev.org)  I used CMake, and maintained three build environments that compile cleanly at high warning levels:  Windows running Visual Studio 2017, MacOS 10.12 running Clang, and Linux (Ubuntu) running GCC.  Since I was working alone on this project, I relied on static and runtime analysis tools to be my code reviewers: PVS-Studio, CPPCheck, FLint++, and CPPLint, and runtime analysis with Valgrind.

Ziesche Til Newman (zTn)
2017-5-24
