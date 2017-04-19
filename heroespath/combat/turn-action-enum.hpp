#ifndef HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
//
// turn-action-enum.hpp
//
#include <string>


namespace heroespath
{
namespace combat
{

    struct TurnAction
    {
        enum Enum
        {
            Nothing = 0,
            Attack,
            Block,
            Advance,
            Retreat,
            Fly,
            Land,
            Cast,
            SkyPounce,
            LandPounce,
            Roar,
            ChangeWeapon,
            Count
        };

        static const std::string ToString(const TurnAction::Enum);
        static const std::string Name(const TurnAction::Enum);
        inline static bool IsMove(const Enum E) { return ((E == Advance) || (E == Retreat)); }
    };

}
}

#endif //HEROESPATH_COMBAT_TURNSTATE_ENUM_HPP_INCLUDED
