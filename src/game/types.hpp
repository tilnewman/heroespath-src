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
#ifndef GAME_TYPES_HPP_INCLUDED
#define GAME_TYPES_HPP_INCLUDED
//
// types.hpp
//
#include "misc/strong-type.hpp"

#include <utility>


namespace game
{

    //custom types //TODO change this into a StrongType named Trait_t
    using Trait_t = int;


    //phantom type tags
    struct NameTag {};
    struct CoinTag {};
    struct GemTag {};
    struct MeteorShardTag {};
    struct ExperienceTag {};
    struct HealthTag {};
    struct RankTag {};


    //strong types
    using Name_t =          misc::StrongType<std::string, NameTag>;
    using Coin_t =          misc::StrongNumericType<int, CoinTag>;
    using Gem_t =           misc::StrongNumericType<int, GemTag>;
    using MeteorShard_t =   misc::StrongNumericType<int, MeteorShardTag>;
    using Experience_t =    misc::StrongNumericType<int, ExperienceTag>;
    using Health_t =        misc::StrongNumericType<int, HealthTag>;
    using Rank_t =          misc::StrongNumericType<int, RankTag>;


    //user defined literals for strong types
    inline Coin_t operator"" _coin(unsigned long long coins)
    {
        return Coin_t(static_cast<int>(coins));
    }

    inline Gem_t operator"" _gem(unsigned long long gems)
    {
        return Gem_t(static_cast<int>(gems));
    }

    inline MeteorShard_t operator"" _mshard(unsigned long long mshards)
    {
        return MeteorShard_t(static_cast<int>(mshards));
    }
    
    inline Experience_t operator"" _exp(unsigned long long exp)
    {
        return Experience_t(static_cast<int>(exp));
    }
    
    inline Health_t operator"" _health(unsigned long long health)
    {
        return Health_t(static_cast<int>(health));
    }

    inline Rank_t operator"" _rank(unsigned long long rank)
    {
        return Rank_t(static_cast<int>(rank));
    }
    
}

#endif //GAME_TYPES_HPP_INCLUDED
