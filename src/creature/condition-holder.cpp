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
        Trait_t Holder::SMALLEST_TRAIT_POSSIBLE_ = std::numeric_limits<Trait_t>::lowest();

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
                TraitSet({ std::make_pair(Traits::Strength, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Accuracy, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Speed, SMALLEST_TRAIT_POSSIBLE_) })));

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
                TraitSet({ std::make_pair(Traits::Strength, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Accuracy, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Speed, SMALLEST_TRAIT_POSSIBLE_) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Unconscious,
                false,
                TraitSet({
                    std::make_pair(Traits::Strength, SMALLEST_TRAIT_POSSIBLE_),
                    std::make_pair(Traits::Accuracy, SMALLEST_TRAIT_POSSIBLE_),
                    std::make_pair(Traits::Charm, SMALLEST_TRAIT_POSSIBLE_),
                    std::make_pair(Traits::Speed, SMALLEST_TRAIT_POSSIBLE_),
                    std::make_pair(Traits::Intelligence, SMALLEST_TRAIT_POSSIBLE_),
                })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Stone,
                true,
                TraitSet({ std::make_pair(Traits::Strength, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Accuracy, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Charm, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Speed, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Intelligence, SMALLEST_TRAIT_POSSIBLE_) })));

            conditionsUVec_.emplace_back(std::make_unique<Condition>(
                Conditions::Dead,
                false,
                TraitSet({ std::make_pair(Traits::Strength, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Accuracy, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Charm, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Luck, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Speed, SMALLEST_TRAIT_POSSIBLE_),
                           std::make_pair(Traits::Intelligence, SMALLEST_TRAIT_POSSIBLE_) })));
        }

        void Holder::Empty() { conditionsUVec_.clear(); }

        const ConditionPtr_t Holder::Get(const Conditions::Enum ENUM)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (conditionsUVec_.empty() == false),
                "creature::condition::Holder::Get(" << NAMEOF_ENUM(ENUM)
                                                    << ") was called when Holder was empty.");

            const auto INDEX { static_cast<std::size_t>(ENUM) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < conditionsUVec_.size()),
                "creature::condition::Holder::Get("
                    << NAMEOF_ENUM(ENUM)
                    << ") found insuff sized conditionsUVec_, probably from a bug in Fill().");

            return ConditionPtr_t(conditionsUVec_[INDEX].get());
        }

    } // namespace condition
} // namespace creature
} // namespace heroespath
