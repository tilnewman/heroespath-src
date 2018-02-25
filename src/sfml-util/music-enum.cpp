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
// music-enum.cpp
//
#include "music-enum.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace heroespath
{
namespace sfml_util
{

    const std::string Footstep::ToString(const Footstep::Enum E)
    {
        switch (E)
        {
            case Grass:     { return "Grass"; }
            case Gravel:    { return "Gravel"; }
            case Leaves:    { return "Leaves"; }
            case Solid:     { return "Solid"; }
            case Wood:      { return "Wood"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::Footstep::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    Footstep::Enum Footstep::FromString(const std::string & NAME_ORIG)
    {
        namespace ba = boost::algorithm;

        auto const NAME_LOWER{ ba::to_lower_copy(NAME_ORIG) };

        for (int i(0); i < Footstep::Count; ++i)
        {
            auto const ENUM{ static_cast<Footstep::Enum>(i) };

            if (ba::to_lower_copy(Footstep::ToString(ENUM)) == NAME_LOWER)
            {
                return ENUM;
            }
        }

        return Footstep::Count;
    }


    const std::string music::ToString(const music::Enum E)
    {
        switch (E)
        {
            case None:              { return "None"; }
            case Theme:             { return "theme-violin"; }
            case Wind:              { return "wind"; }
            case FootstepGrass:     { return "footstep-grass"; }
            case FootstepGravel:    { return "footstep-gravel"; }
            case FootstepLeaves:    { return "footstep-leaves"; }
            case FootstepSolid:     { return "footstep-solid"; }
            case FootstepWood:      { return "footstep-wood"; }
            case FireIndoorSmall:   { return "fire-indoor-small"; }
            case FireIndoorLarge:   { return "fire-indoor-large"; }
            case FireOutdoor1:      { return "fire-outdoor-1"; }
            case FireOutdoor2:      { return "fire-outdoor-2"; }
            case CombatIntro:       { return "combatintro"; }
            case PartyCreation:     { return "party-creation-background"; }
            case Credits:           { return "credits"; }
            case Inventory:         { return "inventory"; }
            case All:               { return "All"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
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
            case Theme:             { return "theme"; }
            case Wind:              { return "wind"; }
            case FootstepGrass:
            case FootstepGravel:
            case FootstepLeaves:
            case FootstepSolid:
            case FootstepWood:      { return "footstep"; }
            case FireIndoorSmall:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:      { return "fire"; }
            case CombatIntro:       { return "combat-intro"; }
            case PartyCreation:     { return "party-creation"; }
            case Credits:           { return "credits"; }
            case Inventory:         { return "inventory"; }
            case All:
            case None:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::Directory(" << music::ToString(E) << ")_InvalidValueError.";
                throw std::logic_error(ss.str());
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::Directory(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool music::IsLooped(const music::Enum E)
    {
        switch (E)
        {
            case Theme:
            case Wind:
            case FootstepGrass:
            case FootstepGravel:
            case FootstepLeaves:
            case FootstepSolid:
            case FootstepWood:
            case FireIndoorSmall:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:
            case PartyCreation:
            case Credits:
            case Inventory:     { return true; }
            case CombatIntro:   { return false; }
            case All:
            case None:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::IsLooped(" << music::ToString(E) << ")_InvalidValueError.";
                throw std::logic_error(ss.str());
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::IsLooped(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string music::ArtistName(const music::Enum E)
    {
        switch (E)
        {
            case Wind:              { return "Luke @RUST LTD"; }
            case FireIndoorSmall:   { return "Inchadney"; }
            case FootstepGrass:
            case FootstepGravel:
            case FootstepLeaves:
            case FootstepSolid:
            case FootstepWood:
            case Theme:
            case FireIndoorLarge:
            case FireOutdoor1:
            case FireOutdoor2:      { return "(unknown)"; }
            case CombatIntro:       { return "(various)"; }
            case Credits:
            case Inventory:         { return "Janne Hanhisuanto"; }
            case PartyCreation:     { return "Marcelo Fernandez"; }
            case All:
            case None:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::ArtistName(" << music::ToString(E) << ")_InvalidValueError.";
                throw std::logic_error(ss.str());
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::ArtistName(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string music::LicenseTitle(const music::Enum E)
    {
        switch (E)
        {
            case FireIndoorSmall:   { return "CC-BY 3.0"; }
            case FireIndoorLarge:   { return "Inchadney"; }
            case Theme:
            case Wind:
            case FootstepGrass:
            case FootstepGravel:
            case FootstepLeaves:
            case FootstepSolid:
            case FootstepWood:
            case FireOutdoor1:
            case FireOutdoor2:      { return "CC0 1.0"; }
            case CombatIntro:       { return "(various)"; }
            case Credits:
            case Inventory:         { return "CC-BY-SA 3.0"; }
            case PartyCreation:     { return "CC-BY 4.0"; }
            case All:
            case None:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::LicenseTitle(" << music::ToString(E) << ")_InvalidValueError.";
                throw std::logic_error(ss.str());
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::LicenseTitle(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string music::SongName(const music::Enum E)
    {
        switch (E)
        {
            case Theme:             { return "Heroes' Path Theme"; }
            case Wind:              { return "Wind"; }
            case FootstepGrass:     { return "Footstep Grass"; }
            case FootstepGravel:    { return "Footstep Gravel"; }
            case FootstepLeaves:    { return "Footstep Leaves"; }
            case FootstepSolid:     { return "Footstep Solid"; }
            case FootstepWood:      { return "Footstep Wood"; }
            case FireIndoorSmall:   { return "Fireplace Small"; }
            case FireIndoorLarge:   { return "Fireplace Large"; }
            case FireOutdoor1:      { return "Campfire1"; }
            case FireOutdoor2:      { return "Campfire2"; }
            case CombatIntro:       { return "(various)"; }
            case PartyCreation:     { return "Intro of Dragons"; }
            case Credits:           { return "Radakan Menu"; }
            case Inventory:         { return "PYC"; }
            case All:
            case None:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::SongName(" << music::ToString(E) << ")_InvalidValueError.";
                throw std::logic_error(ss.str());
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::music::SongName(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    music::Enum music::FootstepToMusic(const Footstep::Enum E)
    {
        if (E == Footstep::Grass) return music::FootstepGrass;
        else if (E == Footstep::Gravel) return music::FootstepGravel;
        else if (E == Footstep::Leaves) return music::FootstepLeaves;
        else if (E == Footstep::Solid) return music::FootstepSolid;
        else if (E == Footstep::Wood) return music::FootstepWood;
        else return music::Count;
    }

}
}
