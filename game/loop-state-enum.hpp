#ifndef GAME_LOOPSTATE_INCLUDED
#define GAME_LOOPSTATE_INCLUDED
//
// loop-state-enum.hpp
//  An enum describing the game loop state.
//
#include <string>


namespace game
{

    struct LoopState
    {
        enum Enum
        {
            None = 0,
            Intro,
            MainMenu,
            LoadGameMenu,
            CharacterCreation,
            PartyCreation,
            Adventure,
            Combat,
            Camp,
            Inn,
            Query,
            Popup,
            Credits,
            Settings,
            Exit,
            Inventory,
            Test,
            Count
        };

        static const std::string ToString(const LoopState::Enum E);
        static LoopState::Enum FromString(const std::string & ENUM_STR);
    };

}
#endif //GAME_LOOPSTATE_INCLUDED