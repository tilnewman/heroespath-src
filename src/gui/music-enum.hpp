// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MUSICENUM_HPP_INCLUDED
#define HEROESPATH_GUI_MUSICENUM_HPP_INCLUDED
//
// music-enum.hpp
//  An enum defining the various background music files
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    struct music : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
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

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MUSICENUM_HPP_INCLUDED
