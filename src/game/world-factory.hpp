// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_WORLD_FACTORY_HPP_INCLUDED
#define HEROESPATH_GAME_WORLD_FACTORY_HPP_INCLUDED
//
// world-factory.hpp
//
#include <memory>

namespace heroespath
{
namespace game
{

    class World;
    using WorldUPtr_t = std::unique_ptr<World>;

    class Level;

    // Responsible for making World objects, specifically new World objects for new games.
    struct WorldFactory
    {
        static WorldUPtr_t MakeForNewGame();
        static void SetupLevelForNewGame(Level &);
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_WORLD_FACTORY_HPP_INCLUDED
