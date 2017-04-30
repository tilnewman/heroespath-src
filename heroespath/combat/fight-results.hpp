#ifndef HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
#define HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
//
// fight-results.hpp
//
#include "heroespath/stats/types.hpp"
#include "heroespath/creature/condition-enum.hpp"

#include <string>
#include <memory>
#include <vector>
#include <tuple>


namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace creature
{
    class Condition;
    using ConditionSPtr_t = std::shared_ptr<Condition>;
    using ConditionSVec_t = std::vector<ConditionSPtr_t>;

    class Creature;
    using CreaturePtr_t  = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace combat
{

    struct DodgeType
    {
        enum Enum
        {
            Speed = 0,
            AmazingSpeed,
            Luck,
            AmazingLuck,
            Count
        };

        static const std::string ToString(const Enum);
    };


    struct HitType
    {
        enum Enum
        {
            Accuracy = 0,
            AmazingAccuracy,
            Luck,
            AmazingLuck,
            Spell,
            Count
        };

        static const std::string ToString(const Enum);
    };


    //Everything required to describe a weapon making contact with a creature.
    //Note:  It is possible to hit with no damage.
    class HitInfo
    {
    public:
        HitInfo(const item::ItemSPtr_t &          ITEM_SPTR       = item::ItemSPtr_t(),
                const HitType::Enum               HIT_TYPE        = HitType::Count,
                const DodgeType::Enum             DODGE_TYPE      = DodgeType::Count,
                const stats::Health_t             DAMAGE          = 0,
                const bool                        IS_CRITICAL_HIT = false,
                const bool                        IS_POWER_HIT    = false,
                const creature::ConditionSVec_t & CONDITIONS_SVEC = creature::ConditionSVec_t(),
                const std::string &               ACTION_VERB     = "");

        inline bool WasHit() const                  { return hitType_ != HitType::Count; }
        inline HitType::Enum HitKind() const        { return hitType_; }
        inline DodgeType::Enum DodgeKind() const    { return dodgeType_; }
        inline item::ItemSPtr_t Weapon() const      { return weaponSPtr_; }
        inline stats::Health_t Damage() const       { return damage_; }
        inline bool IsCritical() const              { return isCritical_; }
        inline bool IsPowerHit() const              { return isPower_; }
        inline const std::string ActionVerb() const { return actionVerb_; }

        inline const creature::ConditionSVec_t Conditions() const { return conditionsSVec_; }

        bool ContainsCondition(const creature::condition::Enum) const;
        bool RemoveCondition(const creature::condition::Enum);

        inline bool WasKill() const                 { return ContainsCondition(creature::condition::Dead); }

        friend bool operator<(const HitInfo & L, const HitInfo & R);
        friend bool operator==(const HitInfo & L, const HitInfo & R);

    private:
        HitType::Enum hitType_;
        DodgeType::Enum dodgeType_;
        item::ItemSPtr_t weaponSPtr_;
        stats::Health_t damage_;
        bool isCritical_;
        bool isPower_;
        creature::ConditionSVec_t conditionsSVec_;
        std::string actionVerb_;
    };

    using HitInfoVec_t = std::vector<HitInfo>;

    bool operator<(const HitInfo & L, const HitInfo & R);

    bool operator==(const HitInfo & L, const HitInfo & R);

    inline bool operator!=(const HitInfo & L, const HitInfo & R)
    {
        return ! (L == R);
    }


    //Everything required to describe all the effects a fight had on a creature.
    class CreatureEffect
    {
    public:
        CreatureEffect(const creature::CreaturePtr_t     CREATURE_PTR    = nullptr,
                       const HitInfoVec_t &              HIT_INFO_VEC    = HitInfoVec_t(),
                       const spell::SpellPtr_t           SPELL_PTR       = nullptr,
                       const creature::ConditionSVec_t & CONDITIONS_SVEC = creature::ConditionSVec_t(),
                       const bool                        WAS_POUNCED     = false);

        CreatureEffect(const CreatureEffect &);
        CreatureEffect operator=(const CreatureEffect &);

        inline std::size_t               GetCount() const           { return hitInfoVec_.size(); }
        inline HitInfoVec_t              GetHitInfoVec() const      { return hitInfoVec_; }
        inline spell::SpellPtr_t         GetSpell() const           { return spellPtr_; }
        inline creature::CreaturePtr_t   GetCreature() const        { return creaturePtr_; }
        inline bool                      GetWasPounced() const      { return wasPounced_; }

        bool GetWasHit() const;
        stats::Health_t GetDamageTotal() const;

        const creature::ConditionSVec_t  GetHitConditions() const;
        inline creature::ConditionSVec_t GetNonHitConditions() const{ return conditionsSVec_; }
        const creature::ConditionSVec_t  GetAllConditions() const;
        bool                             ContainsCondition(const creature::condition::Enum) const;
        bool                             RemoveCondition(const creature::condition::Enum);

        inline bool WasKill() const { return ContainsCondition(creature::condition::Dead); }

        friend bool operator<(const CreatureEffect & L, const CreatureEffect & R);
        friend bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    private:
        HitInfoVec_t hitInfoVec_;
        spell::SpellPtr_t spellPtr_;
        creature::CreaturePtr_t creaturePtr_;
        creature::ConditionSVec_t conditionsSVec_;
        bool wasPounced_;
    };

    using CreatureEffectVec_t = std::vector<CreatureEffect>;

    bool operator<(const CreatureEffect & L, const CreatureEffect & R);

    bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    inline bool operator!=(const CreatureEffect & L, const CreatureEffect & R)
    {
        return ! (L == R);
    }


    //Everything required to describe the results of a fight or fight action.
    class FightResult
    {
    public:
        FightResult(const CreatureEffectVec_t & CREATURE_EFFECT_VEC = CreatureEffectVec_t());
        FightResult(const CreatureEffect & CREATURE_EFFECT);

        inline CreatureEffectVec_t Effects() const  { return creatureEffectVec_; }
        inline std::size_t Count() const            { return creatureEffectVec_.size(); }

        CreatureEffect FirstEffect() const;
        creature::CreaturePtr_t FirstCreature() const;

        stats::Health_t DamageTotal() const;

        bool WasHit() const;

        std::size_t HitCount() const;

        const creature::ConditionSVec_t Conditions(const bool WILL_UNIUQE) const;

        bool GetHitInfo(HitInfo &         HitInfo_OutParam,
                        const std::size_t EFFECT_INDEX = 0,
                        const std::size_t HIT_INDEX = 0) const;

        std::size_t EffectedCreatures(creature::CreaturePVec_t &) const;

    private:
        CreatureEffectVec_t creatureEffectVec_;
    };

}
}

#endif //HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
