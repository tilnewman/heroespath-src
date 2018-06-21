// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// music-enum.cpp
//
#include "music-enum.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::string music::ToString(const music::Enum E)
    {
        switch (E)
        {
            case None:
            {
                return "None";
            }
            case Theme:
            {
                return "theme-violin";
            }
            case Wind:
            {
                return "wind";
            }
            case FireIndoorSmall:
            {
                return "fire-indoor-small";
            }
            case FireIndoorLarge:
            {
                return "fire-indoor-large";
            }
            case FireOutdoor1:
            {
                return "fire-outdoor-1";
            }
            case FireOutdoor2:
            {
                return "fire-outdoor-2";
            }
            case CombatIntro:
            {
                return "combatintro";
            }
            case PartyCreation:
            {
                return "party-creation-background";
            }
            case Credits:
            {
                return "credits";
            }
            case Inventory:
            {
                return "inventory";
            }
            case All:
            {
                return "All";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string music::Filename(const music::Enum E)
    {
        std::ostringstream ss;
        ss << ToString(E) << FileExt();
        return ss.str();
    }

    const std::string music::Directory(const music::Enum E)
    {
        switch (E)
        {
            case Theme:
            {
                return "theme";
            }
            case Wind:
            {
                return "wind";
            }
            case FireIndoorSmall:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:
            {
                return "fire";
            }
            case CombatIntro:
            {
                return "combat-intro";
            }
            case PartyCreation:
            {
                return "party-creation";
            }
            case Credits:
            {
                return "credits";
            }
            case Inventory:
            {
                return "inventory";
            }
            case All:
            {
                ThrowInvalidValueForFunction(E, "Directory(All)");
            }
            case None:
            {
                ThrowInvalidValueForFunction(E, "Directory(None)");
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "Directory");
            }
        }
    }

    bool music::IsLooped(const music::Enum E)
    {
        switch (E)
        {
            case Theme:
            case Wind:
            case FireIndoorSmall:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:
            case PartyCreation:
            case Credits:
            case Inventory:
            {
                return true;
            }
            case CombatIntro:
            {
                return false;
            }
            case All:
            {
                ThrowInvalidValueForFunction(E, "IsLooped(All)");
            }
            case None:
            {
                ThrowInvalidValueForFunction(E, "IsLooped(None)");
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "IsLooped");
            }
        }
    }

    const std::string music::ArtistName(const music::Enum E)
    {
        switch (E)
        {
            case Wind:
            {
                return "Luke @RUST LTD";
            }
            case FireIndoorSmall:
            {
                return "Inchadney";
            }
            case Theme:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:
            {
                return "(unknown)";
            }
            case CombatIntro:
            {
                return "(various)";
            }
            case Credits:
            case Inventory:
            {
                return "Janne Hanhisuanto";
            }
            case PartyCreation:
            {
                return "Marcelo Fernandez";
            }
            case All:
            {
                ThrowInvalidValueForFunction(E, "ArtistName(All)");
            }
            case None:
            {
                ThrowInvalidValueForFunction(E, "ArtistName(None)");
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ArtistName");
            }
        }
    }

    const std::string music::LicenseTitle(const music::Enum E)
    {
        switch (E)
        {
            case FireIndoorSmall:
            {
                return "CC-BY 3.0";
            }
            case FireIndoorLarge:
            {
                return "Inchadney";
            }
            case Theme:
            case Wind:
            case FireOutdoor1:
            case FireOutdoor2:
            {
                return "CC0 1.0";
            }
            case CombatIntro:
            {
                return "(various)";
            }
            case Credits:
            case Inventory:
            {
                return "CC-BY-SA 3.0";
            }
            case PartyCreation:
            {
                return "CC-BY 4.0";
            }
            case All:
            {
                ThrowInvalidValueForFunction(E, "LicenseTitle(All)");
            }
            case None:
            {
                ThrowInvalidValueForFunction(E, "LicenseTitle(None)");
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "LicenseTitle");
            }
        }
    }

    const std::string music::SongName(const music::Enum E)
    {
        switch (E)
        {
            case Theme:
            {
                return "Heroes' Path Theme";
            }
            case Wind:
            {
                return "Wind";
            }
            case FireIndoorSmall:
            {
                return "Fireplace Small";
            }
            case FireIndoorLarge:
            {
                return "Fireplace Large";
            }
            case FireOutdoor1:
            {
                return "Campfire1";
            }
            case FireOutdoor2:
            {
                return "Campfire2";
            }
            case CombatIntro:
            {
                return "(various)";
            }
            case PartyCreation:
            {
                return "Intro of Dragons";
            }
            case Credits:
            {
                return "Radakan Menu";
            }
            case Inventory:
            {
                return "PYC";
            }
            case All:
            {
                ThrowInvalidValueForFunction(E, "SongName(All)");
            }
            case None:
            {
                ThrowInvalidValueForFunction(E, "SongName(None)");
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "SongName");
            }
        }
    }

} // namespace sfml_util
} // namespace heroespath
