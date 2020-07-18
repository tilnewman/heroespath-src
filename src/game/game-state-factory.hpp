// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_GAME_STATE_FACTORY_HPP_INCLUDED
#define HEROESPATH_GAME_GAME_STATE_FACTORY_HPP_INCLUDED
//
// game-state-factory.hpp
//  Code that marshals GameState objects on and off disk, and creates new states.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class PlayerParty;
    using PlayerPartyUPtr_t = std::unique_ptr<PlayerParty>;

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace game
{

    // forward declarations
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
    using GameStateUPtr_t = std::unique_ptr<GameState>;
    using GameStatePtrOpt_t = boost::optional<GameStatePtr_t>;
    using GameStatePVec_t = std::vector<GameStatePtr_t>;

    // Creates game states either new or from a previous save to disc.
    class GameStateFactory
    {
    public:
        GameStateFactory(const GameStateFactory &) = delete;
        GameStateFactory(GameStateFactory &&) = delete;
        GameStateFactory & operator=(const GameStateFactory &) = delete;
        GameStateFactory & operator=(GameStateFactory &&) = delete;

        GameStateFactory();
        ~GameStateFactory();

        static misc::NotNull<GameStateFactory *> Instance();
        static void Create();
        static void Destroy();

        GameStateUPtr_t MakeForNewGame(creature::PlayerPartyUPtr_t PARTY_UPTR) const;
        GameStateUPtr_t MakeForNewGameForTesting() const;

        // Caller is responsible for the lifetime of the returned GameState objects.
        const GameStatePVec_t LoadAllGames() const;

        void SaveGame(const GameStatePtr_t) const;

        // Loaded Characters are created by CreatureFactory and are therefore already stored in
        // player::CreatureWarehouse, so the caller is responsible for calling
        // CreatureWarehouse::Free().
        const creature::CreaturePVec_t LoadAllUnplayedCharacters() const;

        void SaveCharacter(const creature::CreaturePtr_t) const;

        void DeleteUnplayedCharacterFile(const creature::CreaturePtr_t) const;

    private:
        // hack'ish function that saves either the game or the character
        // depending on which pointer is null...because can't include the
        // boost serializer includes here in this header file...grumble...zTn 2016-10-26
        void Save(
            const GameStatePtrOpt_t & GAMESTATE_PTR_OPT,
            const creature::CreaturePtrOpt_t & CHARACTER_PTR_OPT,
            const std::string & DIR_STR,
            const std::string & FILE_STR,
            const std::string & EXT_STR) const;

        static const std::string SAVED_HEROESPATH_DIR_NAME_;
        static const std::string SAVED_HEROESPATH_FILE_NAME_;
        static const std::string SAVED_HEROESPATH_FILE_EXT_;
        //
        static const std::string UNPLAYED_CHAR_DIR_NAME_;
        static const std::string UNPLAYED_CHAR_FILE_EXT_;
        //
        static std::unique_ptr<GameStateFactory> instanceUPtr_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_GAME_STATE_FACTORY_HPP_INCLUDED
