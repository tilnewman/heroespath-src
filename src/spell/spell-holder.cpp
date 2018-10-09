// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// spell-holder.cpp
//
#include "spell-holder.hpp"

#include "combat/effect-type-enum.hpp"
#include "combat/target-enum.hpp"
#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace spell
{

    SpellUVec_t Holder::spellsUVec_;

    void Holder::Fill()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (spellsUVec_.empty()), "spell::Holder::Setup() was called twice.");

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

    void Holder::Empty() { spellsUVec_.clear(); }

    bool Holder::Test()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "spell::Holder::Test() Starting Tests...");
        }

        static misc::EnumUnderlying_t spellIndex { 0 };
        if (spellIndex < Spells::Count)
        {
            const auto NEXT_ENUM { static_cast<Spells::Enum>(spellIndex) };
            const auto SPELL_PTR { Get(NEXT_ENUM) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->Name().empty() == false),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") resulted in an empty Name().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->Desc().empty() == false),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") resulted in an empty Desc().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->DescExtra().empty() == false),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") resulted in an empty DescExtra().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->ManaCost().IsNonZero()),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") resulted in a zero Mana cost.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->Rank().IsNonZero()),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") resulted in a zero Rank.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SPELL_PTR->Name() == Spells::Name(NEXT_ENUM)),
                "spell::Holder::Test(\"" << Spells::ToString(NEXT_ENUM)
                                         << "\") Spell is out of order.");

            ++spellIndex;

            game::LoopManager::Instance()->TestingStrIncrement(
                "Spell Test \"" + SPELL_PTR->Name() + "\"");

            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("spell::Holder::Test()  ALL TESTS PASSED.");

        return true;
    }

    const SpellPtr_t Holder::Get(const Spells::Enum SPELL_ENUM)
    {
        const auto SPELL_INDEX { static_cast<std::size_t>(SPELL_ENUM) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (spellsUVec_.empty() == false),
            "spell::Holder::Get(spell_enum=" << SPELL_ENUM
                                             << ") was called when the holder was empty.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (SPELL_INDEX < spellsUVec_.size()),
            "spell::Holder::Get(spell_enum="
                << SPELL_ENUM << ", spell_index=" << SPELL_INDEX
                << ") that index was NOT less than the spellsUVec_.size()=" << spellsUVec_.size()
                << ".");

        return spellsUVec_[SPELL_INDEX].get();
    }

} // namespace spell
} // namespace heroespath
