#ifndef GAME_ITEM_TYPEDEFS_INCLUDED
#define GAME_ITEM_TYPEDEFS_INCLUDED
//
// types.hpp
//  A collection of typedefs and constants related to items and inventory.
//
#include <cstddef> //for std::size_t


namespace game
{
namespace item
{

    using Coin_t   = int;
    using Gem_t    = int;
    using Weight_t = std::size_t;

    //TODO TEMP REMOVE -remove this when finished removing meteor shards from the game
    using Meteor_t = int;
}
}
#endif //GAME_ITEM_TYPEDEFS_INCLUDED
