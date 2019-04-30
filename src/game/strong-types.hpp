// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_STRONG_TYPES_HPP_INCLUDED
#define HEROESPATH_GAME_STRONG_TYPES_HPP_INCLUDED
//
// strong-types.hpp
//
#include "creature/trait-type.hpp"
#include "misc/strong-numeric-type.hpp"

#include <string>

namespace heroespath
{

// phantom type tags
struct NameTag
{};
struct CoinTag
{};
struct GemTag
{};
struct ShardTag
{};
struct ExperienceTag
{};
struct HealthTag
{};
struct RankTag
{};
struct ManaTag
{};
struct WeightTag
{};
struct ArmorTag
{};
struct ScoreTag
{};
struct StrengthTag
{};
struct AccuracyTag
{};
struct CharmTag
{};
struct LuckTag
{};
struct SpeedTag
{};
struct IntellTag
{};
struct IDTag
{};
struct IndexTag
{};
struct CountTag
{};

// strong types
using Name_t = misc::StrongType<std::string, NameTag>;
using Coin_t = misc::StrongNumericType<int, CoinTag>;
using Gem_t = misc::StrongNumericType<int, GemTag>;
using Shard_t = misc::StrongNumericType<int, ShardTag>;
using Experience_t = misc::StrongNumericType<int, ExperienceTag>;
using Health_t = misc::StrongNumericType<int, HealthTag>;
using Rank_t = misc::StrongNumericType<int, RankTag>;
using Mana_t = misc::StrongNumericType<int, ManaTag>;
using Weight_t = misc::StrongNumericType<int, WeightTag>;
using Armor_t = misc::StrongNumericType<int, ArmorTag>;
using Score_t = misc::StrongNumericType<int, ScoreTag>;
using Strength_t = misc::StrongNumericType<creature::Trait_t, StrengthTag>;
using Accuracy_t = misc::StrongNumericType<creature::Trait_t, AccuracyTag>;
using Charm_t = misc::StrongNumericType<creature::Trait_t, CharmTag>;
using Luck_t = misc::StrongNumericType<creature::Trait_t, LuckTag>;
using Speed_t = misc::StrongNumericType<creature::Trait_t, SpeedTag>;
using Intel_t = misc::StrongNumericType<creature::Trait_t, IntellTag>;
using ID_t = misc::StrongNumericType<std::size_t, IDTag>;
using Index_t = misc::StrongNumericType<std::size_t, IndexTag>;
using Count_t = misc::StrongNumericType<std::size_t, CountTag>;

// user defined literals
inline const Coin_t operator"" _coin(unsigned long long coins)
{
    return Coin_t::Make(static_cast<Coin_t::value_type>(coins));
}

inline const Gem_t operator"" _gem(unsigned long long gems)
{
    return Gem_t::Make(static_cast<Gem_t::value_type>(gems));
}

inline const Shard_t operator"" _shard(unsigned long long shards)
{
    return Shard_t::Make(static_cast<Shard_t::value_type>(shards));
}

inline const Experience_t operator"" _exp(unsigned long long exp)
{
    return Experience_t::Make(static_cast<Experience_t::value_type>(exp));
}

inline const Health_t operator"" _health(unsigned long long health)
{
    return Health_t::Make(static_cast<Health_t::value_type>(health));
}

inline const Rank_t operator"" _rank(unsigned long long rank)
{
    return Rank_t::Make(static_cast<Rank_t::value_type>(rank));
}

inline const Mana_t operator"" _mana(unsigned long long mana)
{
    return Mana_t::Make(static_cast<Mana_t::value_type>(mana));
}

inline const Weight_t operator"" _weight(unsigned long long weight)
{
    return Weight_t::Make(static_cast<Weight_t::value_type>(weight));
}

inline const Armor_t operator"" _armor(unsigned long long armor)
{
    return Armor_t::Make(static_cast<Armor_t::value_type>(armor));
}

inline const Score_t operator"" _score(unsigned long long score)
{
    return Score_t::Make(static_cast<Score_t::value_type>(score));
}

inline const Strength_t operator"" _str(unsigned long long strength)
{
    return Strength_t::Make(static_cast<Strength_t::value_type>(strength));
}

inline const Accuracy_t operator"" _acc(unsigned long long accuracy)
{
    return Accuracy_t::Make(static_cast<Accuracy_t::value_type>(accuracy));
}

inline const Charm_t operator"" _cha(unsigned long long charm)
{
    return Charm_t::Make(static_cast<Charm_t::value_type>(charm));
}

inline const Luck_t operator"" _lck(unsigned long long luck)
{
    return Luck_t::Make(static_cast<Luck_t::value_type>(luck));
}

inline const Speed_t operator"" _spd(unsigned long long speed)
{
    return Speed_t::Make(static_cast<Speed_t::value_type>(speed));
}

inline const Intel_t operator"" _int(unsigned long long intell)
{
    return Intel_t::Make(static_cast<Intel_t::value_type>(intell));
}

inline const ID_t operator"" _id(unsigned long long id)
{
    return ID_t::Make(static_cast<ID_t::value_type>(id));
}

inline const Index_t operator"" _index(unsigned long long index)
{
    return Index_t::Make(static_cast<Index_t::value_type>(index));
}

inline const Count_t operator"" _count(unsigned long long count)
{
    return Count_t::Make(static_cast<Count_t::value_type>(count));
}

} // namespace heroespath

#endif // HEROESPATH_GAME_STRONG_TYPES_HPP_INCLUDED
