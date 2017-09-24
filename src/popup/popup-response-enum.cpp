// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// popup-response-enum.cpp
//
#include "popup-response-enum.hpp"

#include <exception>
#include <sstream>


namespace popup
{

    const std::string ResponseTypes::ToString(const ResponseTypes::Enum E)
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
                ss << "popup::ResponseTypes::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool ResponseTypes::IsValid(const ResponseTypes::Enum E)
    {
        const unsigned MAX(None | Okay | Continue | Yes | No | Cancel | Select);
        return (static_cast<unsigned>(E) <= MAX);
    }


    bool ResponseTypes::IsAffirmative(const ResponseTypes::Enum E)
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
                ss << "popup::ResponseTypes::IsAffirmative(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
