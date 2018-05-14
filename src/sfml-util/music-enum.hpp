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
#ifndef HEROESPATH_SFMLUTIL_MUSICENUM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MUSICENUM_HPP_INCLUDED
//
// music-enum.hpp
//  An enum defining the various background music files
//
#include "misc/enum-util.hpp"

#include <set>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    struct Footstep : public misc::EnumBaseCounting<Footstep, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Grass = 0,
            Gravel,
            Leaves,
            Solid,
            Wood,
            Count
        };

        static const std::string ToString(const Enum);
    };

    struct music : public misc::EnumBaseCounting<music, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Theme = 0,
            Wind,
            FootstepGrass,
            FootstepGravel,
            FootstepLeaves,
            FootstepSolid,
            FootstepWood,
            FireIndoorSmall,
            FireIndoorLarge,
            FireOutdoor1,
            FireOutdoor2,
            CombatIntro,
            PartyCreation,
            Credits,
            Inventory,
            Count,
            None,
            All
        };

        static const std::string FileExt() { return ".ogg"; }

        static const std::string ToString(const music::Enum);
        static const std::string Filename(const music::Enum);
        static const std::string Directory(const music::Enum);
        static const std::string ArtistName(const music::Enum);
        static const std::string LicenseTitle(const music::Enum);
        static const std::string SongName(const music::Enum);
        static bool IsLooped(const music::Enum);
        static music::Enum FootstepToMusic(const Footstep::Enum);
    };

    using MusicEnumVec_t = std::vector<music::Enum>;
    using MusicEnumSet_t = std::set<music::Enum>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MUSICENUM_HPP_INCLUDED
