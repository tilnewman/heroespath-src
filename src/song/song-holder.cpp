// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// song-holder.cpp
//
#include "song-holder.hpp"

#include "misc/assertlogandthrow.hpp"
#include "song/song.hpp"
#include "stage/i-stage.hpp"

#include <memory>

namespace heroespath
{
namespace song
{

    SongUVec_t Holder::songsUVec_;

    void Holder::Fill()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (songsUVec_.empty()), "song::Holder::Setup() was called twice.");

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

    bool Holder::Test(stage::IStagePtr_t iStagePtr)
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            iStagePtr->TestingStrAppend("song::Holder::Test() Starting Tests...");
        }

        static std::size_t songIndex { 0 };
        if (songIndex < Songs::Count)
        {
            const auto NEXT_ENUM { static_cast<Songs::Enum>(songIndex) };
            const auto SONG_PTR { Get(NEXT_ENUM) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->Name().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty Name().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->Desc().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty Desc().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->DescExtra().empty() == false),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in an empty DescExtra().");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->ManaCost().IsNonZero()),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in a zero Mana cost.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->Rank().IsNonZero()),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") resulted in a zero Rank.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SONG_PTR->Name() == Songs::Name(NEXT_ENUM)),
                "song::Holder::Test(\"" << Songs::ToString(NEXT_ENUM)
                                        << "\") Song is out of order.");

            ++songIndex;
            iStagePtr->TestingStrIncrement("Song Test \"" + SONG_PTR->Name() + "\"");

            return false;
        }

        iStagePtr->TestingStrAppend("song::Holder::Test()  ALL TESTS PASSED.");
        return true;
    }

    const SongPtr_t Holder::Get(const Songs::Enum E)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (songsUVec_.empty() == false),
            "song::Holder::Get(" << Songs::ToString(E)
                                 << ") was called when the holder was empty.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (Songs::IsValid(E)), "song::Holder::Get(" << E << ") but that value is invalid.");

        const auto INDEX { static_cast<std::size_t>(E) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < songsUVec_.size()),
            "song::Holder::Get(" << Songs::ToString(
                E) << ") found insuff sized songsUVec_, probably from a bug in Setup().");

        return SongPtr_t(songsUVec_[INDEX].get());
    }

} // namespace song
} // namespace heroespath
