// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
#define HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
//
// condition.hpp
//  Code that places a Creature under a temporary condition.
//  Examples would be poisoned, unconcious, etc.
//
#include "combat/hit-info.hpp"
#include "creature/condition-enum.hpp"
#include "creature/stat-set.hpp"
#include "creature/traits-set.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{
    class CreatureInteraction;
}
namespace creature
{

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    // Responsible for all state and operation of all Conditions in the game.
    class Condition
    {
    public:
        Condition(const Condition &) = delete;
        Condition(Condition &&) = delete;
        Condition & operator=(const Condition &) = delete;
        Condition & operator=(Condition &&) = delete;

        explicit Condition(
            const Conditions::Enum TYPE = Conditions::Good,
            const bool IS_MAGICAL = false,
            const TraitSet & TRAIT_SET = TraitSet());

        const std::string Name() const { return Conditions::Name(type_); }
        Conditions::Enum Which() const { return type_; }
        const std::string Desc() const { return Conditions::Desc(type_); }
        const std::string ToString() const;
        const std::string LongDesc() const;
        std::size_t Severity() const { return condition::Severity::Get(type_); }
        bool IsMagical() const { return isMagical_; }
        const TraitSet Traits() const { return traitSet_; }

        // These two functions do not alter traits
        void InitialChange(const CreaturePtr_t) const;
        void FinalChange(const CreaturePtr_t) const;

        void PerTurnEffect(
            const combat::CreatureInteraction & CREATURE_INTERACTION,
            const CreaturePtr_t CREATURE_PTR,
            combat::HitInfoVec_t & hitInfoVec,
            bool & hasTurnBeenConsumed) const;

        friend bool operator<(const Condition & L, const Condition & R);
        friend bool operator==(const Condition & L, const Condition & R);

    private:
        Conditions::Enum type_;
        bool isMagical_;
        TraitSet traitSet_;
    };

    using ConditionPtr_t = misc::NotNull<Condition *>;
    using ConditionPtrOpt_t = boost::optional<ConditionPtr_t>;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;

    bool operator<(const Condition & L, const Condition & R);

    bool operator==(const Condition & L, const Condition & R);

    inline bool operator!=(const Condition & L, const Condition & R) { return !(L == R); }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
