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
// song-enum.cpp
//
#include "song-enum.hpp"

#include "game/game-data-file.hpp"
#include "misc/boost-string-includes.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace song
{

    const std::string Songs::ToString(const Enum E)
    {
        switch (E)
        {
            case RallyDrum:
            {
                return "RallyDrum";
            }
            case SpiritResonance:
            {
                return "SpiritResonance";
            }
            case RousingRhythm:
            {
                return "RousingRhythm";
            }
            case TripBeat:
            {
                return "TripBeat";
            }
            case PanicStrings:
            {
                return "PanicStrings";
            }
            case Lullaby:
            {
                return "Lullaby";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "song::Songs::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Songs::Name(const Enum E)
    {
        switch (E)
        {
            case RallyDrum:
            {
                return "Rally Drum";
            }
            case SpiritResonance:
            {
                return "Spirit Resonance";
            }
            case RousingRhythm:
            {
                return "Rousing Rhythm";
            }
            case TripBeat:
            {
                return "Tri pBeat";
            }
            case PanicStrings:
            {
                return "Panic Strings";
            }
            case Lullaby:
            {
                return "Lullaby";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "song::Songs::Name(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    const std::string Songs::ShortDesc(const Songs::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(E) << "-short-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Songs::ExtraDesc(const Songs::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-song-" << ToString(E) << "-extra-desc";
        return game::GameDataFile::Instance()->GetCopyStr(keySS.str());
    }

    const std::string Songs::ImageFilename(const Songs::Enum E)
    {
        return boost::algorithm::to_lower_copy(ToString(E) + ".png");
    }
} // namespace song
} // namespace heroespath
