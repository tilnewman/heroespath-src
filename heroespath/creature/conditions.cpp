//
// conditions.cpp
//
#include "conditions.hpp"

#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/loop-manager.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace creature
{
namespace condition
{

    const ConditionSPtr_t ConditionFactory::GOOD_SPTR{ std::make_shared<Cond_Good>() };


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
                ss << "heroespath::creature::condition::ConditionWarehouse::Get(" << E << ")_InvalidValueError.";
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
            LoopManager::Instance()->TestingStrAppend("heroespath::creature::condition::ConditionWarehouse::Test() Starting Tests...");
        }

        static auto condIndex{ 0 };
        if (condIndex < creature::condition::Count)
        {
            auto const NEXT_ENUM(static_cast<creature::condition::Enum>(condIndex));
            creature::ConditionSPtr_t COND_SPTR( Make(NEXT_ENUM) );
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR.get() != nullptr),             "heroespath::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->Desc().empty() == false),     "heroespath::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->LongDesc().empty() == false), "heroespath::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((COND_SPTR->ToString().empty() == false), "heroespath::creature::condition::ConditionWarehouse::Test(\"" << creature::condition::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            ++condIndex;
            LoopManager::Instance()->TestingStrIncrement("Condition Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("heroespath::creature::title::ConditionWarehouse::Test()  ALL TESTS PASSED.");
        return true;
    }

}
}
}

