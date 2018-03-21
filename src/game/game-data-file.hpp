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
#ifndef HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
#define HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
//
// game-data-file.hpp
//  A singleton config file that contains game data
//
#include "config/configbase.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/type_index.hpp> //for boost::typeindex::type_id<T>().pretty_name()

#include <memory>
#include <string>

namespace heroespath
{
namespace game
{

    // Singleton class that manages a simple configuration file
    class GameDataFile : public config::ConfigBase
    {
    public:
        GameDataFile(const GameDataFile &) = delete;
        GameDataFile(GameDataFile &&) = delete;
        GameDataFile & operator=(const GameDataFile &) = delete;
        GameDataFile & operator=(GameDataFile &&) = delete;

        GameDataFile();
        virtual ~GameDataFile() = default;
        static GameDataFile * Instance();
        static void Acquire();
        static void Release();

        const std::string GetMediaPath(const std::string & KEY) const;
        const std::string CreateMediaPath(const std::string & PATH) const;
        const std::string GetMediaBasePathStr() const;

    private:
        static std::unique_ptr<GameDataFile> instanceUPtr_;
    };
} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
