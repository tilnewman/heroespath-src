// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_HITINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_HITINFO_HPP_INCLUDED
//
// hit-info.hpp
//
#include "creature/condition-enum.hpp"
#include "creature/trait.hpp"
#include "game/strong-types.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/enum-common.hpp"
#include "misc/not-null.hpp"
#include "name-position-enum.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
    using ConditionPtrOpt_t = boost::optional<ConditionPtr_t>;
} // namespace creature
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPtrOpt_t = boost::optional<SpellPtr_t>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongPtrOpt_t = boost::optional<SongPtr_t>;
} // namespace song
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
} // namespace item

namespace combat
{

    struct HitType : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Weapon = 0,
            Spell,
            Song,
            Pounce,
            Roar,
            Condition,
            Trap,
            Count
        };
    };

    // Everything required to describe an attempted combat action.
    // Note:  It is possible to hit with a weapon and cause no damage.
    class HitInfo
    {
    public:
        // use this constructor to create invalid HitInfo objects
        HitInfo();

        // use this constructor when a weapon is used to hit
        HitInfo(
            const bool WAS_HIT,
            const item::ItemPtr_t ITEM_PTR,
            const Health_t & DAMAGE = 0_health,
            const bool IS_CRITICAL_HIT = false,
            const bool IS_POWER_HIT = false,
            const bool DID_ARMOR_ABSORB = false,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t(),
            const std::string & ACTION_VERB = "");

        // use this constructor for spells
        HitInfo(
            const bool WAS_HIT,
            const spell::SpellPtr_t SPELL_PTR,
            const ContentAndNamePos & ACTION_PHRASE_CNP,
            const Health_t & DAMAGE = 0_health,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        // use this constructor for songs
        HitInfo(
            const bool WAS_HIT,
            const song::SongPtr_t SONG_PTR,
            const ContentAndNamePos & ACTION_PHRASE_CNP,
            const Health_t & DAMAGE = 0_health,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        // use this constructor for condition effects
        HitInfo(
            const bool WAS_HIT,
            const creature::Conditions::Enum COND_ENUM,
            const ContentAndNamePos & ACTION_PHRASE_CNP,
            const Health_t & DAMAGE = 0_health,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        // use this constructor for pounce and roar
        HitInfo(
            const bool WAS_HIT,
            const HitType::Enum HIT_TYPE,
            const ContentAndNamePos & ACTION_PHRASE_CNP,
            const Health_t & DAMAGE = 0_health,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        // use this constructor for treasure trap hits
        HitInfo(
            const Health_t & DAMAGE,
            const std::string & ACTION_VERB,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        // use this constructor when the attempt failed/missed
        HitInfo(const HitType::Enum HIT_TYPE, const ContentAndNamePos & ACTION_PHRASE_CNP);

        bool WasHit() const { return wasHit_; }
        HitType::Enum TypeOfHit() const { return hitType_; }
        const item::ItemPtrOpt_t Weapon() const { return weaponPtrOpt_; }
        Health_t Damage() const { return damage_; }
        bool IsCritical() const { return isCritical_; }
        bool IsPowerHit() const { return isPower_; }
        const std::string ActionVerb() const { return actionVerb_; }
        const spell::SpellPtrOpt_t SpellPtrOpt() const { return spellPtrOpt_; }
        const song::SongPtrOpt_t SongPtrOpt() const { return songPtrOpt_; }
        bool IsSpell() const { return !!spellPtrOpt_; }
        bool IsWeapon() const { return !!weaponPtrOpt_; }

        const creature::ConditionPtrOpt_t ConditionPtrOpt() const { return conditionPtrOpt_; }

        bool DidArmorAbsorb() const { return didArmorAbsorb_; }

        const ContentAndNamePos ActionPhrase() const { return actionPhraseCNP_; }

        const creature::CondEnumVec_t CondsAdded() const { return condsAddedVec_; }

        const creature::CondEnumVec_t CondsRemoved() const { return condsRemovedVec_; }

        bool CondsAddedContains(const creature::Conditions::Enum) const;
        bool CondsAddedRemove(const creature::Conditions::Enum);

        bool CondsRemovedContains(const creature::Conditions::Enum) const;
        bool CondsRemovedRemove(const creature::Conditions::Enum);

        bool WasKill() const { return CondsAddedContains(creature::Conditions::Dead); }

        bool IsValid() const;

        bool IsCloseEnoughToEqual(const HitInfo &) const;

        const std::string ToString() const;

        friend bool operator<(const HitInfo & L, const HitInfo & R);
        friend bool operator==(const HitInfo & L, const HitInfo & R);

    private:
        bool wasHit_;
        HitType::Enum hitType_;
        item::ItemPtrOpt_t weaponPtrOpt_;
        Health_t damage_;
        bool isCritical_;
        bool isPower_;
        creature::CondEnumVec_t condsAddedVec_;
        creature::CondEnumVec_t condsRemovedVec_;
        std::string actionVerb_;
        spell::SpellPtrOpt_t spellPtrOpt_;
        ContentAndNamePos actionPhraseCNP_;
        song::SongPtrOpt_t songPtrOpt_;
        bool didArmorAbsorb_;
        creature::ConditionPtrOpt_t conditionPtrOpt_;
    };

    using HitInfoVec_t = std::vector<HitInfo>;

    bool operator<(const HitInfo & L, const HitInfo & R);

    bool operator==(const HitInfo & L, const HitInfo & R);

    inline bool operator!=(const HitInfo & L, const HitInfo & R) { return !(L == R); }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_HITINFO_HPP_INCLUDED
