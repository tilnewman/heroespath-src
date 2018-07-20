// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_TYPES_HPP_INCLUDED
#define HEROESPATH_MISC_TYPES_HPP_INCLUDED
//
// types.hpp
//  Strong types used by the game.
//
#include "creature/trait.hpp"

#include "misc/strong-numeric-type.hpp"
#include "misc/strong-type.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{

// phantom type tags
struct NameTag
{};
struct CoinTag
{};
struct GemTag
{};
struct MeteorShardTag
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
struct RowTag
{};
struct ColumnTag
{};

// strong types
using Name_t = misc::StrongType<std::string, NameTag>;
using Coin_t = misc::StrongNumericType<int, CoinTag>;
using Gem_t = misc::StrongNumericType<int, GemTag>;
using MeteorShard_t = misc::StrongNumericType<int, MeteorShardTag>;
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
using Intell_t = misc::StrongNumericType<creature::Trait_t, IntellTag>;
using ID_t = misc::StrongNumericType<std::size_t, IDTag>;
using Index_t = misc::StrongNumericType<std::size_t, IndexTag>;
using Count_t = misc::StrongNumericType<std::size_t, CountTag>;
using Row_t = misc::StrongNumericType<std::size_t, RowTag>;
using Column_t = misc::StrongNumericType<std::size_t, ColumnTag>;

// user defined literals
inline Coin_t operator"" _coin(unsigned long long coins) { return Coin_t::Make(coins); }

inline Gem_t operator"" _gem(unsigned long long gems) { return Gem_t::Make(gems); }

inline MeteorShard_t operator"" _mshard(unsigned long long mshards)
{
    return MeteorShard_t::Make(mshards);
}

inline Experience_t operator"" _exp(unsigned long long exp) { return Experience_t::Make(exp); }

inline Health_t operator"" _health(unsigned long long health) { return Health_t::Make(health); }

inline Rank_t operator"" _rank(unsigned long long rank) { return Rank_t::Make(rank); }

inline Mana_t operator"" _mana(unsigned long long mana) { return Mana_t::Make(mana); }

inline Weight_t operator"" _weight(unsigned long long weight) { return Weight_t::Make(weight); }

inline Armor_t operator"" _armor(unsigned long long armor) { return Armor_t::Make(armor); }

inline Score_t operator"" _score(unsigned long long score) { return Score_t::Make(score); }

inline Strength_t operator"" _str(unsigned long long strength)
{
    return Strength_t::Make(strength);
}

inline Accuracy_t operator"" _acc(unsigned long long accuracy)
{
    return Accuracy_t::Make(accuracy);
}

inline Charm_t operator"" _cha(unsigned long long charm) { return Charm_t::Make(charm); }

inline Luck_t operator"" _lck(unsigned long long luck) { return Luck_t::Make(luck); }

inline Speed_t operator"" _spd(unsigned long long speed) { return Speed_t::Make(speed); }

inline Intell_t operator"" _int(unsigned long long intell) { return Intell_t::Make(intell); }

inline ID_t operator"" _id(unsigned long long id) { return ID_t::Make(id); }

inline Index_t operator"" _index(unsigned long long index) { return Index_t::Make(index); }

inline Count_t operator"" _count(unsigned long long count) { return Count_t::Make(count); }

inline Row_t operator"" _row(unsigned long long row) { return Row_t::Make(row); }

inline Column_t operator"" _column(unsigned long long column) { return Column_t::Make(column); }

// commonly container types
using IDVec_t = std::vector<ID_t>;
using IndexVec_t = std::vector<Index_t>;
using CountVec_t = std::vector<Count_t>;

} // namespace heroespath

#endif // HEROESPATH_MISC_TYPES_HPP_INCLUDED
