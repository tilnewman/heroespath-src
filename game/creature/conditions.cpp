//
// conditions.cpp
//
#include "conditions.hpp"

#include "game/loop-manager.hpp"
#include "game/creature/condition-warehouse.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "utilz/random.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace creature
{
namespace condition
{

    const ConditionEnumVec_t Poisoned::PerTurnChange(CreaturePtrC_t creaturePtrC)
    {
        const stats::Health_t DAMAGE_BASE{ utilz::random::Int(2, 6) };
        auto const DAMAGE_FROM_HEALTH_NORMAL{ static_cast<stats::Health_t>(static_cast<float>(creaturePtrC->HealthNormal()) * 0.1f) };
        creaturePtrC->HealthCurrentAdj(-1 * (DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL));

        if (creaturePtrC->HealthCurrent() <= 0)
        {
            creaturePtrC->ConditionAdd(Conditions::Dead);
            return ConditionEnumVec_t(1, Conditions::Dead );
        }
        else
        {
            return ConditionEnumVec_t();
        }
    }

}
}
}

