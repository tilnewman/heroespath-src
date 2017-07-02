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
#ifndef GAME_COMBAT_HITINFO_HPP_INCLUDED
#define GAME_COMBAT_HITINFO_HPP_INCLUDED
//
// hit-info.hpp
//
#include "game/stats/types.hpp"
#include "game/creature/condition-enum.hpp"
#include "game/name-position-enum.hpp"

#include <string>
#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    class Condition;
    using ConditionPtr_t = Condition *;
}
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace song
{
    class Song;
    using SongPtr_t = Song *;
}
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}

namespace combat
{

    struct HitType
    {
        enum Enum
        {
            Weapon,
            Spell,
            Song,
            Pounce,
            Roar,
            Condition,
            Count
        };

        static const std::string ToString(const Enum);
    };


    //Everything required to describe an attempted combat action.
    //Note:  It is possible to hit with a weapon and cause no damage.
    class HitInfo
    {
    public:
        //use this constructor when a weapon is used to hit
        explicit HitInfo(
            const bool                      WAS_HIT           = false,
            const item::ItemPtr_t           ITEM_PTR          = nullptr,
            const stats::Health_t           DAMAGE            = 0,
            const bool                      IS_CRITICAL_HIT   = false,
            const bool                      IS_POWER_HIT      = false,
            const bool                      DID_ARMOR_ABSORB  = false,
            const creature::CondEnumVec_t & CONDS_ADDED_VEC   = creature::CondEnumVec_t(),
            const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t(),
            const std::string &             ACTION_VERB       = "");

        //use this constructor for spells
        HitInfo(const bool                      WAS_HIT,
                const spell::SpellPtr_t         SPELL_CPTR,
                const ContentAndNamePos &       ACTION_PHRASE_CNP,
                const stats::Health_t           DAMAGE            = 0,
                const creature::CondEnumVec_t & CONDS_ADDED_VEC   = creature::CondEnumVec_t(),
                const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        //use this constructor for songs
        HitInfo(const bool                      WAS_HIT,
                const song::SongPtr_t           SONG_CPTR,
                const ContentAndNamePos &       ACTION_PHRASE_CNP,
                const stats::Health_t           DAMAGE            = 0,
                const creature::CondEnumVec_t & CONDS_ADDED_VEC   = creature::CondEnumVec_t(),
                const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        //use this constructor for condition effects
        HitInfo(const bool                       WAS_HIT,
                const creature::Conditions::Enum COND_ENUM,
                const ContentAndNamePos &        ACTION_PHRASE_CNP,
                const stats::Health_t            DAMAGE            = 0,
                const creature::CondEnumVec_t &  CONDS_ADDED_VEC   = creature::CondEnumVec_t(),
                const creature::CondEnumVec_t &  CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        //use this constructor for pounce and roar
        HitInfo(const bool                      WAS_HIT,
                const HitType::Enum             HIT_TYPE,
                const ContentAndNamePos &       ACTION_PHRASE_CNP,
                const stats::Health_t           DAMAGE            = 0,
                const creature::CondEnumVec_t & CONDS_ADDED_VEC   = creature::CondEnumVec_t(),
                const creature::CondEnumVec_t & CONDS_REMOVED_VEC = creature::CondEnumVec_t());

        //use this constructor when the attempt failed/missed
        HitInfo(const HitType::Enum HIT_TYPE, const ContentAndNamePos & ACTION_PHRASE_CNP);

        HitInfo(const HitInfo &);
        HitInfo & operator=(const HitInfo &);

        inline bool              WasHit() const     { return wasHit_; }
        inline HitType::Enum     TypeOfHit() const  { return hitType_; }
        inline item::ItemPtr_t   Weapon() const     { return weaponPtr_; }
        inline stats::Health_t   Damage() const     { return damage_; }
        inline bool              IsCritical() const { return isCritical_; }
        inline bool              IsPowerHit() const { return isPower_; }
        inline const std::string ActionVerb() const { return actionVerb_; }
        inline spell::SpellPtr_t SpellPtr() const   { return spellPtr_; }
        inline song::SongPtr_t   SongPtr() const    { return songPtr_; }
        inline bool              IsSpell() const    { return (spellPtr_ != nullptr); }
        inline bool              IsWeapon() const   { return (weaponPtr_ != nullptr); }
        
        inline creature::ConditionPtr_t ConditionPtr() const { return conditionPtr_; }
        
        inline bool DidArmorAbsorb() const { return didArmorAbsorb_; }
        
        inline const ContentAndNamePos ActionPhrase() const
        {
            return actionPhraseCNP_;
        }

        inline const creature::CondEnumVec_t CondsAdded() const
        {
            return condsAddedVec_;
        }

        inline const creature::CondEnumVec_t CondsRemoved() const
        {
            return condsRemovedVec_;
        }

        bool CondsAddedContains(const creature::Conditions::Enum) const;
        bool CondsAddedRemove(const creature::Conditions::Enum);

        bool CondsRemovedContains(const creature::Conditions::Enum) const;
        bool CondsRemovedRemove(const creature::Conditions::Enum);

        inline bool WasKill() const { return CondsAddedContains(creature::Conditions::Dead); }

        bool IsValid() const;

        bool IsCloseEnoughToEqual(const HitInfo &) const;

        const std::string ToString() const;

        friend bool operator<(const HitInfo & L, const HitInfo & R);
        friend bool operator==(const HitInfo & L, const HitInfo & R);

    private:
        bool                     wasHit_;
        HitType::Enum            hitType_;
        item::ItemPtr_t          weaponPtr_;
        stats::Health_t          damage_;
        bool                     isCritical_;
        bool                     isPower_;
        creature::CondEnumVec_t  condsAddedVec_;
        creature::CondEnumVec_t  condsRemovedVec_;
        std::string              actionVerb_;
        spell::SpellPtr_t        spellPtr_;
        ContentAndNamePos        actionPhraseCNP_;
        song::SongPtr_t          songPtr_;
        bool                     didArmorAbsorb_;
        creature::ConditionPtr_t conditionPtr_;
    };

    using HitInfoVec_t = std::vector<HitInfo>;


    bool operator<(const HitInfo & L, const HitInfo & R);

    bool operator==(const HitInfo & L, const HitInfo & R);

    inline bool operator!=(const HitInfo & L, const HitInfo & R)
    {
        return ! (L == R);
    }

}
}

#endif //GAME_COMBAT_HITINFO_HPP_INCLUDED
