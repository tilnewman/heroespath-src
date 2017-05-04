//
// which-popup-enum.cpp
//
#include "which-popup-enum.hpp"
#include <exception>
#include <sstream>


namespace game
{

    const std::string Popup::ToString(const Popup::Enum E)
    {
        switch (E)
        {
            case Generic:                       { return "Generic"; }
            case ResolutionChange:              { return "ResolutionChange"; }
            case ImageSelection:                { return "ImageSelection"; }
            case ImageFade:                     { return "ImageFade"; }
            case ContentSelectionWithItem:      { return "ContentSelectionWithItem"; }
            case ContentSelectionWithoutItem:   { return "ContentSelectionWithoutItem"; }
            case CharacterSelection:            { return "CharacterSelection"; }
            case NumberSelection:               { return "NumberSelection"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::Popup::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
