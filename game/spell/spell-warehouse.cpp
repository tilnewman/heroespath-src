//
// spell-warehouse.cpp
//
#include "spell-warehouse.hpp"

#include "game/loop-manager.hpp"
#include "game/spell/spells.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <memory>


namespace game
{
namespace spell
{

    SpellUVec_t Warehouse::spellsUVec_;


    void Warehouse::Setup()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsUVec_.empty()), "game::spell::Warehouse::Setup() was called twice.");

        spellsUVec_.resize(Spells::Count);

        spellsUVec_[Spells::Antidote]   = std::make_unique<Antidote>();
        spellsUVec_[Spells::Awaken]     = std::make_unique<Awaken>();
        spellsUVec_[Spells::Bandage]    = std::make_unique<Bandage>();
        spellsUVec_[Spells::ClearMind]  = std::make_unique<ClearMind>();
        spellsUVec_[Spells::Daze]       = std::make_unique<Daze>();
        spellsUVec_[Spells::Frighten]   = std::make_unique<Frighten>();
        spellsUVec_[Spells::Lift]       = std::make_unique<Lift>();
        spellsUVec_[Spells::Poison]     = std::make_unique<Poison>();
        spellsUVec_[Spells::Sleep]      = std::make_unique<Sleep>();
        spellsUVec_[Spells::Sparks]     = std::make_unique<Sparks>();
        spellsUVec_[Spells::Trip]       = std::make_unique<Trip>();
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend("game::spell::Warehouse::Test() Starting Tests...");
        }

        static auto spellIndex{ 0 };
        if (spellIndex < Spells::Count)
        {
            auto const NEXT_ENUM{ static_cast<Spells::Enum>(spellIndex) };
            auto spellPtr{ Get(NEXT_ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr != nullptr),                    "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") Get() resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Name().empty() == false),      "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty Name().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Desc().empty() == false),      "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->DescExtra().empty() == false), "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty DescExtra().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->ManaCost() != 0),              "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in a zero Mana cost.");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Rank() != 0),                  "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in a zero Rank.");
            ++spellIndex;
            LoopManager::Instance()->TestingStrIncrement("Spell Test #");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::spell::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }


    SpellPtr_t Warehouse::Get(const Spells::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsUVec_.empty() == false), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") was called before Setup().");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < spellsUVec_.size()), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") found insuff sized spellsUVec_, probably from a bug in Setup().");
        SpellPtr_t spellPtr{ spellsUVec_[E].get() };
        return spellPtr;
    }

}
}
