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
#ifndef HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
#define HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
//
// condition.hpp
//  Code that places a Creature under a temporary condition.
//  Examples would be poisoned, unconcious, etc.
//
#include "combat/hit-info.hpp"
#include "creature/condition-enum.hpp"
#include "stats/stat-set.hpp"
#include "stats/traits-set.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    // forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePtrC_t = Creature * const;

    // base class for all conditions
    class Condition
    {
        Condition(const Condition &) = delete;
        Condition & operator=(const Condition &) = delete;

    public:
        explicit Condition(
            const Conditions::Enum TYPE = Conditions::Good,
            const bool IS_MAGICAL = false,
            const stats::TraitSet & TRAIT_SET = stats::TraitSet());

        virtual ~Condition();

        inline const std::string Name() const { return Conditions::Name(type_); }
        inline Conditions::Enum Which() const { return type_; }
        inline const std::string Desc() const { return Conditions::Desc(type_); }
        const std::string ToString() const;
        const std::string LongDesc() const;
        inline std::size_t Severity() const { return condition::Severity::Get(type_); }
        inline bool IsMagical() const { return isMagical_; }
        inline const stats::TraitSet Traits() const { return traitSet_; }

        // These two functions do not alter traits
        virtual void InitialChange(CreaturePtrC_t) const;
        virtual void FinalChange(CreaturePtrC_t) const;

        virtual void PerTurnEffect(
            CreaturePtr_t creaturePtr,
            combat::HitInfoVec_t & hitInfoVec,
            bool & hasTurnBeenConsumed) const;

        friend bool operator<(const Condition & L, const Condition & R);
        friend bool operator==(const Condition & L, const Condition & R);

    private:
        Conditions::Enum type_;
        bool isMagical_;
        stats::TraitSet traitSet_;
    };

    using ConditionPtr_t = Condition *;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;

    bool operator<(const Condition & L, const Condition & R);

    bool operator==(const Condition & L, const Condition & R);

    inline bool operator!=(const Condition & L, const Condition & R) { return !(L == R); }
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CONDITION_HPP_INCLUDED
