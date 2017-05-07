//
// conditions.cpp
//
#include "conditions.hpp"

#include "game/loop-manager.hpp"

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

    const ConditionSPtr_t ConditionFactory::GOOD_SPTR{ std::make_shared<Cond_Good>() };


    const condition::ConditionEnumVec_t Cond_Poisoned::PerTurnChange(CreaturePtrC_t creaturePtrC)
    {
        const stats::Health_t DAMAGE_BASE{ utilz::random::Int(2, 6) };
        auto const DAMAGE_FROM_HEALTH_NORMAL{ static_cast<stats::Health_t>(static_cast<float>(creaturePtrC->HealthNormal()) * 0.1f) };
        creaturePtrC->HealthCurrentAdj(-1 * (DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL));

        if (creaturePtrC->HealthCurrent() <= 0)
        {
            creaturePtrC->ConditionAdd(condition::ConditionFactory::Make(condition::Dead));
            return condition::ConditionEnumVec_t(1, condition::Dead );
        }
        else
        {
            return condition::ConditionEnumVec_t();
        }
    }


    ConditionSPtr_t ConditionFactory::Make(const condition::Enum E)
    {
        switch (E)
        {
            case Good:          { return GOOD_SPTR; }
            case Frightened:    { return std::make_shared<Cond_Frightened>(); }
            case Tripped:       { return std::make_shared<Cond_Tripped>(); }
            case Dazed:         { return std::make_shared<Cond_Dazed>(); }
            case Unconscious:   { return std::make_shared<Cond_Unconscious>(); }
            case Stone:         { return std::make_shared<Cond_Stone>(); }
            case Dead:          { return std::make_shared<Cond_Dead>(); }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::condition::ConditionWarehouse::Get(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool ConditionFactory::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::creature::condition::ConditionWarehouse::Test() Starting Tests...");
        }

        static auto condIndex{ 0 };
        if (condIndex < creature::condition::Count)
        {
            auto const NEXT_ENUM(static_cast<creature::condition::Enum>(condIndex));
            creature::ConditionSPtr_t COND_SPTR( Make(NEXT_ENUM) );
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR.get() != nullptr),             "game::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->Desc().empty() == false),     "game::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->LongDesc().empty() == false), "game::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->ToString().empty() == false), "game::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            ++condIndex;
            LoopManager::Instance()->TestingStrIncrement("Condition Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::title::ConditionWarehouse::Test()  ALL TESTS PASSED.");
        return true;
    }

}
}
}

