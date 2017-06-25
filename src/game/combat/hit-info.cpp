// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// hit-info.cpp
//
#include "hit-info.hpp"

#include "misc/vectors.hpp"

#include <sstream>
#include <exception>
#include <algorithm>
#include <tuple>


namespace game
{
namespace combat
{

    const std::string HitType::ToString(const Enum E)
    {
        switch (E)
        {
            case Weapon:    { return "Weapon"; }
            case Spell:     { return "Spell"; }
            case Song:      { return "Song"; }
            case Pounce:    { return "Pounce"; }
            case Roar:      { return "Roar"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::combat::HitType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    HitInfo::HitInfo(const bool                      WAS_HIT,
                     const item::ItemPtr_t           ITEM_PTR,
                     const stats::Health_t           DAMAGE,
                     const bool                      IS_CRITICAL_HIT,
                     const bool                      IS_POWER_HIT,
                     const bool                      DID_ARMOR_ABSORB,
                     const creature::CondEnumVec_t & CONDS_ADDED_VEC,
                     const creature::CondEnumVec_t & CONDS_REMOVED_VEC,
                     const std::string &             ACTION_VERB)
    :
        wasHit_         (WAS_HIT),
        hitType_        (HitType::Weapon),
        weaponPtr_      (ITEM_PTR),
        damage_         (DAMAGE),
        isCritical_     (IS_CRITICAL_HIT),
        isPower_        (IS_POWER_HIT),
        condsAddedVec_  (CONDS_ADDED_VEC),
        condsRemovedVec_(CONDS_REMOVED_VEC),
        actionVerb_     (ACTION_VERB),
        spellPtr_       (nullptr),
        actionPhraseCNP_(),
        songPtr_        (nullptr),
        didArmorAbsorb_ (DID_ARMOR_ABSORB)
    {}


    HitInfo::HitInfo(const bool                      WAS_HIT,
                     const spell::SpellPtr_t         SPELL_CPTR,
                     const ContentAndNamePos &       ACTION_PHRASE_CNP,
                     const stats::Health_t           DAMAGE,
                     const creature::CondEnumVec_t & CONDS_ADDED_VEC,
                     const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
    :
        wasHit_         (WAS_HIT),
        hitType_        (HitType::Spell),
        weaponPtr_      (nullptr),
        damage_         (DAMAGE),
        isCritical_     (false),
        isPower_        (false),
        condsAddedVec_  (CONDS_ADDED_VEC),
        condsRemovedVec_(CONDS_REMOVED_VEC),
        actionVerb_     ("casts"),
        spellPtr_       (SPELL_CPTR),
        actionPhraseCNP_(ACTION_PHRASE_CNP),
        songPtr_        (nullptr),
        didArmorAbsorb_ (false)
    {}


    HitInfo::HitInfo(const bool                      WAS_HIT,
                     const song::SongPtr_t           SONG_CPTR,
                     const ContentAndNamePos &       ACTION_PHRASE_CNP,
                     const stats::Health_t           DAMAGE,
                     const creature::CondEnumVec_t & CONDS_ADDED_VEC,
                     const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
    :
        wasHit_         (WAS_HIT),
        hitType_        (HitType::Song),
        weaponPtr_      (nullptr),
        damage_         (DAMAGE),
        isCritical_     (false),
        isPower_        (false),
        condsAddedVec_  (CONDS_ADDED_VEC),
        condsRemovedVec_(CONDS_REMOVED_VEC),
        actionVerb_     ("plays"),
        spellPtr_       (nullptr),
        actionPhraseCNP_(ACTION_PHRASE_CNP),
        songPtr_        (SONG_CPTR),
        didArmorAbsorb_ (false)
    {}


    HitInfo::HitInfo(const bool                      WAS_HIT,
                     const HitType::Enum             HIT_TYPE,
                     const ContentAndNamePos &       ACTION_PHRASE_CNP,
                     const stats::Health_t           DAMAGE,
                     const creature::CondEnumVec_t & CONDS_ADDED_VEC,
                     const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
    :
        wasHit_         (WAS_HIT),
        hitType_        (HIT_TYPE),
        weaponPtr_      (nullptr),
        damage_         (DAMAGE),
        isCritical_     (false),
        isPower_        (false),
        condsAddedVec_  (CONDS_ADDED_VEC),
        condsRemovedVec_(CONDS_REMOVED_VEC),
        actionVerb_     (""),
        spellPtr_       (nullptr),
        actionPhraseCNP_(ACTION_PHRASE_CNP),
        songPtr_        (nullptr),
        didArmorAbsorb_ (false)
    {}


    HitInfo::HitInfo(const HitType::Enum       HIT_TYPE,
                     const ContentAndNamePos & ACTION_PHRASE_CNP)
    :
        wasHit_         (false),
        hitType_        (HIT_TYPE),
        weaponPtr_      (nullptr),
        damage_         (0),
        isCritical_     (false),
        isPower_        (false),
        condsAddedVec_  (),
        condsRemovedVec_(),
        actionVerb_     (""),
        spellPtr_       (nullptr),
        actionPhraseCNP_(ACTION_PHRASE_CNP),
        songPtr_        (nullptr),
        didArmorAbsorb_ (false)
    {}


    HitInfo::HitInfo(const HitInfo & HI)
    :
        wasHit_(HI.wasHit_),
        hitType_(HI.hitType_),
        
        //The lifetime of this object is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        weaponPtr_(HI.weaponPtr_),

        damage_         (HI.damage_),
        isCritical_     (HI.isCritical_),
        isPower_        (HI.isPower_),
        condsAddedVec_  (HI.condsAddedVec_),
        condsRemovedVec_(HI.condsRemovedVec_),
        actionVerb_     (HI.actionVerb_),

        //see the comment above regarding pointers in this class
        spellPtr_(HI.spellPtr_),

        actionPhraseCNP_(HI.actionPhraseCNP_),

        //see the comment above regarding pointers in this class
        songPtr_(HI.songPtr_),

        didArmorAbsorb_(HI.didArmorAbsorb_)
    {}


    HitInfo & HitInfo::operator=(const HitInfo & HI)
    {
        if (& HI != this)
        {
            wasHit_ = HI.wasHit_;
            hitType_ = HI.hitType_;
            damage_ = HI.damage_;
            isCritical_ = HI.isCritical_;
            isPower_ = HI.isPower_;
            condsAddedVec_ = HI.condsAddedVec_;
            condsRemovedVec_ = HI.condsRemovedVec_;
            actionVerb_ = HI.actionVerb_;
            actionPhraseCNP_ = HI.actionPhraseCNP_;
            didArmorAbsorb_ = HI.didArmorAbsorb_;

            //see copy constructor comment regarding these pointers
            weaponPtr_ = HI.weaponPtr_;
            spellPtr_ = HI.spellPtr_;
            songPtr_ = HI.songPtr_;
        }

        return * this;
    }


    bool HitInfo::CondsAddedContains(const creature::Conditions::Enum E) const
    {
        return (std::find(condsAddedVec_.begin(), condsAddedVec_.end(), E) !=
            condsAddedVec_.end());
    }


    bool HitInfo::CondsAddedRemove(const creature::Conditions::Enum E)
    {
        creature::CondEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : condsAddedVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        if (condsToRemoveVec.empty())
        {
            return false;
        }

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            condsAddedVec_.erase(std::remove(condsAddedVec_.begin(),
                                             condsAddedVec_.end(),
                                             NEXT_CONDITION_TO_REMOVE_ENUM),
                                 condsAddedVec_.end());
        }

        return true;
    }


    bool HitInfo::CondsRemovedContains(const creature::Conditions::Enum E) const
    {
        return (std::find(condsRemovedVec_.begin(), condsRemovedVec_.end(), E) !=
            condsRemovedVec_.end());
    }


    bool HitInfo::CondsRemovedRemove(const creature::Conditions::Enum E)
    {
        creature::CondEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : condsRemovedVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        if (condsToRemoveVec.empty())
        {
            return false;
        }

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            condsAddedVec_.erase(std::remove(condsRemovedVec_.begin(),
                                             condsRemovedVec_.end(),
                                             NEXT_CONDITION_TO_REMOVE_ENUM),
                                 condsRemovedVec_.end());
        }

        return true;
    }


    bool HitInfo::IsValid() const
    {
        switch (hitType_)
        {
            case HitType::Weapon:
            {
                return (weaponPtr_ != nullptr);
            }
            case HitType::Spell:
            {
                return ((spellPtr_ != nullptr) &&
                        (actionPhraseCNP_.NamePos() != NamePosition::Count));
            }
            case HitType::Song:
            {
                return ((songPtr_ != nullptr) &&
                        (actionPhraseCNP_.NamePos() != NamePosition::Count));
            }
            case HitType::Pounce:
            case HitType::Roar:
            {
                return (actionPhraseCNP_.NamePos() != NamePosition::Count);
            }
            case HitType::Count:
            default:
            {
                return false;
            }
        }
    }


    bool HitInfo::IsCloseEnoughToEqual(const HitInfo & HI) const
    {
        if (misc::Vector::OrderlessCompareEqual(condsAddedVec_, HI.condsAddedVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(condsRemovedVec_, HI.condsRemovedVec_) == false)
        {
            return false;
        }

        return std::tie(hitType_,
                        wasHit_,
                        weaponPtr_,
                        damage_,
                        isCritical_,
                        isPower_,
                        spellPtr_,
                        songPtr_,
                        didArmorAbsorb_)
               ==
               std::tie(HI.hitType_,
                        HI.wasHit_,
                        HI.weaponPtr_,
                        HI.damage_,
                        HI.isCritical_,
                        HI.isPower_,
                        HI.spellPtr_,
                        HI.songPtr_,
                        HI.didArmorAbsorb_);
    }


    bool operator<(const HitInfo & L, const HitInfo & R)
    {
        if (misc::Vector::OrderlessCompareLess(L.condsAddedVec_, R.condsAddedVec_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.condsRemovedVec_, R.condsRemovedVec_))
        {
            return true;
        }

        return std::tie(L.hitType_,
                        L.wasHit_,
                        L.weaponPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.actionPhraseCNP_,
                        L.songPtr_,
                        L.didArmorAbsorb_)
               <
               std::tie(R.hitType_,
                        R.wasHit_,
                        R.weaponPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.actionPhraseCNP_,
                        R.songPtr_,
                        R.didArmorAbsorb_);
    }


    bool operator==(const HitInfo & L, const HitInfo & R)
    {
        if (misc::Vector::OrderlessCompareEqual(L.condsAddedVec_, R.condsAddedVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.condsRemovedVec_, R.condsRemovedVec_) == false)
        {
            return false;
        }

        return std::tie(L.hitType_,
                        L.wasHit_,
                        L.weaponPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.actionPhraseCNP_,
                        L.songPtr_,
                        L.didArmorAbsorb_)
               ==
               std::tie(R.hitType_,
                        R.wasHit_,
                        R.weaponPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.actionPhraseCNP_,
                        R.songPtr_,
                        R.didArmorAbsorb_);
    }

}
}
