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
// condition-holder.cpp
//
#include "condition-holder.hpp"

#include "creature/condition.hpp"
#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace creature
{
    namespace condition
    {

        ConditionUVec_t Holder::conditionsUVec_;

        void Holder::Fill()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (conditionsUVec_.empty()),
                "creature::condition::Holder::Setup() was called twice.");

            // Note:  Keep order in sync with creature::Conditions::Enum
            conditionsUVec_.emplace_back(std::make_unique<Condition>());

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Bold,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, 25),
                                  std::make_pair(stats::Traits::Accuracy, 25),
                                  std::make_pair(stats::Traits::Charm, 25),
                                  std::make_pair(stats::Traits::Luck, 25),
                                  std::make_pair(stats::Traits::Speed, 25),
                                  std::make_pair(stats::Traits::Intelligence, 25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Heroic,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, 50),
                                  std::make_pair(stats::Traits::Accuracy, 50),
                                  std::make_pair(stats::Traits::Charm, 50),
                                  std::make_pair(stats::Traits::Luck, 50),
                                  std::make_pair(stats::Traits::Speed, 50),
                                  std::make_pair(stats::Traits::Intelligence, 50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Daunted,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, -25),
                                  std::make_pair(stats::Traits::Charm, -25),
                                  std::make_pair(stats::Traits::Speed, -16),
                                  std::make_pair(stats::Traits::Intelligence, -25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Panic,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Accuracy, -50),
                                  std::make_pair(stats::Traits::Charm, -50),
                                  std::make_pair(stats::Traits::Speed, -33),
                                  std::make_pair(stats::Traits::Intelligence, -50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Dazed,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -50),
                                  std::make_pair(stats::Traits::Accuracy, -66),
                                  std::make_pair(stats::Traits::Speed, -66),
                                  std::make_pair(stats::Traits::Intelligence, -50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Tripped,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -75),
                                  std::make_pair(stats::Traits::Accuracy, -75),
                                  std::make_pair(stats::Traits::Speed, -75) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::AsleepNatural,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -9999),
                                  std::make_pair(stats::Traits::Accuracy, -9999),
                                  std::make_pair(stats::Traits::Speed, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Poisoned,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -25),
                                  std::make_pair(stats::Traits::Charm, -50),
                                  std::make_pair(stats::Traits::Speed, -25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Pounced,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -75),
                                  std::make_pair(stats::Traits::Accuracy, -75),
                                  std::make_pair(stats::Traits::Speed, -75) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::AsleepMagical,
                true,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -9999),
                                  std::make_pair(stats::Traits::Accuracy, -9999),
                                  std::make_pair(stats::Traits::Speed, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Unconscious,
                false,
                stats::TraitSet({
                    std::make_pair(stats::Traits::Strength, -9999),
                    std::make_pair(stats::Traits::Accuracy, -9999),
                    std::make_pair(stats::Traits::Charm, -9999),
                    std::make_pair(stats::Traits::Speed, -9999),
                    std::make_pair(stats::Traits::Intelligence, -9999),
                })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Stone,
                true,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -9999),
                                  std::make_pair(stats::Traits::Accuracy, -9999),
                                  std::make_pair(stats::Traits::Charm, -9999),
                                  std::make_pair(stats::Traits::Speed, -9999),
                                  std::make_pair(stats::Traits::Intelligence, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Dead,
                false,
                stats::TraitSet({ std::make_pair(stats::Traits::Strength, -9999),
                                  std::make_pair(stats::Traits::Accuracy, -9999),
                                  std::make_pair(stats::Traits::Charm, -9999),
                                  std::make_pair(stats::Traits::Luck, -9999),
                                  std::make_pair(stats::Traits::Speed, -9999),
                                  std::make_pair(stats::Traits::Intelligence, -9999) })));
        }

        void Holder::Empty() { conditionsUVec_.clear(); }

        bool Holder::Test()
        {
            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "creature::condition::Holder::Test() Starting Tests...");
            }

            static misc::EnumUnderlying_t condIndex{ 0 };
            if (condIndex < creature::Conditions::Count)
            {
                auto const NEXT_ENUM{ static_cast<creature::Conditions::Enum>(condIndex) };
                auto const CONDITION_PTR{ Get(NEXT_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CONDITION_PTR->Desc().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(NEXT_ENUM)
                                                           << "\") resulted in an empty Desc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CONDITION_PTR->LongDesc().empty() == false),
                    "creature::condition::Holder::Test(\""
                        << Conditions::ToString(NEXT_ENUM)
                        << "\") resulted in an empty LongDesc().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CONDITION_PTR->ToString().empty() == false),
                    "creature::condition::Holder::Test(\""
                        << Conditions::ToString(NEXT_ENUM)
                        << "\") resulted in an empty ImageFilename().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CONDITION_PTR->Name().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(NEXT_ENUM)
                                                           << "\") resulted in an empty Name().");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (CONDITION_PTR->Name() == Conditions::Name(NEXT_ENUM)),
                    "creature::condition::Holder::Test(ptr=\""
                        << CONDITION_PTR->Name() << "\", enum=\"" << Conditions::ToString(NEXT_ENUM)
                        << "\") Condition is out of order.");

                ++condIndex;

                game::LoopManager::Instance()->TestingStrIncrement(
                    "Condition Test \"" + CONDITION_PTR->Name() + "\"");

                return false;
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "creature::title::Holder::Test()  ALL TESTS PASSED.");

            return true;
        }

        const ConditionPtr_t Holder::Get(const Conditions::Enum E)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (conditionsUVec_.empty() == false),
                "creature::condition::Holder::Get(" << Conditions::ToString(E)
                                                    << ") was called when Holder was empty.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (static_cast<std::size_t>(E) < conditionsUVec_.size()),
                "creature::condition::Holder::Get("
                    << Conditions::ToString(E)
                    << ") found insuff sized conditionsUVec_, probably from a bug in Fill().");

            return conditionsUVec_.at(static_cast<std::size_t>(E)).get();
        }

    } // namespace condition
} // namespace creature
} // namespace heroespath
