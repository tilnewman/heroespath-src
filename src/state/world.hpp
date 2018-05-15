// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STATE_WORLD_HPP_INCLUDED
#define HEROESPATH_STATE_WORLD_HPP_INCLUDED
//
// world.hpp
//  A class that represents the entire state of the game world.
//
#include "map/level-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "state/maps.hpp"

#include <memory>

namespace heroespath
{
namespace state
{

    // Encapsulates all states that describe the game world.
    class World
    {
    public:
        World(const World &) = delete;
        World(World &&) = delete;
        World & operator=(const World &) = delete;
        World & operator=(World &&) = delete;

    public:
        World();

        Maps & GetMaps() { return maps_; }
        std::size_t EncounterCount() const { return encounterCount_; }
        void EncounterCountInc() { ++encounterCount_; }
        void HandleLevelLoad(const map::Level::Enum);
        void HandleLevelUnload(const map::Level::Enum);

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

    private:
        // TODO quests
        // TODO events
        Maps maps_;
        std::size_t encounterCount_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & encounterCount_;
        }
    };

    using WorldUPtr_t = std::unique_ptr<World>;

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_WORLDcd_HPP_INCLUDED
