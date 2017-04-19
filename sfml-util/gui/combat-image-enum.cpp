//
// combat-image-enum.hpp
//
#include "combat-image-enum.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{
namespace gui
{

    const std::string CombatImageType::ToString(const Enum E)
    {
        switch (E)
        {
            case Wing:      { return "Wing"; }
            case Arrow1:    { return "Arrow1"; }
            case Arrow2:    { return "Arrow2"; }
            case Arrow3:    { return "Arrow3"; }
            case Bolt:      { return "Bolt"; }
            case Dart:      { return "Dart"; }
            case Stone1:    { return "Stone1"; }
            case Stone2:    { return "Stone2"; }
            case Stone3:    { return "Stone3"; }
            case Stone4:    { return "Stone4"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::CombatImageType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string CombatImageType::Filename(const Enum E)
    {
        switch (E)
        {
            case Wing:      { return "wing.png";   }
            case Arrow1:    { return "arrow1.gif"; }
            case Arrow2:    { return "arrow2.gif"; }
            case Arrow3:    { return "arrow3.gif"; }
            case Bolt:      { return "arrow4.gif"; }
            case Dart:      { return "dart.gif";   }
            case Stone1:    { return "stone1.gif"; }
            case Stone2:    { return "stone2.gif"; }
            case Stone3:    { return "stone3.gif"; }
            case Stone4:    { return "stone4.gif"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::CombatImageType::Filename(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
