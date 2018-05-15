// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
#define HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
//
// game-data-file.hpp
//  A Subsystem config file that contains game data
//
#include "config/configbase.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/not-null.hpp"

#include <boost/type_index.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace game
{

    // Subsystem class that manages a simple configuration file
    class GameDataFile : public config::ConfigBase
    {
    public:
        GameDataFile(const GameDataFile &) = delete;
        GameDataFile(GameDataFile &&) = delete;
        GameDataFile & operator=(const GameDataFile &) = delete;
        GameDataFile & operator=(GameDataFile &&) = delete;

        GameDataFile();
        virtual ~GameDataFile() = default;

        static misc::NotNull<GameDataFile *> Instance();
        static void Acquire();
        static void Release();

        void Initialize();
        const std::string GetMediaPath(const std::string & KEY) const;

    private:
        std::string mediaBasePathKeyStr_;
        std::string mediaBasePathStr_;
        static std::unique_ptr<GameDataFile> instanceUPtr_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_GAMEDATAFILE_HPP_INCLUDED
