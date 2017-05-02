#ifndef APPBASE_HEROESPATH_STATE_WORLDSTATE_INCLUDED
#define APPBASE_HEROESPATH_STATE_WORLDSTATE_INCLUDED
//
// world-state.hpp
//  A class that represents the entire state of the game world.
//
#include "utilz/boost-serialize-includes.hpp"
#include <memory>


namespace heroespath
{
namespace state
{

    //Encapsulates everything about the game world.
    class WorldState
    {
        //prevent copy construction
        WorldState(const WorldState &);

        //prevent copy assignment
        WorldState & operator=(const WorldState &);

    public:
        WorldState();
        virtual ~WorldState();

        inline std::size_t EncounterCount() const   { return encounterCount_; }
        inline void EncoundterCountInc()            { ++encounterCount_; }

    private:
        //TODO quests
        //TODO events
        //TODO map states
        std::size_t encounterCount_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & encounterCount_;
        }
    };

    using WorldStateSPtr_t = std::shared_ptr<WorldState>;

}
}
#endif //APPBASE_HEROESPATH_STATE_WORLDSTATE_INCLUDED
