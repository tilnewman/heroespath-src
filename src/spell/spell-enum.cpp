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
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "game/game-data-file.hpp"

#include "misc/boost-string-includes.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace spell
{

    const std::string Spells::ToString(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "ClearMind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "PoisonCloud";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::spell::Spells::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Spells::Name(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "Clear Mind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "Poison Cloud";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "creature::spell::Spells::Name(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Spells::ShortDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-short-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Spells::ExtraDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-extra-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Spells::ImageFilename(const Spells::Enum E)
    {
        return boost::algorithm::to_lower_copy(ToString(E) + ".png");
    }
} // namespace spell
} // namespace heroespath
