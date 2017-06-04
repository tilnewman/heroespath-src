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

    const std::string DodgeType::ToString(const Enum E)
    {
        switch (E)
        {
            case Speed:         { return "Speed"; }
            case AmazingSpeed:  { return "AmazingSpeed"; }
            case Luck:          { return "Luck"; }
            case AmazingLuck:   { return "AmazingLuck"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::combat::DodgeType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string HitType::ToString(const Enum E)
    {
        switch (E)
        {
            case Accuracy:          { return "Accuracy"; }
            case AmazingAccuracy:   { return "AmazingAccuracy"; }
            case Luck:              { return "Luck"; }
            case AmazingLuck:       { return "AmazingLuck"; }
            case Spell:             { return "Spell"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::combat::HitType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    HitInfo::HitInfo(const item::ItemPtr_t                ITEM_PTR,
                     const HitType::Enum                  HIT_TYPE,
                     const DodgeType::Enum                DODGE_TYPE,
                     const stats::Health_t                DAMAGE,
                     const bool                           IS_CRITICAL_HIT,
                     const bool                           IS_POWER_HIT,
                     const creature::ConditionEnumVec_t & CONDITIONS_VEC,
                     const std::string &                  ACTION_VERB)
    :
        hitType_       (HIT_TYPE),
        dodgeType_     (DODGE_TYPE),
        weaponPtr_     (ITEM_PTR),
        damage_        (DAMAGE),
        isCritical_    (IS_CRITICAL_HIT),
        isPower_       (IS_POWER_HIT),
        conditionsVec_ (CONDITIONS_VEC),
        actionVerb_    (ACTION_VERB),
        spellPtr_      (nullptr),
        spellEffectStr_("")
    {}


    HitInfo::HitInfo(const spell::SpellPtr_t              SPELL_CPTR,
                     const std::string &                  EFFECT_STR,
                     const stats::Health_t                DAMAGE,
                     const creature::ConditionEnumVec_t & CONDITIONS_VEC,
                     const std::string &                  ACTION_PHRASE)
    :
        hitType_       (HitType::Spell),
        dodgeType_     (DodgeType::Count),
        weaponPtr_     (),
        damage_        (DAMAGE),
        isCritical_    (false),
        isPower_       (false),
        conditionsVec_ (CONDITIONS_VEC),
        actionVerb_    (ACTION_PHRASE),
        spellPtr_      (SPELL_CPTR),
        spellEffectStr_(EFFECT_STR)
    {}


    HitInfo::HitInfo(const HitInfo & HI)
    :
        hitType_      (HI.hitType_),
        dodgeType_    (HI.dodgeType_),

        //The lifetime of this object is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        weaponPtr_    (HI.weaponPtr_),

        damage_       (HI.damage_),
        isCritical_   (HI.isCritical_),
        isPower_      (HI.isPower_),
        conditionsVec_(HI.conditionsVec_),
        actionVerb_   (HI.actionVerb_),

        //The lifetime of this object is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        spellPtr_(HI.spellPtr_),

        spellEffectStr_(HI.spellEffectStr_)
    {}


    HitInfo & HitInfo::operator=(const HitInfo & HI)
    {
        if (& HI != this)
        {
            hitType_        = HI.hitType_;
            dodgeType_      = HI.dodgeType_;

            //see copy constructor comment regarding this pointer
            weaponPtr_      = HI.weaponPtr_;

            damage_         = HI.damage_;
            isCritical_     = HI.isCritical_;
            isPower_        = HI.isPower_;
            conditionsVec_  = HI.conditionsVec_;
            actionVerb_     = HI.actionVerb_;
            spellEffectStr_ = HI.spellEffectStr_;

            //see copy constructor comment regarding this pointer
            spellPtr_ = HI.spellPtr_;
        }

        return * this;
    }


    bool HitInfo::ContainsCondition(const creature::Conditions::Enum E) const
    {
        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                return true;
            }
        }

        return false;
    }


    bool HitInfo::RemoveCondition(const creature::Conditions::Enum E)
    {
        creature::ConditionEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        auto const WILL_REMOVE_A_CONDITION{ ! condsToRemoveVec.empty() };

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            conditionsVec_.erase(std::remove(conditionsVec_.begin(),
                                             conditionsVec_.end(),
                                             NEXT_CONDITION_TO_REMOVE_ENUM), conditionsVec_.end());
        }

        return WILL_REMOVE_A_CONDITION;
    }


    bool operator<(const HitInfo & L, const HitInfo & R)
    {
        if (misc::Vector::OrderlessCompareLess(L.conditionsVec_, R.conditionsVec_) == true)
        {
            return true;
        }

        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.spellEffectStr_)
               <
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.spellEffectStr_);
    }


    bool operator==(const HitInfo & L, const HitInfo & R)
    {
        if (misc::Vector::OrderlessCompareEqual(L.conditionsVec_, R.conditionsVec_) == false)
        {
            return false;
        }

        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.spellEffectStr_)
               ==
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.spellEffectStr_);
    }

}
}
