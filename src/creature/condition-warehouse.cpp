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

#include "creature/conditions.hpp"
#include "game/loop-manager.hpp"

#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace creature
{
    namespace condition
    {

        ConditionUVec_t Warehouse::conditionsUVec_;

        void Warehouse::Fill()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (conditionsUVec_.empty()),
                "creature::condition::Warehouse::Setup() was called twice.");

            // Note:  Keep order in sync with creature::Conditions::Enum
            conditionsUVec_.emplace_back(std::make_unique<Good>());
            conditionsUVec_.emplace_back(std::make_unique<Bold>());
            conditionsUVec_.emplace_back(std::make_unique<Heroic>());
            conditionsUVec_.emplace_back(std::make_unique<Daunted>());
            conditionsUVec_.emplace_back(std::make_unique<Panic>());
            conditionsUVec_.emplace_back(std::make_unique<Dazed>());
            conditionsUVec_.emplace_back(std::make_unique<Tripped>());
            conditionsUVec_.emplace_back(std::make_unique<AsleepNatural>());
            conditionsUVec_.emplace_back(std::make_unique<Poisoned>());
            conditionsUVec_.emplace_back(std::make_unique<Pounced>());
            conditionsUVec_.emplace_back(std::make_unique<AsleepMagical>());
            conditionsUVec_.emplace_back(std::make_unique<Unconscious>());
            conditionsUVec_.emplace_back(std::make_unique<Stone>());
            conditionsUVec_.emplace_back(std::make_unique<Dead>());
        }

        void Warehouse::Empty() { conditionsUVec_.clear(); }

        bool Warehouse::Test()
        {
            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "creature::condition::Warehouse::Test() Starting Tests...");
            }

            static auto condIndex{ 0 };
            if (condIndex < creature::Conditions::Count)
            {
                auto const NEXT_ENUM{ static_cast<creature::Conditions::Enum>(condIndex) };
                auto cndPtr{ Get(NEXT_ENUM) };
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr != nullptr),
                    "creature::condition::Warehouse::Test(\""
                        << Conditions::ToString(NEXT_ENUM)
                        << "\") resulted in a nullptr being returned.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr->Desc().empty() == false),
                    "creature::condition::Warehouse::Test(\""
                        << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr->LongDesc().empty() == false),
                    "creature::condition::Warehouse::Test(\""
                        << Conditions::ToString(NEXT_ENUM)
                        << "\") resulted in an empty LongDesc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr->ToString().empty() == false),
                    "creature::condition::Warehouse::Test(\""
                        << Conditions::ToString(NEXT_ENUM)
                        << "\") resulted in an empty ImageFilename().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr->Name().empty() == false),
                    "creature::condition::Warehouse::Test(\""
                        << Conditions::ToString(NEXT_ENUM) << "\") resulted in an empty Name().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cndPtr->Name() == Conditions::Name(NEXT_ENUM)),
                    "creature::condition::Warehouse::Test(ptr=\""
                        << cndPtr->Name() << "\", enum=\"" << Conditions::ToString(NEXT_ENUM)
                        << "\") Condition is out of order.");

                ++condIndex;
                game::LoopManager::Instance()->TestingStrIncrement(
                    "Condition Test \"" + cndPtr->Name() + "\"");

                return false;
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "creature::title::Warehouse::Test()  ALL TESTS PASSED.");
            return true;
        }

        ConditionPtr_t Warehouse::Get(const Conditions::Enum E)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (conditionsUVec_.empty() == false),
                "creature::condition::Warehouse::Get(" << Conditions::ToString(E)
                                                       << ") was called before Setup().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (static_cast<std::size_t>(E) < conditionsUVec_.size()),
                "creature::condition::Warehouse::Get("
                    << Conditions::ToString(E)
                    << ") found insuff sized conditionsUVec_, probably from a bug in Setup().");

            return conditionsUVec_.at(static_cast<std::size_t>(E)).get();
        }
    } // namespace condition
} // namespace creature
} // namespace heroespath
