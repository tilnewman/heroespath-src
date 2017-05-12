#ifndef GAME_PHASEENUM_HPP_INCLUDED
#define GAME_PHASEENUM_HPP_INCLUDED
//
// phase-enum.hpp
//
#include <string>


namespace game
{

    struct Phase
    {
        enum Enum : unsigned int
        {
            Combat          = 1 << 0,
            Exploring       = 1 << 1,
            Conversation    = 1 << 2,
            Quest           = 1 << 3,
            Inventory       = 1 << 4
        };

        static const std::string ToString(const Enum, const bool WILL_WRAP);
        static Enum FromString(const std::string &);
    };

}

#endif //GAME_PHASEENUM_HPP_INCLUDED
