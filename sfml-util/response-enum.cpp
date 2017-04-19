//
// response-enum.cpp
//
#include "response-enum.hpp"
#include <exception>
#include <sstream>


namespace sfml_util
{

    const std::string Response::ToString(const Response::Enum E)
    {
        switch (E)
        {
            case Okay:     { return "Okay";     }
            case Continue: { return "Continue"; }
            case Yes:      { return "Yes";      }
            case No:       { return "No";       }
            case Cancel:   { return "Cancel";   }
            case Select:   { return "Select";   }
            case Error:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Response::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Response::IsValid(const Response::Enum E)
    {
        const unsigned MAX(None | Okay | Continue | Yes | No | Cancel | Select);
        return (static_cast<unsigned>(E) <= MAX);
    }


    bool Response::IsAffirmative(const Response::Enum E)
    {
        switch (E)
        {
            case Okay:
            case Continue:
            case Yes:
            case Select:    { return true;  }
            case No:
            case Cancel:    { return false; }
            case Error:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Response::IsAffirmative(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
