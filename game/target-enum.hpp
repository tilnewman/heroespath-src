#ifndef GAME_TARGETENUM_HPP_INCLUDED
#define GAME_TARGETENUM_HPP_INCLUDED
//
// target-enum.hpp
//
#include <string>


namespace game
{

    struct TargetType
    {
        enum Enum
        {
            None = 0,
            SingleOpponent,
            AllOpponents,
            SingleCompanion,
            AllCompanions,
            Item,
            QuestSpecific,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
        static const std::string ActionPhrase(const Enum);
    };

}

#endif //GAME_TARGETENUM_HPP_INCLUDED
