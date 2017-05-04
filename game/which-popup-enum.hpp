#ifndef GAME_WHICHPOPUPENUM_INCLUDED
#define GAME_WHICHPOPUPENUM_INCLUDED
//
// which-popup-enum.hpp
//  An enum identifying specific popup windows.
//
#include <string>


namespace game
{

    struct Popup
    {
        enum Enum
        {
            Generic = 0,
            ResolutionChange,
            ImageSelection,
            ImageFade,
            ContentSelectionWithItem,
            ContentSelectionWithoutItem,
            CharacterSelection,
            NumberSelection,
            Count
        };

        static const std::string ToString(const Popup::Enum E);
    };

}
#endif //GAME_WHICHPOPUPENUM_INCLUDED
