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
// song-warehouse.cpp
//
#include "song-warehouse.hpp"

#include "game/loop-manager.hpp"
#include "game/song/songs.hpp"

#include "misc/assertlogandthrow.hpp"

#include <memory>


namespace game
{
namespace song
{

    SongUVec_t Warehouse::songsUVec_;


    void Warehouse::Fill()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((songsUVec_.empty()),
            "game::song::Warehouse::Setup() was called twice.");

        //Note::Keep order in sync with game::song::Songs::Enum
        songsUVec_.push_back( std::make_unique<RallyDrum>() );
        songsUVec_.push_back( std::make_unique<SpiritResonance>() );
        songsUVec_.push_back( std::make_unique<RousingRhythm>() );
        songsUVec_.push_back( std::make_unique<TripBeat>() );
        songsUVec_.push_back( std::make_unique<PanicStrings>() );
        songsUVec_.push_back( std::make_unique<Lullaby>() );
    }


    void Warehouse::Empty()
    {
        songsUVec_.clear();
    }


    bool Warehouse::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend(
                "game::song::Warehouse::Test() Starting Tests...");
        }

        static auto songIndex{ 0 };
        if (songIndex < Songs::Count)
        {
            auto const NEXT_ENUM{ static_cast<Songs::Enum>(songIndex) };
            auto songPtr{ Get(NEXT_ENUM) };

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr != nullptr),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") Get() resulted in a nullptr being returned.");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->Name().empty() == false),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") resulted in an empty Name().");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->Desc().empty() == false),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") resulted in an empty Desc().");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->DescExtra().empty() == false),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") resulted in an empty DescExtra().");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->ManaCost() != 0),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") resulted in a zero Mana cost.");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->Rank() != 0_rank),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") resulted in a zero Rank.");

            M_ASSERT_OR_LOGANDTHROW_SS((songPtr->Name() == Songs::Name(NEXT_ENUM)),
                "game::song::Warehouse::Test(\"" << Songs::ToString(NEXT_ENUM)
                << "\") Song is out of order.");

            ++songIndex;
            LoopManager::Instance()->TestingStrIncrement("Song Test \"" +
                songPtr->Name() + "\"");

            return false;
        }

        LoopManager::Instance()->TestingStrAppend(
            "game::song::Warehouse::Test()  ALL TESTS PASSED.");

        return true;
    }


    SongPtr_t Warehouse::Get(const Songs::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((songsUVec_.empty() == false),
            "game::song::Warehouse::Get(" << Songs::ToString(E)
            << ") was called before Setup().");

        M_ASSERT_OR_LOGANDTHROW_SS((static_cast<std::size_t>(E) < songsUVec_.size()),
            "game::song::Warehouse::Get(" << Songs::ToString(E)
            << ") found insuff sized songsUVec_, probably from a bug in Setup().");

        return songsUVec_.at(static_cast<std::size_t>(E)).get();
    }

}
}
