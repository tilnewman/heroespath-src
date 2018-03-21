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
#ifndef HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCLUDED
//
// enchantment.hpp
//  Similar to a Condition, an enchantment is something that changes a creature.
//
#include "creature/condition-enum.hpp"
#include "creature/enchantment-type.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "game/phase-enum.hpp"
#include "misc/types.hpp"
#include "spell/spell-enum.hpp"
#include "stats/trait.hpp"
#include "stats/traits-set.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;

    // all the 'use' info about an enchantment
    class UseInfo
    {
    public:
        explicit UseInfo(
            const int COUNT = 0, // negative means infinite
            const game::Phase::Enum PHASE_RESTRICT = game::Phase::NotAPhase,
            const spell::Spells::Enum SPELL = spell::Spells::Count,
            const CondEnumVec_t & CONDS_REMOVED_VEC = CondEnumVec_t())
            : countOrig_(COUNT)
            , countRemain_(COUNT)
            , phase_(PHASE_RESTRICT)
            , spell_(SPELL)
            , condsRemovedVec_(CONDS_REMOVED_VEC)
        {}

        int CountOrig() const { return countOrig_; }
        int CountRemaining() const { return countRemain_; }
        void CountConsume() { --countRemain_; }
        game::Phase::Enum RestrictedToPhase() const { return phase_; }
        spell::Spells::Enum Spell() const { return spell_; }
        const CondEnumVec_t & ConditionsRemoved() const { return condsRemovedVec_; }

        friend bool operator==(const UseInfo & L, const UseInfo & R);

    private:
        // negative means infinite
        int countOrig_;
        int countRemain_;

        game::Phase::Enum phase_;
        spell::Spells::Enum spell_;
        CondEnumVec_t condsRemovedVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & countOrig_;
            ar & countRemain_;
            ar & phase_;
            ar & spell_;
            ar & condsRemovedVec_;
        }
    };

    inline bool operator==(const UseInfo & L, const UseInfo & R)
    {
        return std::tie(L.countOrig_, L.countRemain_, L.phase_, L.spell_, L.condsRemovedVec_)
            == std::tie(R.countOrig_, R.countRemain_, R.phase_, R.spell_, R.condsRemovedVec_);
    }

    inline bool operator!=(const UseInfo & L, const UseInfo & R) { return !(L == R); }

    // Responsible for storing all the info of an Enchantment,
    // and for applying and removing related changes to a creature.
    class Enchantment
    {
    public:
        explicit Enchantment(
            const EnchantmentType::Enum TYPE = EnchantmentType::None,
            const stats::TraitSet & TRAIT_SET = stats::TraitSet(),
            const UseInfo & USE_INFO = UseInfo());

        virtual ~Enchantment();

        EnchantmentType::Enum Type() const { return type_; }
        bool IsType(const EnchantmentType::Enum E) const { return (E & type_); }
        const stats::TraitSet & Traits() const { return traitSet_; }
        const UseInfo & Use() const { return useInfo_; }
        void UseCountConsume() { useInfo_.CountConsume(); }

        bool IsUseableEver() const
        {
            // negative useCountOrig_ means infinite
            return ((type_ & EnchantmentType::WhenUsed) && (useInfo_.CountOrig() != 0));
        }

        bool IsUseableNow() const { return (IsUseableEver() && (useInfo_.CountRemaining() > 0)); }

        virtual const std::string EffectStr(const CreaturePtr_t CREATURE_PTR = nullptr) const;

        virtual void CreatureChangeApply(const CreaturePtr_t) {}
        virtual void CreatureChangeRemove(const CreaturePtr_t) {}
        virtual void UseEffect(const CreaturePtr_t) {}

        virtual Score_t TreasureScore() const;

        friend bool operator==(const Enchantment & L, const Enchantment & R);

    private:
        const std::string SepIfNotEmpty(const std::string & S) const
        {
            return ((S.empty()) ? "" : ", ");
        }

    private:
        EnchantmentType::Enum type_;
        stats::TraitSet traitSet_;
        UseInfo useInfo_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & traitSet_;
            ar & useInfo_;
        }
    };

    inline bool operator==(const Enchantment & L, const Enchantment & R)
    {
        return std::tie(L.type_, L.traitSet_, L.useInfo_)
            == std::tie(R.type_, R.traitSet_, R.useInfo_);
    }

    inline bool operator!=(const Enchantment & L, const Enchantment & R) { return !(L == R); }

    using EnchantmentPtr_t = Enchantment *;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENT_HPP_INCRUDED
