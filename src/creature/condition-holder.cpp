// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition-holder.cpp
//
#include "condition-holder.hpp"

#include "creature/condition.hpp"
#include "misc/assertlogandthrow.hpp"
#include "stage/i-stage.hpp"

namespace heroespath
{
namespace creature
{
    namespace condition
    {

        ConditionUVec_t Holder::conditionsUVec_;

        void Holder::Fill()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (conditionsUVec_.empty()),
                "creature::condition::Holder::Setup() was called twice.");

            // Note:  Keep order in sync with creature::Conditions::Enum
            conditionsUVec_.emplace_back(std::make_unique<Condition>());

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Bold,
                false,
                TraitSet({ std::make_pair(Traits::Strength, 25),
                           std::make_pair(Traits::Accuracy, 25),
                           std::make_pair(Traits::Charm, 25),
                           std::make_pair(Traits::Luck, 25),
                           std::make_pair(Traits::Speed, 25),
                           std::make_pair(Traits::Intelligence, 25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Heroic,
                false,
                TraitSet({ std::make_pair(Traits::Strength, 50),
                           std::make_pair(Traits::Accuracy, 50),
                           std::make_pair(Traits::Charm, 50),
                           std::make_pair(Traits::Luck, 50),
                           std::make_pair(Traits::Speed, 50),
                           std::make_pair(Traits::Intelligence, 50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Daunted,
                false,
                TraitSet({ std::make_pair(Traits::Accuracy, -25),
                           std::make_pair(Traits::Charm, -25),
                           std::make_pair(Traits::Speed, -16),
                           std::make_pair(Traits::Intelligence, -25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Panic,
                false,
                TraitSet({ std::make_pair(Traits::Accuracy, -50),
                           std::make_pair(Traits::Charm, -50),
                           std::make_pair(Traits::Speed, -33),
                           std::make_pair(Traits::Intelligence, -50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Dazed,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -50),
                           std::make_pair(Traits::Accuracy, -66),
                           std::make_pair(Traits::Speed, -66),
                           std::make_pair(Traits::Intelligence, -50) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Tripped,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -75),
                           std::make_pair(Traits::Accuracy, -75),
                           std::make_pair(Traits::Speed, -75) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::AsleepNatural,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -9999),
                           std::make_pair(Traits::Accuracy, -9999),
                           std::make_pair(Traits::Speed, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Poisoned,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -25),
                           std::make_pair(Traits::Charm, -50),
                           std::make_pair(Traits::Speed, -25) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Pounced,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -75),
                           std::make_pair(Traits::Accuracy, -75),
                           std::make_pair(Traits::Speed, -75) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::AsleepMagical,
                true,
                TraitSet({ std::make_pair(Traits::Strength, -9999),
                           std::make_pair(Traits::Accuracy, -9999),
                           std::make_pair(Traits::Speed, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Unconscious,
                false,
                TraitSet({
                    std::make_pair(Traits::Strength, -9999),
                    std::make_pair(Traits::Accuracy, -9999),
                    std::make_pair(Traits::Charm, -9999),
                    std::make_pair(Traits::Speed, -9999),
                    std::make_pair(Traits::Intelligence, -9999),
                })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Stone,
                true,
                TraitSet({ std::make_pair(Traits::Strength, -9999),
                           std::make_pair(Traits::Accuracy, -9999),
                           std::make_pair(Traits::Charm, -9999),
                           std::make_pair(Traits::Speed, -9999),
                           std::make_pair(Traits::Intelligence, -9999) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Dead,
                false,
                TraitSet({ std::make_pair(Traits::Strength, -9999),
                           std::make_pair(Traits::Accuracy, -9999),
                           std::make_pair(Traits::Charm, -9999),
                           std::make_pair(Traits::Luck, -9999),
                           std::make_pair(Traits::Speed, -9999),
                           std::make_pair(Traits::Intelligence, -9999) })));
        }

        void Holder::Empty() { conditionsUVec_.clear(); }

        bool Holder::Test(stage::IStagePtr_t iStagePtr)
        {
            static auto hasInitialPrompt { false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                iStagePtr->TestingStrAppend(
                    "creature::condition::Holder::Test() Starting Tests...");
            }

            static misc::EnumUnderlying_t condIndex { 0 };
            if (condIndex < creature::Conditions::Count)
            {
                const auto NEXT_ENUM { static_cast<creature::Conditions::Enum>(condIndex) };
                const auto CONDITION_PTR { Get(NEXT_ENUM) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (CONDITION_PTR->Desc().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(NEXT_ENUM)
                                                           << "\") resulted in an empty Desc().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (CONDITION_PTR->LongDesc().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(
                        NEXT_ENUM) << "\") resulted in an empty LongDesc().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (CONDITION_PTR->ToString().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(
                        NEXT_ENUM) << "\") resulted in an empty ImageFilename().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (CONDITION_PTR->Name().empty() == false),
                    "creature::condition::Holder::Test(\"" << Conditions::ToString(NEXT_ENUM)
                                                           << "\") resulted in an empty Name().");

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (CONDITION_PTR->Name() == Conditions::Name(NEXT_ENUM)),
                    "creature::condition::Holder::Test(ptr=\""
                        << CONDITION_PTR->Name() << "\", enum=\"" << Conditions::ToString(NEXT_ENUM)
                        << "\") Condition is out of order.");

                ++condIndex;

                iStagePtr->TestingStrIncrement("Condition Test \"" + CONDITION_PTR->Name() + "\"");

                return false;
            }

            iStagePtr->TestingStrAppend("creature::title::Holder::Test()  ALL TESTS PASSED.");

            return true;
        }

        const ConditionPtr_t Holder::Get(const Conditions::Enum E)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (conditionsUVec_.empty() == false),
                "creature::condition::Holder::Get(" << Conditions::ToString(E)
                                                    << ") was called when Holder was empty.");

            const auto INDEX { static_cast<std::size_t>(E) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < conditionsUVec_.size()),
                "creature::condition::Holder::Get(" << Conditions::ToString(
                    E) << ") found insuff sized conditionsUVec_, probably from a bug in Fill().");

            return ConditionPtr_t(conditionsUVec_[INDEX].get());
        }

    } // namespace condition
} // namespace creature
} // namespace heroespath
