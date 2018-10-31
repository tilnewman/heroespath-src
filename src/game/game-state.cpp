// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// game-state.cpp
//
#include "creature/creature.hpp"
#include "game-state.hpp"
#include "misc/assertlogandthrow.hpp"

#include <tuple>

namespace heroespath
{
namespace game
{

    GameState::GameState(creature::PlayerPartyUPtr_t PARTY_UPTR, WorldUPtr_t WORLD_UPTR)
        : partyUPtr_(std::move(PARTY_UPTR))
        , worldUPtr_(std::move(WORLD_UPTR))
        , isGameNew_(false)
        , dateTimeStarted_()
        , dateTimeLastSave_()
    {}

    GameState::~GameState() = default;

    World & GameState::GetWorld()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (worldUPtr_), "GameState::GetWorld() called when the worldUptr_ was null.");

        return *worldUPtr_;
    }

    creature::PlayerParty & GameState::Party()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (partyUPtr_), "GameState::GetParty() called when the partyUptr_ was null.");

        return *partyUPtr_;
    }

    void GameState::SetupAsNewGame()
    {
        isGameNew_ = true;
        dateTimeStarted_ = sfml_util::DateTime::CurrentDateTime();
    }

    void GameState::BeforeSerialize()
    {
        partyUPtr_->BeforeSerialize();
        worldUPtr_->BeforeSerialize();
    }

    void GameState::AfterSerialize()
    {
        partyUPtr_->AfterSerialize();
        worldUPtr_->AfterSerialize();
    }

    void GameState::AfterDeserialize()
    {
        worldUPtr_->AfterDeserialize();
        partyUPtr_->AfterDeserialize();
    }

    bool operator<(const GameState & L, const GameState & R)
    {
        return std::tie(
                   L.partyUPtr_,
                   L.worldUPtr_,
                   L.isGameNew_,
                   L.dateTimeStarted_,
                   L.dateTimeLastSave_)
            < std::tie(
                   R.partyUPtr_,
                   R.worldUPtr_,
                   R.isGameNew_,
                   R.dateTimeStarted_,
                   R.dateTimeLastSave_);
    }

    bool operator==(const GameState & L, const GameState & R)
    {
        return std::tie(
                   L.partyUPtr_,
                   L.worldUPtr_,
                   L.isGameNew_,
                   L.dateTimeStarted_,
                   L.dateTimeLastSave_)
            == std::tie(
                   R.partyUPtr_,
                   R.worldUPtr_,
                   R.isGameNew_,
                   R.dateTimeStarted_,
                   R.dateTimeLastSave_);
    }

} // namespace game
} // namespace heroespath
