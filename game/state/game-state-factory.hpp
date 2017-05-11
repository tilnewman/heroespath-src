#ifndef GAME_STATE_GAMESTATEFACTORY_INCLUDED
#define GAME_STATE_GAMESTATEFACTORY_INCLUDED
//
// game-state-factory.hpp
//  Code that marshals GameState objects on and off disk, and creates new states.
//
#include "utilz/boost-serialize-includes.hpp"

#include <set>
#include <string>
#include <memory>
#include <sstream>


namespace game
{
    namespace player
    {
        class Party;
        using PartySPtr_t = std::shared_ptr<Party>;
        using PartySSet_t = std::set<PartySPtr_t>;

        class Character;
        using CharacterSPtr_t = std::shared_ptr<Character>;
        using CharacterSSet_t = std::set<CharacterSPtr_t>;
    }

namespace state
{

    //forward declarations
    class GameState;
    using GameStateSPtr_t = std::shared_ptr<GameState>;
    using GameStateSSet_t = std::set<GameStateSPtr_t>;


    //types required by singleton implementation
    class GameStateFactory;
    using GameStateFactorySPtr_t = std::shared_ptr<GameStateFactory>;


    //Creates game states either new or from a previous save to disc.
    class GameStateFactory
    {
        //prevent all but singleton construction
        GameStateFactory();

        //prevent copy construction
        GameStateFactory(const GameStateFactory &) =delete;

        //prevent copy assignment
        GameStateFactory & operator=(const GameStateFactory &) =delete;

    public:
        static GameStateFactorySPtr_t Instance();
        virtual ~GameStateFactory();

        void NewGame(const player::PartySPtr_t & PARTY_SPTR) const;
        GameStateSSet_t LoadAllGames() const;
        void SaveGame(const GameStateSPtr_t & GAME_SPTR) const;

        player::CharacterSSet_t LoadAllCharacters() const;
        void SaveCharacter(const player::CharacterSPtr_t &) const;
        bool DeleteCharacter(const player::CharacterSPtr_t &) const;

    private:
        //hack'ish function that saves either the game or the character
        //depending on which pointer is null...because can't include the
        //boost serializer includes here in this header file...grumble...zTn 2016-10-26
        void Save(const GameStateSPtr_t &         GAME_SPTR,
                  const player::CharacterSPtr_t & CHARACTER_SPTR,
                  const std::string &             DIR_STR,
                  const std::string &             FILE_STR,
                  const std::string &             EXT_STR) const;

        static const std::string SAVED_GAME_DIR_NAME_;
        static const std::string SAVED_GAME_FILE_NAME_;
        static const std::string SAVED_GAME_FILE_EXT_;
        //
        static const std::string SAVED_CHAR_DIR_NAME_;
        static const std::string SAVED_CHAR_FILE_EXT_;
        //
        static GameStateFactorySPtr_t instanceSPtr_;
    };

}
}
#endif //GAME_STATE_GAMESTATEFACTORY_INCLUDED
