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
#include "misc/strings.hpp"

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

        static constexpr std::string_view FileExt { ".ogg" };

        static inline const std::string Filename(const music::Enum MUSIC)
        {
            std::string str(NAMEOF_ENUM(MUSIC));
            misc::ToLower(str);
            str += FileExt;
            return str;
        }

        static constexpr std::string_view Directory(const music::Enum MUSIC) noexcept
        {
            switch (MUSIC)
            {
                case Theme: return "theme";
                case Wind: return "wind";

                case FireIndoorSmall:
                case FireIndoorLarge:
                case FireOutdoor1:
                case FireOutdoor2: return "fire";

                case CombatIntro: return "combat-intro";
                case PartyCreation: return "party-creation";
                case Credits: return "credits";
                case Inventory: return "inventory";
                case All: return "All";
                case None: return "";
                case Count: return "Count";
                default: return "gui::music::Directory(ENUM=out_of_range)";
            }
        }

        static constexpr std::string_view ArtistName(const music::Enum MUSIC) noexcept
        {
            switch (MUSIC)
            {
                case Wind: return "Luke @RUST LTD";
                case FireIndoorSmall: return "Inchadney";

                case Theme:
                case FireIndoorLarge:
                case FireOutdoor1:
                case FireOutdoor2: return "(unknown)";

                case CombatIntro: return "(various)";

                case Credits:
                case Inventory: return "Janne Hanhisuanto";

                case PartyCreation: return "Marcelo Fernandez";

                case All:
                case None:
                case Count:
                default: return "gui::music::ArtistName(ENUM=out_of_range)";
            }
        }

        static constexpr std::string_view LicenseTitle(const music::Enum MUSIC) noexcept
        {
            switch (MUSIC)
            {
                case FireIndoorSmall: return "CC-BY 3.0";
                case FireIndoorLarge: return "Inchadney";

                case Theme:
                case Wind:
                case FireOutdoor1:
                case FireOutdoor2: return "CC0 1.0";

                case CombatIntro: return "(various)";

                case Credits:
                case Inventory: return "CC-BY-SA 3.0";

                case PartyCreation: return "CC-BY 4.0";

                case All:
                case None:
                case Count: return "";

                default: return "gui::music::LicenseTitle(ENUM=out_of_range)";
            }
        }

        static constexpr std::string_view SongName(const music::Enum MUSIC) noexcept
        {
            switch (MUSIC)
            {
                case Theme: return "Heroes' Path Theme";
                case Wind: return "Wind";
                case FireIndoorSmall: return "Fireplace Small";
                case FireIndoorLarge: return "Fireplace Large";
                case FireOutdoor1: return "Campfire1";
                case FireOutdoor2: return "Campfire2";
                case CombatIntro: return "(various)";
                case PartyCreation: return "Intro of Dragons";
                case Credits: return "Radakan Menu";
                case Inventory: return "PYC";
                case Count: return "Count";
                case None: return "None";
                case All: return "All";
                default: return "gui::music::SongName(ENUM=out_of_range)";
            }
        }

        static constexpr bool IsLooped(const music::Enum MUSIC) noexcept
        {
            switch (MUSIC)
            {
                case Theme:
                case Wind:
                case FireIndoorSmall:
                case FireIndoorLarge:
                case FireOutdoor1:
                case FireOutdoor2:
                case PartyCreation:
                case Credits:
                case Inventory: return true;

                case CombatIntro:
                case All:
                case None:
                case Count:
                default: return false;
            }
        }
    };

    using MusicEnumVec_t = std::vector<music::Enum>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MUSICENUM_HPP_INCLUDED
