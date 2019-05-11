// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SPELL_SPELL_HPP_INCLUDED
#define HEROESPATH_SPELL_SPELL_HPP_INCLUDED
//
// spell.hpp
//
#include "combat/effect-type-enum.hpp"
#include "combat/name-position-enum.hpp"
#include "combat/target-enum.hpp"
#include "creature/condition-enum.hpp"
#include "game/phase-enum.hpp"
#include "game/strong-types.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "spell/spell-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{

// forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item

namespace spell
{

    // Responsible for wrapping all information about a spell in the game.
    class Spell
    {
    public:
        Spell(
            const Spells::Enum WHICH,
            const combat::EffectType::Enum EFFECT_TYPE,
            const game::Phase::Enum VALID_PHASES,
            const Mana_t & MANA_COST,
            const Rank_t & RANK,
            const combat::TargetType::Enum TARGET_TYPE,
            const std::string & VERB_THIRD_PERSON,
            const std::string & VERB_PAST_TENSE);

        const std::string Name() const { return std::string(Spells::Name(which_)); }

        const std::string ToString() const;

        const std::string VerbThirdPerson() const { return verbThirdPerson_; }
        const std::string VerbPastTense() const { return verbPastTense_; }

        const std::string Desc() const { return Spells::ShortDesc(which_); }
        const std::string DescExtra() const { return Spells::ExtraDesc(which_); }
        const std::string DescDetails() const;
        const std::string DescComplete() const;

        Spells::Enum Which() const { return which_; }
        combat::EffectType::Enum Effect() const { return effectType_; }
        game::Phase::Enum ValidPhases() const { return validPhases_; }
        Mana_t ManaCost() const { return manaCost_; }
        Rank_t Rank() const { return rank_; }
        combat::TargetType::Enum Target() const { return targetType_; }

        const combat::ContentAndNamePos
            ActionPhrase(const creature::CreaturePtr_t CREATURE_CAST_UPON_PTR) const;

        // Allows the spell to change the target creature.
        // Don't adjust creatureCastUponPtr's health, that will be done in Fight.cpp.
        // Don't check if creatureCastUponPtr is already dead, that will be done in fight.cpp
        bool EffectCreature(
            const creature::CreaturePtr_t CREATURE_CASTING_PTR,
            const creature::CreaturePtr_t CREATURE_CAST_UPON_PTR,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

        // Allows the spell to change the target item.
        const std::string EffectItem(const creature::CreaturePtr_t, const item::ItemPtr_t) const;

        friend bool operator<(const Spell & L, const Spell & R);
        friend bool operator==(const Spell & L, const Spell & R);

    public:
        static const std::string RESISTED_STR_;
        static const std::string FAILED_BECAUSE_STR_;

    protected:
        Spells::Enum which_;
        Rank_t rank_;
        combat::EffectType::Enum effectType_;
        game::Phase::Enum validPhases_;
        Mana_t manaCost_;
        combat::TargetType::Enum targetType_;
        std::string verbThirdPerson_;
        std::string verbPastTense_;
    };

    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPtrOpt_t = boost::optional<SpellPtr_t>;
    using SpellPVec_t = std::vector<SpellPtr_t>;

    inline bool operator<(const Spell & L, const Spell & R)
    {
        return std::tie(
                   L.which_,
                   L.rank_,
                   L.effectType_,
                   L.validPhases_,
                   L.manaCost_,
                   L.targetType_,
                   L.verbThirdPerson_,
                   L.verbPastTense_)
            < std::tie(
                   R.which_,
                   R.rank_,
                   R.effectType_,
                   R.validPhases_,
                   R.manaCost_,
                   R.targetType_,
                   R.verbThirdPerson_,
                   R.verbPastTense_);
    }

    inline bool operator==(const Spell & L, const Spell & R)
    {
        return std::tie(
                   L.which_,
                   L.rank_,
                   L.effectType_,
                   L.validPhases_,
                   L.manaCost_,
                   L.targetType_,
                   L.verbThirdPerson_,
                   L.verbPastTense_)
            == std::tie(
                   R.which_,
                   R.rank_,
                   R.effectType_,
                   R.validPhases_,
                   R.manaCost_,
                   R.targetType_,
                   R.verbThirdPerson_,
                   R.verbPastTense_);
    }

    inline bool operator!=(const Spell & L, const Spell & R) { return !(L == R); }

} // namespace spell
} // namespace heroespath

#endif // HEROESPATH_SPELL_SPELL_HPP_INCLUDED
