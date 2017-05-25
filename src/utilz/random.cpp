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
//  random.cpp
//
#include "random.hpp"


namespace utilz
{
namespace random
{

    std::mt19937 MersenneTwister::engine;


    int Int(const int THE_MIN, const int THE_MAX)
    {
        if (THE_MIN == THE_MAX)
        {
            return THE_MIN;
        }
        else
        {
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "utilz::random::Int(min=" << THE_MIN << ", max=" << THE_MAX << ")  The min was not less than the max.");

            //uniform_int_distribution is [x,y] (inclusive to the max value) so no increment is needed
            std::uniform_int_distribution<int> uni_int_dist(THE_MIN, THE_MAX);

            return uni_int_dist(MersenneTwister::engine);
        }
    }

}
}
