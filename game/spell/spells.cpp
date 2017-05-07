//
// spells.cpp
//
#include "spells.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/conditions.hpp"


namespace game
{
namespace spell
{

    const std::string Sparks::ActionPhrase(creature::CreaturePtr_t castingCreaturePtr,
                                           creature::CreaturePtr_t effectedCreaturePtr) const
    {
        auto const RANDOM_VALUE{ utilz::random::Int(3) };
        if      (RANDOM_VALUE == 0) return castingCreaturePtr->NameOrRaceAndClass() + " shoots sparks at " + effectedCreaturePtr->NameOrRaceAndClass() + ".";
        else if (RANDOM_VALUE == 1) return effectedCreaturePtr->NameOrRaceAndClass() + " is sprayed with sparks.";
        else if (RANDOM_VALUE == 2) return "A sputter of sparks hits " + effectedCreaturePtr->NameOrRaceAndClass() + ".";
        else                        return "Sparks shower upon " + effectedCreaturePtr->NameOrRaceAndClass() + ".";
    }


    stats::Health_t Sparks::Health(creature::CreaturePtr_t castingCreaturePtr,
                                   creature::CreaturePtr_t) const
    {
        return GenerateHealthValue((castingCreaturePtr->Stats().Int().Current() / 5), 6, castingCreaturePtr->Rank(), 10);
    }


    const std::string Bandage::ActionPhrase(creature::CreaturePtr_t castingCreaturePtr,
                                            creature::CreaturePtr_t effectedCreaturePtr) const
    {
        auto const RANDOM_VALUE{ utilz::random::Int(3) };
        if      (RANDOM_VALUE == 0) return castingCreaturePtr->NameOrRaceAndClass() + " magically bandages " + effectedCreaturePtr->NameOrRaceAndClass() + ".";
        else if (RANDOM_VALUE == 1) return castingCreaturePtr->NameOrRaceAndClass() + " magically bandages " + effectedCreaturePtr->NameOrRaceAndClass() + "'s wounds.";
        else if (RANDOM_VALUE == 2) return "Magical bandages wrap " + effectedCreaturePtr->NameOrRaceAndClass() + "'s injuries.";
        else                        return effectedCreaturePtr->NameOrRaceAndClass() + "'s wounds are magically bandaged.";
    }


    stats::Health_t Bandage::Health(creature::CreaturePtr_t castingCreaturePtr,
                                    creature::CreaturePtr_t) const
    {
        return GenerateHealthValue((castingCreaturePtr->Stats().Int().Current() / 5), 6, castingCreaturePtr->Rank(), 10);
    }


    const std::string Sleep::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::AsleepNatural) ||
            effectedCreaturePtr->HasCondition(creature::condition::AsleepMagical))
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is already asleep";
        }
        else
        {
            effectedCreaturePtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::AsleepNatural));
            return Spell::EFFECT_STR_SUCCESS_;
        }
    }


    const std::string Awaken::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::AsleepNatural))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::AsleepNatural);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else if (effectedCreaturePtr->HasCondition(creature::condition::AsleepMagical))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::AsleepMagical);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else if (effectedCreaturePtr->HasCondition(creature::condition::Unconscious))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::Unconscious);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is not asleep or unconscious";
        }
    }


    const std::string Trip::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Tripped))
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is already tripped";
        }
        else
        {
            effectedCreaturePtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Tripped));
            return Spell::EFFECT_STR_SUCCESS_;
        }
    }


    const std::string Lift::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Tripped))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::Tripped);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is not tripped";
        }
    }


    const std::string Daze::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Dazed))
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is already dazed";
        }
        else
        {
            effectedCreaturePtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Dazed));
            return Spell::EFFECT_STR_SUCCESS_;
        }
    }


    const std::string Frighten::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Frightened))
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is already frightened";
        }
        else
        {
            effectedCreaturePtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Frightened));
            return Spell::EFFECT_STR_SUCCESS_;
        }
    }


    const std::string ClearMind::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Frightened))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::Frightened);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else if (effectedCreaturePtr->HasCondition(creature::condition::Dazed))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::Dazed);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is not frightened or dazed";
        }
    }


    const std::string Poison::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Poisoned))
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is already poisoned";
        }
        else
        {
            effectedCreaturePtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Poisoned));
            return Spell::EFFECT_STR_SUCCESS_;
        }
    }


    const std::string Antidote::EffectCreature(creature::CreaturePtr_t, creature::CreaturePtr_t effectedCreaturePtr) const
    {
        if (effectedCreaturePtr->HasCondition(creature::condition::Poisoned))
        {
            effectedCreaturePtr->ConditionRemove(creature::condition::Poisoned);
            return Spell::EFFECT_STR_SUCCESS_;
        }
        else
        {
            return effectedCreaturePtr->NameOrRaceAndClass() + " is not poisoned";
        }
    }

}
}
