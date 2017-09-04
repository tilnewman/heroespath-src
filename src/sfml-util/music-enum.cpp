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
#include <sstream>
#include <exception>


namespace sfml_util
{

    const std::string music::ToString(const music::Enum E)
    {
        switch (E)
        {
            case None:          { return "None"; }
            case Theme:         { return "theme-violin"; }
            case Wind:          { return "wind"; }
            case Fire1:         { return "fire-1"; }
            case Fire2:         { return "fire-2"; }
            case CombatIntro:   { return "combatintro"; }
            case PartyCreation: { return "party-creation-background"; }
            case Credits:       { return "credits"; }
            case Inventory:     { return "inventory"; }
            case All:           { return "All"; }
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
            case Theme:         { return "theme"; }
            case Wind:          { return "wind"; }
            case Fire1:
            case Fire2:         { return "fire"; }
            case CombatIntro:   { return "combat-intro"; }
            case PartyCreation: { return "party-creation"; }
            case Credits:       { return "credits"; }
            case Inventory:     { return "inventory"; }
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
            case Fire1:
            case PartyCreation:
            case Fire2:       
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
            case Wind:          { return "Luke @RUST LTD"; }
            case Theme:
            case Fire1:
            case Fire2:         { return "(unknown)"; }
            case CombatIntro:   { return "(various)"; }
            case Credits:
            case Inventory:     { return "Janne Hanhisuanto"; }
            case PartyCreation: { return "Marcelo Fernandez"; }
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
            case Theme:
            case Wind:
            case Fire1:
            case Fire2:         { return "CC0 1.0"; }
            case CombatIntro:   { return "(various)"; }
            case Credits:
            case Inventory:     { return "CC-BY-SA 3.0"; }
            case PartyCreation: { return "CC-BY 4.0"; }
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
            case Theme:         { return "Heroes' Path Theme"; }
            case Wind:          { return "Wind"; }
            case Fire1:         { return "Campfire1"; }
            case Fire2:         { return "Campfire2"; }
            case CombatIntro:   { return "(various)"; }
            case PartyCreation: { return "Intro of Dragons"; }
            case Credits:       { return "Radakan Menu"; }
            case Inventory:     { return "PYC"; } 
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

}
