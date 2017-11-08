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
// armor-types.cpp
//
#include "armor-types.hpp"
#include <exception>
#include <sstream>


namespace heroespath
{
namespace item
{
namespace armor
{

    const std::string shield_type::ToString(const shield_type::Enum E)
    {
        switch(E)
        {
            case Buckler: { return "Buckler"; }
            case Kite:    { return "Kite"; }
            case Heater:  { return "Heater"; }
            case Pavis:   { return "Pavis"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::armor::shield_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    shield_type::Enum shield_type::FromString(const std::string & SHIELD_NAME)
    {
        if      (SHIELD_NAME == ToString(Buckler)) return Buckler;
        else if (SHIELD_NAME == ToString(Kite))    return Kite;
        else if (SHIELD_NAME == ToString(Heater))  return Heater;
        else if (SHIELD_NAME == ToString(Pavis))   return Pavis;
        else
            return Count;
    }


    const std::string helm_type::ToString(const helm_type::Enum E)
    {
        switch(E)
        {
            case Leather:  { return "Leather"; }
            case MailCoif: { return "MailCoif"; }
            case Kettle:   { return "Kettle"; }
            case Archers:  { return "Archers"; }
            case Bascinet: { return "Bascinet"; }
            case Great:    { return "Great"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::armor::helm_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        };
    }


    helm_type::Enum helm_type::FromString(const std::string & HELM_NAME)
    {
        if      (HELM_NAME == ToString(Leather))    return Leather;
        else if (HELM_NAME == ToString(MailCoif))   return MailCoif;
        else if (HELM_NAME == ToString(Kettle))     return Kettle;
        else if (HELM_NAME == ToString(Archers))    return Archers;
        else if (HELM_NAME == ToString(Bascinet))   return Bascinet;
        else if (HELM_NAME == ToString(Great))      return Great;
        else
            return Count;
    }


    const std::string base_type::ToString(const base_type::Enum E)
    {
        switch(E)
        {
            case Plain: { return "Plain"; }
            case Mail:  { return "Mail"; }
            case Scale: { return "Scale"; }
            case Plate: { return "Plate"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::armor::base_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        };
    }


    base_type::Enum base_type::FromString(const std::string & TYPE_NAME)
    {
        if      (TYPE_NAME == ToString(Plain))  return Plain;
        else if (TYPE_NAME == ToString(Mail))   return Mail;
        else if (TYPE_NAME == ToString(Scale))  return Scale;
        else if (TYPE_NAME == ToString(Plate))  return Plate;
        else
            return Count;
    }


    const std::string cover_type::ToString(const cover_type::Enum E)
    {
        switch(E)
        {
            case Cape:  { return "Cape"; }
            case Vest:  { return "Vest"; }
            case Robe:  { return "Robe"; }
            case Cloak: { return "Cloak"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "item::armor::cover_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        };
    }


    cover_type::Enum cover_type::FromString(const std::string & COVER_NAME)
    {
        if      (COVER_NAME == ToString(Cape))  return Cape;
        else if (COVER_NAME == ToString(Vest))  return Vest;
        else if (COVER_NAME == ToString(Robe))  return Robe;
        else if (COVER_NAME == ToString(Cloak)) return Cloak;
        else
            return Count;
    }

}
}
}
