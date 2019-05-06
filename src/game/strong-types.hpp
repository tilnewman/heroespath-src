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

namespace helpers
{
    // phantom type tags
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
} // namespace helpers

// strong types
using Coin_t = misc::StrongNumericType<int, helpers::CoinTag>;
using Gem_t = misc::StrongNumericType<int, helpers::GemTag>;
using Shard_t = misc::StrongNumericType<int, helpers::ShardTag>;
using Experience_t = misc::StrongNumericType<int, helpers::ExperienceTag>;
using Health_t = misc::StrongNumericType<int, helpers::HealthTag>;
using Rank_t = misc::StrongNumericType<int, helpers::RankTag>;
using Mana_t = misc::StrongNumericType<int, helpers::ManaTag>;
using Weight_t = misc::StrongNumericType<int, helpers::WeightTag>;
using Armor_t = misc::StrongNumericType<int, helpers::ArmorTag>;
using Score_t = misc::StrongNumericType<int, helpers::ScoreTag>;
using Strength_t = misc::StrongNumericType<creature::Trait_t, helpers::StrengthTag>;
using Accuracy_t = misc::StrongNumericType<creature::Trait_t, helpers::AccuracyTag>;
using Charm_t = misc::StrongNumericType<creature::Trait_t, helpers::CharmTag>;
using Luck_t = misc::StrongNumericType<creature::Trait_t, helpers::LuckTag>;
using Speed_t = misc::StrongNumericType<creature::Trait_t, helpers::SpeedTag>;
using Intel_t = misc::StrongNumericType<creature::Trait_t, helpers::IntellTag>;

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

} // namespace heroespath

#endif // HEROESPATH_GAME_STRONG_TYPES_HPP_INCLUDED
