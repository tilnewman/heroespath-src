// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

    struct music : public misc::EnumBaseCounting<music, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Theme = 0,
            Wind,
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
    };

    using MusicEnumVec_t = std::vector<music::Enum>;
    using MusicEnumSet_t = std::set<music::Enum>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MUSICENUM_HPP_INCLUDED
