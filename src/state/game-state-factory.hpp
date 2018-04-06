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
#ifndef HEROESPATH_STATE_GAMESTATEFACTORY_HPP_INCLUDED
#define HEROESPATH_STATE_GAMESTATEFACTORY_HPP_INCLUDED
//
// game-state-factory.hpp
//  Code that marshals GameState objects on and off disk, and creates new states.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace player
{
    class Party;
    using PartyUPtr_t = std::unique_ptr<Party>;
} // namespace player
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace state
{

    // forward declarations
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
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

    public:
        GameStateFactory();
        ~GameStateFactory();

        static misc::NotNull<GameStateFactory *> Instance();
        static void Acquire();
        static void Release();

        void NewGame(player::PartyUPtr_t) const;

        // Caller is responsible for the lifetime of the returned GameState objects.
        const GameStatePVec_t LoadAllGames() const;

        void SaveGame(const GameStatePtr_t) const;

        // Loaded Characters are not stored in player::CharacterWarehouse,
        // so the caller is responsible for the lifetime of the returned objects.
        const creature::CreaturePVec_t LoadAllUnplayedCharacters() const;

        void SaveCharacter(const creature::CreaturePtr_t) const;
        bool DeleteUnplayedCharacterFile(const creature::CreaturePtr_t) const;

    private:
        // hack'ish function that saves either the game or the character
        // depending on which pointer is null...because can't include the
        // boost serializer includes here in this header file...grumble...zTn 2016-10-26
        void Save(
            const GameStatePtrOpt_t GAMESTATE_PTR_OPT,
            const creature::CreaturePtrOpt_t CHARACTER_PTR_OPT,
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

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_GAMESTATEFACTORY_HPP_INCLUDED
