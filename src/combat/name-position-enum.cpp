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
// name-position-enum.cpp
//
#include "name-position-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace combat
{

    const std::string NamePosition::ToString(const Enum E)
    {
        switch (E)
        {
            case NoName:
            {
                return "NoName";
            }
            case SourceBefore:
            {
                return "SourceBefore";
            }
            case SourceAfter:
            {
                return "SourceAfter";
            }
            case SourceThenTarget:
            {
                return "SourceThenTarget";
            }
            case TargetBefore:
            {
                return "TargetBefore";
            }
            case TargetAfter:
            {
                return "TargetAfter";
            }
            case TargetThenSource:
            {
                return "TargetThenSource";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::NamePosition::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }

    ContentAndNamePos::ContentAndNamePos(
        const std::string & PRE_STR,
        const std::string & CONTENT_STR,
        const std::string & POST_STR,
        const NamePosition::Enum NAME_POS_ENUM)
        : pre_(PRE_STR)
        , content_(CONTENT_STR)
        , post_(POST_STR)
        , posEnum_(NAME_POS_ENUM)
    {}

    ContentAndNamePos::ContentAndNamePos(
        const std::string & CONTENT_STR, const NamePosition::Enum NAME_POS_ENUM)
        : pre_("")
        , content_(CONTENT_STR)
        , post_("")
        , posEnum_(NAME_POS_ENUM)
    {}

    const std::string ContentAndNamePos::Compose(
        const std::string & SOURCE_NAME, const std::string & TARGET_NAME) const
    {
        std::ostringstream ss;
        ss << pre_;

        if ((NamePosition::SourceThenTarget == posEnum_)
            || (NamePosition::SourceBefore == posEnum_))
        {
            ss << SOURCE_NAME;
        }
        else if (
            (NamePosition::TargetThenSource == posEnum_)
            || (NamePosition::TargetBefore == posEnum_))
        {
            ss << TARGET_NAME;
        }

        ss << content_;

        if ((NamePosition::TargetThenSource == posEnum_) || (NamePosition::SourceAfter == posEnum_))
        {
            ss << SOURCE_NAME;
        }
        else if (
            (NamePosition::SourceThenTarget == posEnum_) || (NamePosition::TargetAfter == posEnum_))
        {
            ss << TARGET_NAME;
        }

        ss << post_;

        return ss.str();
    }
}
}
