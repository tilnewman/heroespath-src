// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_COMPLEXITY_TYPE_HPP_INCLUDED
#define HEROESPATH_CREATURE_COMPLEXITY_TYPE_HPP_INCLUDED
//
// complexity-type.hpp
//
#include "misc/enum-common.hpp"
#include "misc/not-null.hpp"

namespace heroespath
{
namespace creature
{

    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    namespace nonplayer
    {

        // define the complexity of items owned by the creature
        struct complexity_type : public EnumBaseCounting<>
        {
            enum Enum : EnumUnderlying_t
            {
                //...nothing
                Animal = 0,

                // shortbow/longbow/blowpipe, rings,
                // leather/stone/bone/obsidian/lapis/horn/hide/tooth materials only
                Simple,

                // composite bow, medallion, scale material, iron/steel/tin metals only
                Moderate,

                // crossbow, lantern, necklace, work with all metals
                Complex,

                Count
            };

            static complexity_type::Enum FromCreature(const CreaturePtr_t & CHARACTER_PTR);
        };

    } // namespace nonplayer
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_COMPLEXITY_TYPE_HPP_INCLUDED
