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

    SpellSVec_t Warehouse::spellsSVec_;


    void Warehouse::Setup()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsSVec_.empty()), "game::spell::Warehouse::Setup() was called twice.");

        spellsSVec_.resize(Spells::Count);

        spellsSVec_[Spells::Antidote]    = std::make_shared<Antidote>();
        spellsSVec_[Spells::Awaken]      = std::make_shared<Awaken>();
        spellsSVec_[Spells::Bandage]     = std::make_shared<Bandage>();
        spellsSVec_[Spells::ClearMind]   = std::make_shared<ClearMind>();
        spellsSVec_[Spells::Daze]        = std::make_shared<Daze>();
        spellsSVec_[Spells::Frighten]    = std::make_shared<Frighten>();
        spellsSVec_[Spells::Lift]        = std::make_shared<Lift>();
        spellsSVec_[Spells::Poison]      = std::make_shared<Poison>();
        spellsSVec_[Spells::PoisonCloud] = std::make_shared<PoisonCloud>();
        spellsSVec_[Spells::Sleep]       = std::make_shared<Sleep>();
        spellsSVec_[Spells::Sparks]      = std::make_shared<Sparks>();
        spellsSVec_[Spells::Trip]        = std::make_shared<Trip>();
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
            LoopManager::Instance()->TestingStrIncrement("Spell Test \"" + spellPtr->Name() + "\"");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::spell::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }


    SpellPtr_t Warehouse::Get(const Spells::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsSVec_.empty() == false), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") was called before Setup().");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < spellsSVec_.size()), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") found insuff sized spellsSVec_, probably from a bug in Setup().");
        return spellsSVec_.at(E).get();
    }

}
}
