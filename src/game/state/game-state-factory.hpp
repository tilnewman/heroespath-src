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
#ifndef GAME_STATE_GAMESTATEFACTORY_INCLUDED
#define GAME_STATE_GAMESTATEFACTORY_INCLUDED
//
// game-state-factory.hpp
//  Code that marshals GameState objects on and off disk, and creates new states.
//
#include "misc/boost-serialize-includes.hpp"

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
        static GameStateFactory * Instance();
        static void Acquire();
        static void Release();

        void NewGame(const player::PartySPtr_t & PARTY_SPTR) const;
        GameStateSSet_t LoadAllGames() const;
        void SaveGame(const GameStateSPtr_t & GAME_SPTR) const;

        player::CharacterSSet_t LoadAllCompanions() const;
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
        static std::unique_ptr<GameStateFactory> instanceUPtr_;
    };

}
}
#endif //GAME_STATE_GAMESTATEFACTORY_INCLUDED
