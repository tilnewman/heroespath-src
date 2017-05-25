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

#include "game/loop-manager.hpp"
#include "game/spell/spells.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <memory>


namespace game
{
namespace spell
{

    SpellUVec_t Warehouse::spellsUVec_;


    void Warehouse::Fill()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsUVec_.empty()), "game::spell::Warehouse::Setup() was called twice.");

        //Note::Keep order in sync with game::spell::Spells::Enum
        spellsUVec_.push_back( std::unique_ptr<Sparks>( new Sparks() ) );
        spellsUVec_.push_back( std::unique_ptr<Bandage>( new Bandage() ) );
        spellsUVec_.push_back( std::unique_ptr<Sleep>( new Sleep() ) );
        spellsUVec_.push_back( std::unique_ptr<Awaken>( new Awaken() ) );
        spellsUVec_.push_back( std::unique_ptr<Trip>( new Trip() ) );
        spellsUVec_.push_back( std::unique_ptr<Lift>( new Lift() ) );
        spellsUVec_.push_back( std::unique_ptr<Daze>( new Daze() ) );
        spellsUVec_.push_back( std::unique_ptr<Frighten>( new Frighten() ) );
        spellsUVec_.push_back( std::unique_ptr<ClearMind>( new ClearMind() ) );
        spellsUVec_.push_back( std::unique_ptr<Poison>( new Poison() ) );
        spellsUVec_.push_back( std::unique_ptr<Antidote>( new Antidote() ) );
        spellsUVec_.push_back( std::unique_ptr<PoisonCloud>( new PoisonCloud() ) );
    }


    void Warehouse::Empty()
    {
        spellsUVec_.clear();
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
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr != nullptr),                         "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") Get() resulted in a nullptr being returned.");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Name().empty() == false),           "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty Name().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Desc().empty() == false),           "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty Desc().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->DescExtra().empty() == false),      "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in an empty DescExtra().");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->ManaCost() != 0),                   "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in a zero Mana cost.");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Rank() != 0),                       "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") resulted in a zero Rank.");
            M_ASSERT_OR_LOGANDTHROW_SS((spellPtr->Name() == Spells::Name(NEXT_ENUM)), "game::spell::Warehouse::Test(\"" << Spells::ToString(NEXT_ENUM) << "\") Spell is out of order.");
            ++spellIndex;
            LoopManager::Instance()->TestingStrIncrement("Spell Test \"" + spellPtr->Name() + "\"");
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("game::spell::Warehouse::Test()  ALL TESTS PASSED.");
        return true;
    }


    SpellPtr_t Warehouse::Get(const Spells::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((spellsUVec_.empty() == false), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") was called before Setup().");
        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < spellsUVec_.size()), "game::spell::Warehouse::Get(" << Spells::ToString(E) << ") found insuff sized spellsUVec_, probably from a bug in Setup().");
        return spellsUVec_.at(static_cast<std::size_t>(E)).get();
    }

}
}
