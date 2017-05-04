//
// sex-enum.hpp
//
#include "sex-enum.hpp"
#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string sex::ToString(const sex::Enum E)
    {
        switch (E)
        {
            case Unknown:   { return "Unknown"; }
            case Male:      { return "Male"; }
            case Female:    { return "Female"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::sex::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool sex::IsValid(const sex::Enum E)
    {
        switch (E)
        {
            case Unknown:
            case Male:
            case Female: { return true; }
            case Count:
            default: { return false; }
        }
    }


    const std::string sex::HeSheIt(const sex::Enum E, const bool WILL_CAPITALIZE)
    {
        switch (E)
        {
            case Unknown:   { if (WILL_CAPITALIZE) return "It";  else return "it"; }
            case Male:      { if (WILL_CAPITALIZE) return "He";  else return "he"; }
            case Female:    { if (WILL_CAPITALIZE) return "She"; else return "she"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::sex::Enum::HisHersIts(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string sex::HisHerIts(const sex::Enum E,
                                const bool      WILL_CAPITALIZE,
                                const bool      WILL_POSSESSIVE_HER)
    {
        switch (E)
        {
            case Unknown:   { if (WILL_CAPITALIZE) return "Its";  else return "its"; }
            case Male:      { if (WILL_CAPITALIZE) return "His"; else return "his"; }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return std::string("Her").append((WILL_POSSESSIVE_HER) ? "s" : "");
                else
                    return std::string("her").append((WILL_POSSESSIVE_HER) ? "s" : "");
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::sex::Enum::HisHersIts(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string sex::HimHerIt(const sex::Enum E, const bool WILL_CAPITALIZE)
    {
        switch (E)
        {
            case Unknown:   { if (WILL_CAPITALIZE) return "It";  else return "it"; }
            case Male:      { if (WILL_CAPITALIZE) return "Him"; else return "him"; }
            case Female:    { if (WILL_CAPITALIZE) return "Her"; else return "her"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::sex::Enum::HimHerIt(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
