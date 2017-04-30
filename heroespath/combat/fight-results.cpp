//
// fight-results.cpp
//
#include "fight-results.hpp"

#include "heroespath/item/item.hpp"
#include "heroespath/creature/condition.hpp"
#include "heroespath/creature/condition-algorithms.hpp"
#include "heroespath/creature/creature.hpp"

#include <sstream>
#include <exception>
#include <algorithm>


namespace heroespath
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
                ss << "heroespath::combat::DodgeType::ToString(" << E << ")_InvalidValueError.";
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
                ss << "heroespath::combat::HitType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    HitInfo::HitInfo(const item::ItemSPtr_t &          ITEM_SPTR,
                     const HitType::Enum               HIT_TYPE,
                     const DodgeType::Enum             DODGE_TYPE,
                     const stats::Health_t             DAMAGE,
                     const bool                        IS_CRITICAL_HIT,
                     const bool                        IS_POWER_HIT,
                     const creature::ConditionSVec_t & CONDITIONS_SVEC,
                     const std::string &               ACTION_VERB)
    :
        hitType_       (HIT_TYPE),
        dodgeType_     (DODGE_TYPE),
        weaponSPtr_    (ITEM_SPTR),
        damage_        (DAMAGE),
        isCritical_    (IS_CRITICAL_HIT),
        isPower_       (IS_POWER_HIT),
        conditionsSVec_(CONDITIONS_SVEC),
        actionVerb_    (ACTION_VERB)
    {}


    bool HitInfo::ContainsCondition(const creature::condition::Enum E) const
    {
        for (auto const NEXT_CONDITION_PTR : conditionsSVec_)
            if (NEXT_CONDITION_PTR->Which() == E)
                return true;

        return false;
    }


    bool HitInfo::RemoveCondition(const creature::condition::Enum E)
    {
        creature::ConditionSVec_t condsToRemoveSVec;

        for (auto const & NEXT_CONDITION_SPTR : conditionsSVec_)
            if (NEXT_CONDITION_SPTR->Which() == E)
                condsToRemoveSVec.push_back(NEXT_CONDITION_SPTR);

        auto const WILL_REMOVE_A_CONDITION{ ! condsToRemoveSVec.empty() };

        for (auto const & NEXT_CONDITION_TO_REMOVE_SPTR : condsToRemoveSVec)
        {
            conditionsSVec_.erase(std::remove(conditionsSVec_.begin(), conditionsSVec_.end(), NEXT_CONDITION_TO_REMOVE_SPTR), conditionsSVec_.end());
        }

        return WILL_REMOVE_A_CONDITION;
    }


    bool operator<(const HitInfo & L, const HitInfo & R)
    {
        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponSPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.conditionsSVec_,
                        L.actionVerb_)
               <
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponSPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.conditionsSVec_,
                        R.actionVerb_);
    }


    bool operator==(const HitInfo & L, const HitInfo & R)
    {
        return std::tie(L.hitType_,
                        L.dodgeType_,
                        L.weaponSPtr_,
                        L.damage_,
                        L.isCritical_,
                        L.isPower_,
                        L.conditionsSVec_,
                        L.actionVerb_)
               ==
               std::tie(R.hitType_,
                        R.dodgeType_,
                        R.weaponSPtr_,
                        R.damage_,
                        R.isCritical_,
                        R.isPower_,
                        R.conditionsSVec_,
                        R.actionVerb_);
    }


    CreatureEffect::CreatureEffect(const creature::CreaturePtr_t     CREATURE_PTR,
                                   const HitInfoVec_t &              HIT_INFO_VEC,
                                   const spell::SpellPtr_t           SPELL_PTR,
                                   const creature::ConditionSVec_t & CONDITIONS_SVEC,
                                   const bool                        WAS_POUNCED)
    :
        hitInfoVec_    (HIT_INFO_VEC),
        spellPtr_      (SPELL_PTR),
        creaturePtr_   (CREATURE_PTR),
        conditionsSVec_(CONDITIONS_SVEC),
        wasPounced_    (WAS_POUNCED)
    {}


    CreatureEffect::CreatureEffect(const CreatureEffect & CE)
    :
        hitInfoVec_    (CE.hitInfoVec_),

        //The lifetimes of these two objects are not managed by this class.
        //These pointers were originally provided from an owning
        //std::shared_ptr, which is not effected by the copies made here.
        //So copying these pointers is safe.
        spellPtr_      (CE.spellPtr_),
        creaturePtr_   (CE.creaturePtr_),

        conditionsSVec_(CE.conditionsSVec_),
        wasPounced_    (CE.wasPounced_)
    {}


    CreatureEffect & CreatureEffect::operator=(const CreatureEffect & CE)
    {
        if (& CE != this)
        {
            hitInfoVec_ = CE.hitInfoVec_;
            conditionsSVec_ = CE.conditionsSVec_;
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


    const creature::ConditionSVec_t CreatureEffect::GetHitConditions() const
    {
        creature::ConditionSVec_t conditionsSVec;

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_SVEC{ NEXT_HIT_INFO.Conditions() };
            for (auto const & NEXT_CONDITION_SPTR : NEXT_CONDITION_SVEC)
            {
                conditionsSVec.push_back(NEXT_CONDITION_SPTR);
            }
        }

        return conditionsSVec;
    }


    const creature::ConditionSVec_t CreatureEffect::GetAllConditions() const
    {
        creature::ConditionSVec_t allConditionsSVec;

        auto const HIT_CONDITIONS_SVEC{ GetHitConditions() };

        if (HIT_CONDITIONS_SVEC.empty() == false)
            std::copy(HIT_CONDITIONS_SVEC.begin(), HIT_CONDITIONS_SVEC.end(), back_inserter(allConditionsSVec));

        auto const NON_HIT_CONDITIONS_SVEC{ GetNonHitConditions() };

        if (NON_HIT_CONDITIONS_SVEC.empty() == false)
            std::copy(NON_HIT_CONDITIONS_SVEC.begin(), NON_HIT_CONDITIONS_SVEC.end(), back_inserter(allConditionsSVec));

        return allConditionsSVec;
    }


    bool CreatureEffect::ContainsCondition(const creature::condition::Enum E) const
    {
        const auto CONDITIONS_PVEC{ GetAllConditions() };

        for (auto const NEXT_CONDITION_PTR : CONDITIONS_PVEC)
            if (NEXT_CONDITION_PTR->Which() == E)
                return true;

        return false;
    }


    bool CreatureEffect::RemoveCondition(const creature::condition::Enum E)
    {
        auto wasCondRemoved{ false };

        for (auto & nextHitInfo : hitInfoVec_)
            if (nextHitInfo.RemoveCondition(E))
                wasCondRemoved = true;

        creature::ConditionSVec_t condsToRemoveSVec;

        for (auto const & NEXT_CONDITION_SPTR : conditionsSVec_)
            if (NEXT_CONDITION_SPTR->Which() == E)
                condsToRemoveSVec.push_back(NEXT_CONDITION_SPTR);

        if (condsToRemoveSVec.empty() == false)
        {
            wasCondRemoved = true;
        }

        for (auto const & NEXT_CONDITION_TO_REMOVE_SPTR : condsToRemoveSVec)
        {
            conditionsSVec_.erase(std::remove(conditionsSVec_.begin(), conditionsSVec_.end(), NEXT_CONDITION_TO_REMOVE_SPTR), conditionsSVec_.end());
        }

        return wasCondRemoved;
    }


    bool operator<(const CreatureEffect & L, const CreatureEffect & R)
    {
        return std::tie(L.hitInfoVec_, L.spellPtr_, L.creaturePtr_, L.conditionsSVec_, L.wasPounced_)
               <
               std::tie(R.hitInfoVec_, R.spellPtr_, R.creaturePtr_, R.conditionsSVec_, R.wasPounced_);
    }


    bool operator==(const CreatureEffect & L, const CreatureEffect & R)
    {
        return std::tie(L.hitInfoVec_, L.spellPtr_, L.creaturePtr_, L.conditionsSVec_, L.wasPounced_)
               ==
               std::tie(R.hitInfoVec_, R.spellPtr_, R.creaturePtr_, R.conditionsSVec_, R.wasPounced_);
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
            ss << "heroespath::combat::FightResult::FirstEffect() called when the creatureEffectVec_ is empty.";
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
            ss << "heroespath::combat::FightResult::FirstCreature() called when the creatureEffectVec_ is empty.";
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


    const creature::ConditionSVec_t FightResult::Conditions(const bool WILL_UNIUQE) const
    {
        creature::ConditionSVec_t conditionsSVec;

        for (auto const & NEXT_CREATURE_EFFECT : creatureEffectVec_)
        {
            creature::condition::Algorithms::Append(NEXT_CREATURE_EFFECT.GetAllConditions(), conditionsSVec);
        }

        if (WILL_UNIUQE)
        {
            std::unique(conditionsSVec.begin(), conditionsSVec.end());
        }

        return conditionsSVec;
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
