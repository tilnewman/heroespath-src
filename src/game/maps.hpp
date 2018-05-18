// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_MAPS_HPP_INCLUDED
#define HEROESPATH_GAME_MAPS_HPP_INCLUDED
//
// maps.hpp
//
#include "game/level.hpp"
#include "map/level-enum.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace game
{

    // Responsible for storing all states that represent the game world contained in maps.
    class Maps
    {
    public:
        Maps();

        Level & Current() { return levels_.at(static_cast<std::size_t>(level_)); }

        void SetupForNewGame();
        void HandleLevelLoad(const map::Level::Enum);
        void HandleLevelUnload(const map::Level::Enum);

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

    private:
        map::Level::Enum level_;
        std::vector<Level> levels_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & level_;
            ar & levels_;
        }
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_MAPS_HPP_INCLUDED
