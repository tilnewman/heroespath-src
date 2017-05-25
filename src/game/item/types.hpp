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
#ifndef GAME_ITEM_TYPEDEFS_INCLUDED
#define GAME_ITEM_TYPEDEFS_INCLUDED
//
// types.hpp
//  A collection of typedefs and constants related to items and inventory.
//
#include <cstddef> //for std::size_t


namespace game
{
namespace item
{

    using Coin_t   = int;
    using Gem_t    = int;
    using Weight_t = std::size_t;

    //TODO TEMP REMOVE -remove this when finished removing meteor shards from the game
    using Meteor_t = int;
}
}
#endif //GAME_ITEM_TYPEDEFS_INCLUDED