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
// condition-warehouse.cpp
//
#include "condition-warehouse.hpp"

#include "game/creature/conditions.hpp"
#include "game/loop-manager.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace creature
{
namespace condition
{

    ConditionUVec_t Warehouse::conditionsUVec_;


    void Warehouse::Fill()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((conditionsUVec_.empty()), "game::creature::condition::Warehouse::Setup() was called twice.");

        //Note:  Keep order in sync with game::creature::Conditions::Enum
        conditionsUVec_.push_back( std::unique_ptr<Good>( new Good() ) );
        conditionsUVec_.push_back( std::unique_ptr<Frightened>( new Frightened() ) );
        conditionsUVec_.push_back( std::unique_ptr<Dazed>( new Dazed() ) );
        conditionsUVec_.push_back( std::unique_ptr<Tripped>( new Tripped() ) );
        conditionsUVec_.push_back( std::unique_ptr<AsleepNatural>( new AsleepNatural() ) );
        conditionsUVec_.push_back( std::unique_ptr<Poisoned>( new Poisoned() ) );
        conditionsUVec_.push_back( std::unique_ptr<AsleepMagical>( new AsleepMagical() ) );
        conditionsUVec_.push_back( std::unique_ptr<Unconscious>( new Unconscious() ) );
        conditionsUVec_.push_back( std::unique_ptr<Stone>( new Stone() ) );
        conditionsUVec_.push_back( std::unique_ptr<Dead>( new Dead() ) );
    }


    void Warehouse::Empty()
    {
        conditionsUVec_.clear();
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
            auto cndPtr{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr != nullptr),                             "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr->Desc().empty() == false),               "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr->LongDesc().empty() == false),           "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty LongDesc().");
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr->ToString().empty() == false),           "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty ImageFilename().");
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr->Name().empty() == false),               "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty Name().");
            M_ASSERT_OR_LOGANDTHROW_SS((cndPtr->Name() == Conditions::Name(NEXT_ENUM)), "game::creature::condition::Warehouse::Test(\"" << Conditions::ToString(NEXT_ENUM) << "\") Condition is out of order.");
            ++condIndex;
            LoopManager::Instance()->TestingStrIncrement("Condition Test \"" + cndPtr->Name() + "\"");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::creature::title::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }


    ConditionPtr_t Warehouse::Get(const Conditions::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((conditionsUVec_.empty() == false), "game::creature::condition::Warehouse::Get(" << Conditions::ToString(E) << ") was called before Setup().");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < conditionsUVec_.size()), "game::creature::condition::Warehouse::Get(" << Conditions::ToString(E) << ") found insuff sized conditionsUVec_, probably from a bug in Setup().");
        return conditionsUVec_.at(static_cast<std::size_t>(E)).get();
    }

}
}
}
