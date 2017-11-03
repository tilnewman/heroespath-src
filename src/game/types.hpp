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
//  Strong types used by the game.
//
#include "game/stats/trait.hpp"

#include "misc/strong-type.hpp"
#include "misc/strong-numeric-type.hpp"

#include <vector>
#include <utility>


//global strong types
struct IDTag {};
struct IndexTag {};
struct CountTag {};
struct RowTag {};
struct ColumnTag {};

using ID_t = misc::StrongNumericType<std::size_t, IDTag>;
using Index_t = misc::StrongNumericType<std::size_t, IndexTag>;
using Count_t = misc::StrongNumericType<std::size_t, CountTag>;
using Row_t = misc::StrongNumericType<std::size_t, RowTag>;
using Column_t = misc::StrongNumericType<std::size_t, ColumnTag>;

inline ID_t operator"" _id(unsigned long long id)
{
    return ID_t(static_cast<ID_t::type>(id));
}
inline Index_t operator"" _index(unsigned long long index)
{
    return Index_t(static_cast<Index_t::type>(index));
}
inline Count_t operator"" _count(unsigned long long count)
{
    return Count_t(static_cast<Count_t::type>(count));
}
inline Row_t operator"" _row(unsigned long long row)
{
    return Row_t(static_cast<Row_t::type>(row));
}
inline Column_t operator"" _column(unsigned long long column)
{
    return Column_t(static_cast<Column_t::type>(column));
}

using IDVec_t = std::vector<ID_t>;
using IndexVec_t = std::vector<Index_t>;
using CountVec_t = std::vector<Count_t>;


//game strong types
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
    struct StrengthTag {};
    struct AccuracyTag {};
    struct CharmTag {};
    struct LuckTag {};
    struct SpeedTag {};
    struct IntellTag {};
    

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
    using Strength_t =      misc::StrongNumericType<game::stats::Trait_t, StrengthTag>;
    using Accuracy_t =      misc::StrongNumericType<game::stats::Trait_t, AccuracyTag>;
    using Charm_t =         misc::StrongNumericType<game::stats::Trait_t, CharmTag>;
    using Luck_t =          misc::StrongNumericType<game::stats::Trait_t, LuckTag>;
    using Speed_t =         misc::StrongNumericType<game::stats::Trait_t, SpeedTag>;
    using Intell_t =        misc::StrongNumericType<game::stats::Trait_t, IntellTag>;
    

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

    inline Strength_t operator"" _str(unsigned long long strength)
    {
        return Strength_t(static_cast<Strength_t::type>(strength));
    }

    inline Accuracy_t operator"" _acc(unsigned long long accuracy)
    {
        return Accuracy_t(static_cast<Accuracy_t::type>(accuracy));
    }

    inline Charm_t operator"" _cha(unsigned long long charm)
    {
        return Charm_t(static_cast<Charm_t::type>(charm));
    }

    inline Luck_t operator"" _lck(unsigned long long luck)
    {
        return Luck_t(static_cast<Luck_t::type>(luck));
    }

    inline Speed_t operator"" _spd(unsigned long long speed)
    {
        return Speed_t(static_cast<Speed_t::type>(speed));
    }

    inline Intell_t operator"" _int(unsigned long long intell)
    {
        return Intell_t(static_cast<Intell_t::type>(intell));
    }

}

#endif //GAME_TYPES_HPP_INCLUDED
