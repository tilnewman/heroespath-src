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
#include "game/stats/types.hpp"

#include "misc/strong-type.hpp"
#include "misc/strong-numeric-type.hpp"

#include <utility>


namespace game
{

    //phantom type tags
    struct NameTag {};
    struct CoinTag {};
    struct GemTag {};
    struct MeteorShardTag {};
    struct ExperienceTag {};
    struct HealthTag {};
    struct RankTag {};
    struct ManaTag {};
    struct WeightTag {};
    struct ArmorTag {};
    struct ScoreTag {};

    /*
    //Responsible for wrapping a health value and providing health functions.
    struct StrongNumericTypeHealth : misc::StrongNumericType<stats::Trait_t, HealthTag>
    {
        template<typename T, typename Parameter>
        explicit StrongNumericTypeHealth(const misc::StrongNumericType<T, Parameter> STRONG_TYPE_VALUE)
        :
            StrongNumericType(STRONG_TYPE_VALUE.Get())
        {}

        explicit StrongNumericTypeHealth(const stats::Trait_t VALUE)
        :
            StrongNumericType(VALUE)
        {}

        inline bool IsDamage() const { return m_value < 0; }
        inline bool IsHealing() const { return m_value > 0; }
    };
    */


    //strong types
    using Name_t =          misc::StrongType<std::string, NameTag>;
    using Coin_t =          misc::StrongNumericType<int, CoinTag>;
    using Gem_t =           misc::StrongNumericType<int, GemTag>;
    using MeteorShard_t =   misc::StrongNumericType<int, MeteorShardTag>;
    using Experience_t =    misc::StrongNumericType<int, ExperienceTag>;
    using Health_t =        misc::StrongNumericType<int, HealthTag>;
    using Rank_t =          misc::StrongNumericType<int, RankTag>;
    using Mana_t =          misc::StrongNumericType<int, ManaTag>;
    using Weight_t =        misc::StrongNumericType<int, WeightTag>;
    using Armor_t =         misc::StrongNumericType<int, ArmorTag>;
    using Score_t =         misc::StrongNumericType<int, ScoreTag>;


    //user defined literals for strong types
    inline Coin_t operator"" _coin(unsigned long long coins)
    {
        return Coin_t(static_cast<Coin_t::type>(coins));
    }

    inline Gem_t operator"" _gem(unsigned long long gems)
    {
        return Gem_t(static_cast<Gem_t::type>(gems));
    }

    inline MeteorShard_t operator"" _mshard(unsigned long long mshards)
    {
        return MeteorShard_t(static_cast<MeteorShard_t::type>(mshards));
    }
    
    inline Experience_t operator"" _exp(unsigned long long exp)
    {
        return Experience_t(static_cast<Experience_t::type>(exp));
    }
    
    inline Health_t operator"" _health(unsigned long long health)
    {
        return Health_t(static_cast<Health_t::type>(health));
    }

    inline Rank_t operator"" _rank(unsigned long long rank)
    {
        return Rank_t(static_cast<Rank_t::type>(rank));
    }

    inline Mana_t operator"" _mana(unsigned long long mana)
    {
        return Mana_t(static_cast<Mana_t::type>(mana));
    }

    inline Weight_t operator"" _weight(unsigned long long weight)
    {
        return Weight_t(static_cast<Weight_t::type>(weight));
    }

    inline Armor_t operator"" _armor(unsigned long long armor)
    {
        return Armor_t(static_cast<Armor_t::type>(armor));
    }

    inline Score_t operator"" _score(unsigned long long score)
    {
        return Score_t(static_cast<Score_t::type>(score));
    }
    
}   

#endif //GAME_TYPES_HPP_INCLUDED
