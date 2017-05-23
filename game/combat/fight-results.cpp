//
// fight-results.cpp
//
#include "fight-results.hpp"

#include "game/item/item.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/creature.hpp"

#include "utilz/vectors.hpp"

#include <sstream>
#include <exception>
#include <algorithm>


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


    HitInfo::HitInfo(const item::ItemSPtr_t &             ITEM_SPTR,
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
        weaponSPtr_    (ITEM_SPTR),
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
        weaponSPtr_    (),
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
        weaponSPtr_   (HI.weaponSPtr_),
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
            weaponSPtr_     = HI.weaponSPtr_;
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
            if (NEXT_CONDITION_ENUM == E)
                return true;

        return false;
    }


    bool HitInfo::RemoveCondition(const creature::Conditions::Enum E)
    {
        creature::ConditionEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
            if (NEXT_CONDITION_ENUM == E)
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);

        auto const WILL_REMOVE_A_CONDITION{ ! condsToRemoveVec.empty() };

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            conditionsVec_.erase(std::remove(conditionsVec_.begin(), conditionsVec_.end(), NEXT_CONDITION_TO_REMOVE_ENUM), conditionsVec_.end());
        }

        return WILL_REMOVE_A_CONDITION;
    }


    bool operator<(const HitInfo & L, const HitInfo & R)
    {
        auto lConditionsVec{ L.conditionsVec_ };
        auto rConditionsVec{ R.conditionsVec_ };
        if (lConditionsVec.empty() == false) std::sort(lConditionsVec.begin(), lConditionsVec.end());
        if (rConditionsVec.empty() == false) std::sort(rConditionsVec.begin(), rConditionsVec.end());

        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponSPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.spellEffectStr_,
                        lConditionsVec)
               <
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponSPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.spellEffectStr_,
                        rConditionsVec);
    }


    bool operator==(const HitInfo & L, const HitInfo & R)
    {
        auto lConditionsVec{ L.conditionsVec_ };
        auto rConditionsVec{ R.conditionsVec_ };
        if (lConditionsVec.empty() == false) std::sort(lConditionsVec.begin(), lConditionsVec.end());
        if (rConditionsVec.empty() == false) std::sort(rConditionsVec.begin(), rConditionsVec.end());

        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponSPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.actionVerb_,
                        L.spellPtr_,
                        L.spellEffectStr_,
                        lConditionsVec)
               ==
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponSPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.actionVerb_,
                        R.spellPtr_,
                        R.spellEffectStr_,
                        rConditionsVec);
    }


    CreatureEffect::CreatureEffect(const creature::CreaturePtr_t        CREATURE_PTR,
                                   const HitInfoVec_t &                 HIT_INFO_VEC,
                                   const spell::SpellPtr_t              SPELL_PTR,
                                   const creature::ConditionEnumVec_t & CONDITIONS_VEC,
                                   const bool                           WAS_POUNCED)
    :
        hitInfoVec_   (HIT_INFO_VEC),
        spellPtr_     (SPELL_PTR),
        creaturePtr_  (CREATURE_PTR),
        conditionsVec_(CONDITIONS_VEC),
        wasPounced_   (WAS_POUNCED)
    {}


    CreatureEffect::CreatureEffect(const CreatureEffect & CE)
    :
        hitInfoVec_   (CE.hitInfoVec_),

        //The lifetime of these objects is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        spellPtr_     (CE.spellPtr_),
        creaturePtr_  (CE.creaturePtr_),

        conditionsVec_(CE.conditionsVec_),
        wasPounced_   (CE.wasPounced_)
    {}


    CreatureEffect & CreatureEffect::operator=(const CreatureEffect & CE)
    {
        if (& CE != this)
        {
            hitInfoVec_ = CE.hitInfoVec_;
            conditionsVec_ = CE.conditionsVec_;
            wasPounced_ = CE.wasPounced_;

            //see copy constructor comment regarding these pointers
            spellPtr_ = CE.spellPtr_;
            creaturePtr_ = CE.creaturePtr_;
        }

        return * this;
    }


    bool CreatureEffect::GetWasHit() const
    {
        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
            if (NEXT_HIT_INFO.WasHit())
                return true;

        return false;
    }


    stats::Health_t CreatureEffect::GetDamageTotal() const
    {
        stats::Health_t damageTotal{ 0 };

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
            damageTotal += NEXT_HIT_INFO.Damage();

        return damageTotal;
    }


    const creature::ConditionEnumVec_t CreatureEffect::GetHitConditions() const
    {
        creature::ConditionEnumVec_t conditionsVec;

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC{ NEXT_HIT_INFO.Conditions() };
            for (auto const NEXT_CONDITION_ENUM : NEXT_CONDITION_VEC)
            {
                conditionsVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        return conditionsVec;
    }


    const creature::ConditionEnumVec_t CreatureEffect::GetAllConditions() const
    {
        creature::ConditionEnumVec_t allConditionsVec;

        auto const HIT_CONDITIONS_VEC{ GetHitConditions() };

        if (HIT_CONDITIONS_VEC.empty() == false)
            std::copy(HIT_CONDITIONS_VEC.begin(), HIT_CONDITIONS_VEC.end(), back_inserter(allConditionsVec));

        auto const NON_HIT_CONDITIONS_VEC{ GetNonHitConditions() };

        if (NON_HIT_CONDITIONS_VEC.empty() == false)
            std::copy(NON_HIT_CONDITIONS_VEC.begin(), NON_HIT_CONDITIONS_VEC.end(), back_inserter(allConditionsVec));

        return allConditionsVec;
    }


    bool CreatureEffect::ContainsCondition(const creature::Conditions::Enum E) const
    {
        const auto CONDITIONS_VEC{ GetAllConditions() };

        for (auto const NEXT_CONDITION_ENUM : CONDITIONS_VEC)
            if (NEXT_CONDITION_ENUM == E)
                return true;

        return false;
    }


    bool CreatureEffect::RemoveCondition(const creature::Conditions::Enum E)
    {
        auto wasCondRemoved{ false };

        for (auto & nextHitInfo : hitInfoVec_)
            if (nextHitInfo.RemoveCondition(E))
                wasCondRemoved = true;

        creature::ConditionEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
            if (NEXT_CONDITION_ENUM == E)
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);

        if (condsToRemoveVec.empty() == false)
        {
            wasCondRemoved = true;
        }

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            conditionsVec_.erase(std::remove(conditionsVec_.begin(), conditionsVec_.end(), NEXT_CONDITION_TO_REMOVE_ENUM), conditionsVec_.end());
        }

        return wasCondRemoved;
    }


    bool operator<(const CreatureEffect & L, const CreatureEffect & R)
    {
        auto lConditionsVec{ L.conditionsVec_ };
        auto rConditionsVec{ R.conditionsVec_ };
        if (lConditionsVec.empty() == false) std::sort(lConditionsVec.begin(), lConditionsVec.end());
        if (rConditionsVec.empty() == false) std::sort(rConditionsVec.begin(), rConditionsVec.end());

        return std::tie(L.hitInfoVec_,
                        L.spellPtr_,
                        L.creaturePtr_,
                        L.wasPounced_,
                        lConditionsVec)
               <
               std::tie(R.hitInfoVec_,
                        R.spellPtr_,
                        R.creaturePtr_,
                        R.wasPounced_,
                        rConditionsVec);
    }


    bool operator==(const CreatureEffect & L, const CreatureEffect & R)
    {
        auto lConditionsVec{ L.conditionsVec_ };
        auto rConditionsVec{ R.conditionsVec_ };
        if (lConditionsVec.empty() == false) std::sort(lConditionsVec.begin(), lConditionsVec.end());
        if (rConditionsVec.empty() == false) std::sort(rConditionsVec.begin(), rConditionsVec.end());

        return std::tie(L.hitInfoVec_,
                        L.spellPtr_,
                        L.creaturePtr_,
                        L.wasPounced_,
                        lConditionsVec)
               ==
               std::tie(R.hitInfoVec_,
                        R.spellPtr_,
                        R.creaturePtr_,
                        R.wasPounced_,
                        rConditionsVec);
    }


    FightResult::FightResult(const CreatureEffectVec_t & CREATURE_EFFECT_VEC)
    :
        creatureEffectVec_(CREATURE_EFFECT_VEC)
    {}


    FightResult::FightResult(const CreatureEffect & CREATURE_EFFECT)
    :
        creatureEffectVec_(1, CREATURE_EFFECT)
    {}


    CreatureEffect FightResult::FirstEffect() const
    {
        if (creatureEffectVec_.empty())
        {
            std::ostringstream ss;
            ss << "game::combat::FightResult::FirstEffect() called when the creatureEffectVec_ is empty.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return creatureEffectVec_[0];
        }
    }


    creature::CreaturePtr_t FightResult::FirstCreature() const
    {
        if (creatureEffectVec_.empty())
        {
            std::ostringstream ss;
            ss << "game::combat::FightResult::FirstCreature() called when the creatureEffectVec_ is empty.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return creatureEffectVec_[0].GetCreature();
        }
    }


    stats::Health_t FightResult::DamageTotal() const
    {
        stats::Health_t damageTotal{ 0 };

        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
            damageTotal += NEXT_CREATURE_EFFECT.GetDamageTotal();

        return damageTotal;
    }


    bool FightResult::WasHit() const
    {
        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
            if (NEXT_CREATURE_EFFECT.GetWasHit())
                return true;

        return false;
    }


    std::size_t FightResult::HitCount() const
    {
        std::size_t counter{ 0 };

        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
        {
            auto const HIT_INFO_VEC{ NEXT_CREATURE_EFFECT.GetHitInfoVec() };
            for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            {
                if (NEXT_HIT_INFO.WasHit())
                    ++counter;
            }
        }

        return counter;
    }


    const creature::ConditionEnumVec_t FightResult::Conditions(const bool WILL_UNIUQE) const
    {
        creature::ConditionEnumVec_t conditionsVec;

        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
        {
            utilz::Vector::Append(NEXT_CREATURE_EFFECT.GetAllConditions(), conditionsVec);
        }

        if (WILL_UNIUQE)
        {
            conditionsVec.erase(std::unique(conditionsVec.begin(), conditionsVec.end()), conditionsVec.end());
        }

        return conditionsVec;
    }


    bool FightResult::GetHitInfo(HitInfo &         HitInfo_OutParam,
                                 const std::size_t EFFECT_INDEX,
                                 const std::size_t HIT_INDEX) const
    {
        if (EFFECT_INDEX < creatureEffectVec_.size())
        {
            auto const HIT_INFO_VEC{ creatureEffectVec_[EFFECT_INDEX].GetHitInfoVec() };
            if (HIT_INDEX < HIT_INFO_VEC.size())
            {
                HitInfo_OutParam = HIT_INFO_VEC[HIT_INDEX];
                return true;
            }
        }

        return false;
    }


    std::size_t FightResult::EffectedCreatures(creature::CreaturePVec_t & CreaturePVec_OutParam) const
    {
        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
            CreaturePVec_OutParam.push_back(NEXT_CREATURE_EFFECT.GetCreature());

        return creatureEffectVec_.size();
    }

}
}
