// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// resolution.cpp
//
#include "resolution.hpp"
#include <sstream>
#include <iomanip>

namespace sfml_util
{

    Resolution::Resolution(const unsigned int  WIDTH,
                           const unsigned int  HEIGHT,
                           const unsigned int  BITS_PER_PIXEL,
                           const std::string & NAME,
                           const std::string & ASPECT_RATIO)
    :
        name          (NAME),
        width         (WIDTH),
        height        (HEIGHT),
        aspect_ratio  (ASPECT_RATIO),
        bits_per_pixel(BITS_PER_PIXEL)
    {}


    Resolution::Resolution(const sf::VideoMode & VM)
    :
        name          ("unknown"),
        width         (VM.width),
        height        (VM.height),
        aspect_ratio  ("unknown"),
        bits_per_pixel(VM.bitsPerPixel)
    {}


    const std::string Resolution::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;
        ss  << ((WILL_WRAP) ? "[" : "")
            << std::setw(4) << width
            << "x"
            << std::setw(4) << height
            << " : "
            << std::setw(2) << bits_per_pixel
            << " "
            << std::setw(5) << aspect_ratio
            << " "
            << std::setw(7) << name
            << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }


    bool operator==(const Resolution & L, const Resolution & R)
    {
        return ((L.name == R.name)     &&
                (L.width == R.width)   &&
                (L.height == R.height) &&
                (L.bits_per_pixel == R.bits_per_pixel) &&
                (L.aspect_ratio == R.aspect_ratio));
    }


    bool operator!=(const Resolution & L, const Resolution & R)
    {
        return ! (L == R);
    }


    bool operator==(const Resolution & R, const sf::VideoMode & V)
    {
        return ((R.width == V.width) && (R.height == V.height) && (R.bits_per_pixel == V.bitsPerPixel));
    }


    bool operator!=(const Resolution & R, const sf::VideoMode & V)
    {
        return ! (R == V);
    }


    bool operator==(const sf::VideoMode & V, const Resolution & R)
    {
        return (R == V);
    }


    bool operator!=(const sf::VideoMode & V, const Resolution & R)
    {
        return (R != V);
    }

}
