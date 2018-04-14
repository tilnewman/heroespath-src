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
// spell-warehouse.cpp
//
#include "spell-warehouse.hpp"

#include "combat/effect-type-enum.hpp"
#include "combat/target-enum.hpp"
#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace spell
{

    SpellUVec_t Warehouse::spellsUVec_;

    void Warehouse::Fill()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (spellsUVec_.empty()), "spell::Warehouse::Setup() was called twice.");

        // Note::Keep order in sync with spell::Spells::Enum
        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Sparks,
            combat::EffectType::CreatureHarmDamage,
            game::Phase::Combat,
            2_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "sprays with sparks",
            "sprayed with sparks"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Bandage,
            combat::EffectType::CreatureHelpHeal,
            static_cast<game::Phase::Enum>(
                game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::SingleCompanion,
            "bandages",
            "bandaged"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Sleep,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "sleeps",
            "slept"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Awaken,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(
                game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::SingleCompanion,
            "awakens",
            "awakened"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Trip,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "trips",
            "tripped"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Lift,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(
                game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::SingleCompanion,
            "lifts",
            "lifted"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Daze,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "dazes",
            "dazed"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Panic,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "panics",
            "panicked"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::ClearMind,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(
                game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::SingleCompanion,
            "clears the mind",
            "clear headed"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Poison,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::SingleOpponent,
            "poisons",
            "poisoned"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::Antidote,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(
                game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::SingleCompanion,
            "cures",
            "cured"));

        spellsUVec_.emplace_back(std::make_unique<Spell>(
            Spells::PoisonCloud,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            3_mana,
            1_rank,
            combat::TargetType::AllOpponents,
            "poisons",
            "poisoned"));
    }

    void Warehouse::Empty() { spellsUVec_.clear(); }

    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "spell::Warehouse::Test() Starting Tests...");
        }

        static auto spellIndex{ 0 };
        if (spellIndex < Spells::Count)
        {
            auto const NEXT_ENUM{ static_cast<Spells::Enum>(spellIndex) };
            auto const SPELL_PTR{ Get(NEXT_ENUM) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->Name().empty() == false),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") resulted in an empty Name().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->Desc().empty() == false),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") resulted in an empty Desc().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->DescExtra().empty() == false),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") resulted in an empty DescExtra().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->ManaCost().IsNonZero()),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") resulted in a zero Mana cost.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->Rank().IsNonZero()),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") resulted in a zero Rank.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SPELL_PTR->Name() == Spells::Name(NEXT_ENUM)),
                "spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM)
                                            << "\") Spell is out of order.");

            ++spellIndex;

            game::LoopManager::Instance()->TestingStrIncrement(
                "Spell Test \"" + SPELL_PTR->Name() + "\"");

            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "spell::Warehouse::Test()  ALL TESTS PASSED.");

        return true;
    }

    const SpellPtr_t Warehouse::Get(const Spells::Enum SPELL_ENUM)
    {
        auto const SPELL_INDEX{ static_cast<std::size_t>(SPELL_ENUM) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (spellsUVec_.empty() == false),
            "spell::Warehouse::Get(spell_enum=" << SPELL_ENUM
                                                << ") was called when the warehouse was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (SPELL_INDEX < spellsUVec_.size()),
            "spell::Warehouse::Get(spell_enum="
                << SPELL_ENUM << ", spell_index=" << SPELL_INDEX
                << ") that index was NOT less than the spellsUVec_.size()=" << spellsUVec_.size()
                << ".");

        return spellsUVec_[SPELL_INDEX].get();
    }

} // namespace spell
} // namespace heroespath
