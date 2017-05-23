//
// condition-warehouse.cpp
//
#include "condition-warehouse.hpp"

#include "game/creature/conditions.hpp"
#include "game/loop-manager.hpp"

#include "utilz/assertlogandthrow.hpp"


namespace game
{
namespace creature
{
namespace condition
{

    ConditionSVec_t Warehouse::conditionsSVec_;


    void Warehouse::Setup()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((conditionsSVec_.empty()), "game::creature::condition::Warehouse::Setup() was called twice.");

        conditionsSVec_.resize(Conditions::Count);

        conditionsSVec_[Conditions::AsleepMagical]  = std::make_shared<AsleepMagical>();
        conditionsSVec_[Conditions::AsleepNatural]  = std::make_shared<AsleepNatural>();
        conditionsSVec_[Conditions::Dazed]          = std::make_shared<Dazed>();
        conditionsSVec_[Conditions::Dead]           = std::make_shared<Dead>();
        conditionsSVec_[Conditions::Frightened]     = std::make_shared<Frightened>();
        conditionsSVec_[Conditions::Good]           = std::make_shared<Good>();
        conditionsSVec_[Conditions::Poisoned]       = std::make_shared<Poisoned>();
        conditionsSVec_[Conditions::Stone]          = std::make_shared<Stone>();
        conditionsSVec_[Conditions::Tripped]        = std::make_shared<Tripped>();
        conditionsSVec_[Conditions::Unconscious]    = std::make_shared<Unconscious>();
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::creature::condition::Warehouse::Test() Starting Tests...");
        }

        static auto condIndex{ 0 };
        if (condIndex < creature::Conditions::Count)
        {
            auto const NEXT_ENUM{ static_cast<creature::Conditions::Enum>(condIndex) };
            auto conditionPtr{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((conditionPtr != nullptr),                   "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((conditionPtr->Desc().empty() == false),     "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((conditionPtr->LongDesc().empty() == false), "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((conditionPtr->ToString().empty() == false), "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            ++condIndex;
            LoopManager::Instance()->TestingStrIncrement("Condition Test \"" + conditionPtr->Name() + "\"");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::title::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }


    ConditionPtr_t Warehouse::Get(const Conditions::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((conditionsSVec_.empty() == false), "game::creature::condition::Warehouse::Get(" << Conditions::ToString(E) << ") was called before Setup().");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < conditionsSVec_.size()), "game::creature::condition::Warehouse::Get(" << Conditions::ToString(E) << ") found insuff sized conditionsSVec_, probably from a bug in Setup().");
        return conditionsSVec_.at(static_cast<std::size_t>(E)).get();
    }

}
}
}
