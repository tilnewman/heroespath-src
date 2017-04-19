#ifndef APPBASE_HEROESPATH_WHICHPOPUPENUM_INCLUDED
#define APPBASE_HEROESPATH_WHICHPOPUPENUM_INCLUDED
//
// which-popup-enum.hpp
//  An enum identifying specific popup windows.
//
#include <string>


namespace heroespath
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
#endif //APPBASE_HEROESPATH_WHICHPOPUPENUM_INCLUDED
