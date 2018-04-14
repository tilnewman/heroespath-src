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
// song-holder.cpp
//
#include "song-holder.hpp"

#include "game/loop-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "song/song.hpp"

#include <memory>

namespace heroespath
{
namespace song
{

    SongUVec_t Holder::songsUVec_;

    void Holder::Fill()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((songsUVec_.empty()), "song::Holder::Setup() was called twice.");

        // Note::Keep order in sync with song::Songs::Enum
        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::RallyDrum,
            SongType::Drum,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
            1_mana,
            1_rank,
            combat::TargetType::AllCompanions,
            "emboldens",
            "emboldened"));

        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::SpiritResonance,
            SongType::Guitar,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
            5_mana,
            1_rank,
            combat::TargetType::AllCompanions,
            "recharges",
            "recharged"));

        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::RousingRhythm,
            SongType::Drum,
            combat::EffectType::CreatureHelpMisc,
            static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
            3_mana,
            1_rank,
            combat::TargetType::AllCompanions,
            "rouses",
            "roused"));

        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::TripBeat,
            SongType::Drum,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            1_mana,
            1_rank,
            combat::TargetType::AllOpponents,
            "trips",
            "tripped"));

        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::PanicStrings,
            SongType::Guitar,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            2_mana,
            1_rank,
            combat::TargetType::AllOpponents,
            "panics",
            "panicked"));

        songsUVec_.emplace_back(std::make_unique<Song>(
            Songs::Lullaby,
            SongType::Guitar,
            combat::EffectType::CreatureHarmMisc,
            game::Phase::Combat,
            2_mana,
            1_rank,
            combat::TargetType::AllOpponents,
            "sleeps",
            "slept"));
    }

    void Holder::Empty() { songsUVec_.clear(); }

    bool Holder::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "song::Holder::Test() Starting Tests...");
        }

        static auto songIndex{ 0 };
        if (songIndex < Songs::Count)
        {
            auto const NEXT_ENUM{ static_cast<Songs::Enum>(songIndex) };
            auto const SONG_PTR{ Get(NEXT_ENUM) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->Name().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty Name().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->Desc().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty Desc().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->DescExtra().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty DescExtra().");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->ManaCost().IsNonZero()),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in a zero Mana cost.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->Rank().IsNonZero()),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in a zero Rank.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (SONG_PTR->Name() == Songs::Name(NEXT_ENUM)),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") Song is out of order.");

            ++songIndex;
            game::LoopManager::Instance()->TestingStrIncrement(
                "Song Test \"" + SONG_PTR->Name() + "\"");

            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend("song::Holder::Test()  ALL TESTS PASSED.");

        return true;
    }

    const SongPtr_t Holder::Get(const Songs::Enum E)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (songsUVec_.empty() == false),
            "song::Holder::Get(" << Songs::ToString(E)
                                 << ") was called when the holder was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (static_cast<std::size_t>(E) < songsUVec_.size()),
            "song::Holder::Get("
                << Songs::ToString(E)
                << ") found insuff sized songsUVec_, probably from a bug in Setup().");

        return songsUVec_.at(static_cast<std::size_t>(E)).get();
    }

} // namespace song
} // namespace heroespath
