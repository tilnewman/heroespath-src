// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// hit-info.cpp
//
#include "hit-info.hpp"

#include "creature/condition-holder.hpp"
#include "creature/condition.hpp"
#include "item/item.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <algorithm>
#include <tuple>

namespace heroespath
{
namespace combat
{

    const std::string HitType::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case Weapon:
            {
                return "Weapon";
            }
            case Spell:
            {
                return "Spell";
            }
            case Song:
            {
                return "Song";
            }
            case Pounce:
            {
                return "Pounce";
            }
            case Roar:
            {
                return "Roar";
            }
            case Condition:
            {
                return "Condition";
            }
            case Trap:
            {
                return "Trap";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(ENUM));
                return "";
            }
        }
    }

    HitInfo::HitInfo()
        : wasHit_(false)
        , hitType_(HitType::Count)
        , weaponPtrOpt_()
        , damage_(0_health)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_()
        , condsRemovedVec_()
        , actionVerb_("")
        , spellPtrOpt_()
        , actionPhraseCNP_()
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(
        const bool WAS_HIT,
        const item::ItemPtr_t ITEM_PTR,
        const Health_t & DAMAGE,
        const bool IS_CRITICAL_HIT,
        const bool IS_POWER_HIT,
        const bool DID_ARMOR_ABSORB,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC,
        const std::string & ACTION_VERB)
        : wasHit_(WAS_HIT)
        , hitType_(HitType::Weapon)
        , weaponPtrOpt_(ITEM_PTR)
        , damage_(DAMAGE)
        , isCritical_(IS_CRITICAL_HIT)
        , isPower_(IS_POWER_HIT)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_(ACTION_VERB)
        , spellPtrOpt_()
        , actionPhraseCNP_()
        , songPtrOpt_()
        , didArmorAbsorb_(DID_ARMOR_ABSORB)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(
        const bool WAS_HIT,
        const spell::SpellPtr_t SPELL_PTR,
        const ContentAndNamePos & ACTION_PHRASE_CNP,
        const Health_t & DAMAGE,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
        : wasHit_(WAS_HIT)
        , hitType_(HitType::Spell)
        , weaponPtrOpt_()
        , damage_(DAMAGE)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_("casts")
        , spellPtrOpt_(SPELL_PTR)
        , actionPhraseCNP_(ACTION_PHRASE_CNP)
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(
        const bool WAS_HIT,
        const song::SongPtr_t SONG_PTR,
        const ContentAndNamePos & ACTION_PHRASE_CNP,
        const Health_t & DAMAGE,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
        : wasHit_(WAS_HIT)
        , hitType_(HitType::Song)
        , weaponPtrOpt_()
        , damage_(DAMAGE)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_("plays")
        , spellPtrOpt_()
        , actionPhraseCNP_(ACTION_PHRASE_CNP)
        , songPtrOpt_(SONG_PTR)
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(
        const bool WAS_HIT,
        const creature::Conditions::Enum COND_ENUM,
        const ContentAndNamePos & ACTION_PHRASE_CNP,
        const Health_t & DAMAGE,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
        : wasHit_(WAS_HIT)
        , hitType_(HitType::Song)
        , weaponPtrOpt_()
        , damage_(DAMAGE)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_("plays")
        , spellPtrOpt_()
        , actionPhraseCNP_(ACTION_PHRASE_CNP)
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_(creature::condition::Holder::Get(COND_ENUM))
    {}

    HitInfo::HitInfo(
        const bool WAS_HIT,
        const HitType::Enum HIT_TYPE,
        const ContentAndNamePos & ACTION_PHRASE_CNP,
        const Health_t & DAMAGE,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
        : wasHit_(WAS_HIT)
        , hitType_(HIT_TYPE)
        , weaponPtrOpt_()
        , damage_(DAMAGE)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_("")
        , spellPtrOpt_()
        , actionPhraseCNP_(ACTION_PHRASE_CNP)
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(const HitType::Enum HIT_TYPE, const ContentAndNamePos & ACTION_PHRASE_CNP)
        : wasHit_(false)
        , hitType_(HIT_TYPE)
        , weaponPtrOpt_()
        , damage_(0)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_()
        , condsRemovedVec_()
        , actionVerb_("")
        , spellPtrOpt_()
        , actionPhraseCNP_(ACTION_PHRASE_CNP)
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    HitInfo::HitInfo(
        const Health_t & DAMAGE,
        const std::string & ACTION_VERB,
        const creature::CondEnumVec_t & CONDS_ADDED_VEC,
        const creature::CondEnumVec_t & CONDS_REMOVED_VEC)
        : wasHit_(true)
        , hitType_(HitType::Trap)
        , weaponPtrOpt_()
        , damage_(DAMAGE)
        , isCritical_(false)
        , isPower_(false)
        , condsAddedVec_(CONDS_ADDED_VEC)
        , condsRemovedVec_(CONDS_REMOVED_VEC)
        , actionVerb_(ACTION_VERB)
        , spellPtrOpt_()
        , actionPhraseCNP_()
        , songPtrOpt_()
        , didArmorAbsorb_(false)
        , conditionPtrOpt_()
    {}

    bool HitInfo::CondsAddedContains(const creature::Conditions::Enum ENUM) const
    {
        return (
            std::find(condsAddedVec_.begin(), condsAddedVec_.end(), ENUM) != condsAddedVec_.end());
    }

    bool HitInfo::CondsAddedRemove(const creature::Conditions::Enum ENUM)
    {
        creature::CondEnumVec_t condsToRemoveVec;

        for (const auto NEXT_CONDITION_ENUM : condsAddedVec_)
        {
            if (NEXT_CONDITION_ENUM == ENUM)
            {
                condsToRemoveVec.emplace_back(NEXT_CONDITION_ENUM);
            }
        }

        if (condsToRemoveVec.empty())
        {
            return false;
        }

        for (const auto NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            condsAddedVec_.erase(
                std::remove(
                    condsAddedVec_.begin(), condsAddedVec_.end(), NEXT_CONDITION_TO_REMOVE_ENUM),
                condsAddedVec_.end());
        }

        return true;
    }

    bool HitInfo::CondsRemovedContains(const creature::Conditions::Enum ENUM) const
    {
        return (
            std::find(condsRemovedVec_.begin(), condsRemovedVec_.end(), ENUM)
            != condsRemovedVec_.end());
    }

    bool HitInfo::CondsRemovedRemove(const creature::Conditions::Enum ENUM)
    {
        creature::CondEnumVec_t condsToRemoveVec;

        for (const auto NEXT_CONDITION_ENUM : condsRemovedVec_)
        {
            if (NEXT_CONDITION_ENUM == ENUM)
            {
                condsToRemoveVec.emplace_back(NEXT_CONDITION_ENUM);
            }
        }

        if (condsToRemoveVec.empty())
        {
            return false;
        }

        for (const auto NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            condsRemovedVec_.erase(
                std::remove(
                    condsRemovedVec_.begin(),
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
                return !!weaponPtrOpt_;
            }
            case HitType::Spell:
            {
                return (spellPtrOpt_ && (actionPhraseCNP_.NamePos() != NamePosition::Count));
            }
            case HitType::Song:
            {
                return (songPtrOpt_ && (actionPhraseCNP_.NamePos() != NamePosition::Count));
            }
            case HitType::Pounce:
            case HitType::Roar:
            {
                return (actionPhraseCNP_.NamePos() != NamePosition::Count);
            }
            case HitType::Condition:
            {
                return (conditionPtrOpt_ && (actionPhraseCNP_.NamePos() != NamePosition::Count));
            }
            case HitType::Trap:
            {
                return ((actionVerb_.empty() == false) && damage_.IsNonZero());
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
        if (std::tie(
                hitType_,
                wasHit_,
                weaponPtrOpt_,
                damage_,
                isCritical_,
                isPower_,
                spellPtrOpt_,
                songPtrOpt_,
                didArmorAbsorb_,
                conditionPtrOpt_)
            != std::tie(
                HI.hitType_,
                HI.wasHit_,
                HI.weaponPtrOpt_,
                HI.damage_,
                HI.isCritical_,
                HI.isPower_,
                HI.spellPtrOpt_,
                HI.songPtrOpt_,
                HI.didArmorAbsorb_,
                HI.conditionPtrOpt_))
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(condsAddedVec_, HI.condsAddedVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(condsRemovedVec_, HI.condsRemovedVec_) == false)
        {
            return false;
        }

        return true;
    }

    const std::string HitInfo::ToString() const
    {
        if (IsValid() == false)
        {
            return "{HitInfo is Invalid}";
        }

        std::ostringstream ss;

        ss << "{" << HitType::ToString(hitType_);

        switch (hitType_)
        {
            case HitType::Weapon:
            {
                ss << "[" << weaponPtrOpt_.value()->Name() << "]";
                break;
            }
            case HitType::Spell:
            {
                ss << "[" << spellPtrOpt_.value()->Name() << "]";
                break;
            }
            case HitType::Song:
            {
                ss << "[" << songPtrOpt_.value()->Name() << "]";
                break;
            }
            case HitType::Condition:
            {
                ss << "[" << conditionPtrOpt_.value()->Name() << "]";
                break;
            }
            case HitType::Pounce:
            case HitType::Roar:
            case HitType::Trap:
            case HitType::Count:
            default:
            {
                break;
            }
        }

        ss << ", was_hit=" << std::boolalpha << WasHit() << ", damage=" << damage_
           << ", was_kill=" << WasKill() << ", is_critical=" << isCritical_
           << ", is_power=" << isPower_ << ", did_armor_absorb=" << didArmorAbsorb_
           << ", conds_added=[";

        for (const auto NEXT_COND_ENUM : condsAddedVec_)
        {
            ss << creature::Conditions::ToString(NEXT_COND_ENUM) << ",";
        }

        ss << "], conds_removed=[";

        for (const auto NEXT_COND_ENUM : condsRemovedVec_)
        {
            ss << creature::Conditions::ToString(NEXT_COND_ENUM) << ",";
        }

        ss << "]}";

        return ss.str();
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

        return std::tie(
                   L.hitType_,
                   L.wasHit_,
                   L.weaponPtrOpt_,
                   L.damage_,
                   L.isCritical_,
                   L.isPower_,
                   L.actionVerb_,
                   L.spellPtrOpt_,
                   L.actionPhraseCNP_,
                   L.songPtrOpt_,
                   L.didArmorAbsorb_,
                   L.conditionPtrOpt_)
            < std::tie(
                   R.hitType_,
                   R.wasHit_,
                   R.weaponPtrOpt_,
                   R.damage_,
                   R.isCritical_,
                   R.isPower_,
                   R.actionVerb_,
                   R.spellPtrOpt_,
                   R.actionPhraseCNP_,
                   R.songPtrOpt_,
                   R.didArmorAbsorb_,
                   R.conditionPtrOpt_);
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

        return std::tie(
                   L.hitType_,
                   L.wasHit_,
                   L.weaponPtrOpt_,
                   L.damage_,
                   L.isCritical_,
                   L.isPower_,
                   L.actionVerb_,
                   L.spellPtrOpt_,
                   L.actionPhraseCNP_,
                   L.songPtrOpt_,
                   L.didArmorAbsorb_,
                   L.conditionPtrOpt_)
            == std::tie(
                   R.hitType_,
                   R.wasHit_,
                   R.weaponPtrOpt_,
                   R.damage_,
                   R.isCritical_,
                   R.isPower_,
                   R.actionVerb_,
                   R.spellPtrOpt_,
                   R.actionPhraseCNP_,
                   R.songPtrOpt_,
                   R.didArmorAbsorb_,
                   R.conditionPtrOpt_);
    }

} // namespace combat
} // namespace heroespath
