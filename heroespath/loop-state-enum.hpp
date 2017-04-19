#ifndef APPBASE_HEROESPATH_LOOPSTATE_INCLUDED
#define APPBASE_HEROESPATH_LOOPSTATE_INCLUDED
//
// loop-state-enum.hpp
//  An enum describing the game loop state.
//
#include <string>


namespace heroespath
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
#endif //APPBASE_HEROESPATH_LOOPSTATE_INCLUDED
