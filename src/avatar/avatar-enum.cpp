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
// anim-enum.cpp
//
#include "avatar-enum.hpp"

#include "game/game-data-file.hpp"

#include <boost/algorithm/string.hpp>

#include <exception>
#include <sstream>


namespace heroespath
{
namespace avatar
{
    
    const std::string Avatar::ToString(const NameEnum E)
    {
        switch (E)
        {
            case Metal:      { return "Metal"; }
            case Puck:       { return "Puck"; }
            case Sara:       { return "Sara"; }
            case Girl:       { return "Girl"; }
            case Circus:     { return "Circus"; }
            case Poor:       { return "Poor"; }
            case Rags:       { return "Rags"; }
            case Common:     { return "Common"; }
            case Tunic:      { return "Tunic"; }
            case Pants:      { return "Pants"; }
            case Maid:       { return "Maid"; }
            case Robes:      { return "Robes"; }
            case Whitebeard: { return "Whitebeard"; }
            case Dress:      { return "Dress"; }
            case Private:    { return "Private"; }
            case Private2nd: { return "Private2nd"; }
            case Private1st: { return "Private1st"; }
            case Specialist: { return "Specialist"; }
            case Corporal:   { return "Corporal"; }
            case Sergeant:   { return "Sergeant"; }
            case Major:      { return "Major"; }
            case Knight:     { return "Knight"; }
            case KnightGold: { return "KnightGold"; }
            case Thug:       { return "Thug"; }
            case ManAtArms:  { return "ManAtArms"; }
            case Gladiator:  { return "Gladiator"; }
            case Iron:       { return "Iron"; }
            case Monk:       { return "Monk"; }
            default:
            {
                std::ostringstream ss;
                ss << "avatar::Avatar::ToString(NameEnum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Avatar::IsPlayer(const NameEnum E)
    {
        return ((E == Metal) || (E == Puck) || (E == Sara));
    }


    const std::vector<Avatar::Enum> Avatar::Avatars(const NameEnum E)
    {
        switch (E)
        {
            case Metal:
            {
                return {
                    Avatar::Metal_Female_Dark,
                    Avatar::Metal_Female_Light,
                    Avatar::Metal_Male_Dark,
                    Avatar::Metal_Male_Light };
            }
            case Puck:
            {
                return {
                    Avatar::Puck_Female_Dark,
                    Avatar::Puck_Female_Light,
                    Avatar::Puck_Male_Dark,
                    Avatar::Puck_Male_Light };
            }
            case Sara:
            {
                return {
                    Avatar::Sara_BlueWhite,
                    Avatar::Sara_Cyan,
                    Avatar::Sara_Green,
                    Avatar::Sara_White };
            }
            case Girl:
            {
                return {
                    Avatar::Girl_Dark_Loose_Black,
                    Avatar::Girl_Dark_Loose_Brown,
                    Avatar::Girl_Dark_Loose_Brunette,
                    Avatar::Girl_Dark_Loose_Brunette2,
                    Avatar::Girl_Dark_Loose_WhiteBlonde2,
                    Avatar::Girl_Dark_Ponytail2_Black,
                    Avatar::Girl_Dark_Ponytail2_Brown,
                    Avatar::Girl_Dark_Ponytail2_Brunette,
                    Avatar::Girl_Dark_Ponytail2_Brunette2,
                    Avatar::Girl_Dark_Ponytail2_WhiteBlonde2,
                    Avatar::Girl_Dark_Princess_Black,
                    Avatar::Girl_Dark_Princess_Brown,
                    Avatar::Girl_Dark_Princess_Brunette,
                    Avatar::Girl_Dark_Princess_Brunette2,
                    Avatar::Girl_Dark_Princess_WhiteBlonde2,
                    Avatar::Girl_Dark_Unkempt_Black,
                    Avatar::Girl_Dark_Unkempt_Brown,
                    Avatar::Girl_Dark_Unkempt_Brunette,
                    Avatar::Girl_Dark_Unkempt_Brunette2,
                    Avatar::Girl_Dark_Unkempt_WhiteBlonde2,
                    Avatar::Girl_Light_Loose_Blonde,
                    Avatar::Girl_Light_Loose_Blonde2,
                    Avatar::Girl_Light_Loose_Brown,
                    Avatar::Girl_Light_Loose_Brunette,
                    Avatar::Girl_Light_Loose_Brunette2,
                    Avatar::Girl_Light_Loose_DarkBlonde,
                    Avatar::Girl_Light_Loose_LightBlonde,
                    Avatar::Girl_Light_Loose_LightBlonde2,
                    Avatar::Girl_Light_Ponytail2_Blonde,
                    Avatar::Girl_Light_Ponytail2_Blonde2,
                    Avatar::Girl_Light_Ponytail2_Brown,
                    Avatar::Girl_Light_Ponytail2_Brunette,
                    Avatar::Girl_Light_Ponytail2_Brunette2,
                    Avatar::Girl_Light_Ponytail2_DarkBlonde,
                    Avatar::Girl_Light_Ponytail2_LightBlonde,
                    Avatar::Girl_Light_Ponytail2_LightBlonde2,
                    Avatar::Girl_Light_Princess_Blonde,
                    Avatar::Girl_Light_Princess_Blonde2,
                    Avatar::Girl_Light_Princess_Brown,
                    Avatar::Girl_Light_Princess_Brunette,
                    Avatar::Girl_Light_Princess_Brunette2,
                    Avatar::Girl_Light_Princess_DarkBlonde,
                    Avatar::Girl_Light_Princess_LightBlonde,
                    Avatar::Girl_Light_Princess_LightBlonde2,
                    Avatar::Girl_Light_Unkempt_Blonde,
                    Avatar::Girl_Light_Unkempt_Blonde2,
                    Avatar::Girl_Light_Unkempt_Brown,
                    Avatar::Girl_Light_Unkempt_Brunette,
                    Avatar::Girl_Light_Unkempt_Brunette2,
                    Avatar::Girl_Light_Unkempt_DarkBlonde,
                    Avatar::Girl_Light_Unkempt_LightBlonde,
                    Avatar::Girl_Light_Unkempt_LightBlonde2 };
            }
            case Circus:
            {
                return {
                    Avatar::Leather_Circus1_Dark_Brunette,
                    Avatar::Leather_Circus1_Dark_Raven,
                    Avatar::Leather_Circus1_Dark_White,
                    Avatar::Leather_Circus1_Light_Blonde,
                    Avatar::Leather_Circus1_Light_Brunette,
                    Avatar::Leather_Circus1_Light_Raven,
                    Avatar::Leather_Circus2_Dark_Brunette,
                    Avatar::Leather_Circus2_Dark_Raven,
                    Avatar::Leather_Circus2_Dark_White,
                    Avatar::Leather_Circus2_Light_Blonde,
                    Avatar::Leather_Circus2_Light_Brunette,
                    Avatar::Leather_Circus2_Light_Raven };
            }
            case Poor:
            {
                return {
                    Avatar::Leather_Poor_Dark_Brunette,
                    Avatar::Leather_Poor_Dark_Raven,
                    Avatar::Leather_Poor_Dark_White,
                    Avatar::Leather_Poor_Light_Blonde,
                    Avatar::Leather_Poor_Light_Brunette,
                    Avatar::Leather_Poor_Light_Raven };
            }
            case Rags:
            {
                return {
                    Avatar::Rags_Female_Dark_Bangsshort_Black,
                    Avatar::Rags_Female_Dark_Bangsshort_Brown,
                    Avatar::Rags_Female_Dark_Bangsshort_Brunette,
                    Avatar::Rags_Female_Dark_Loose_Black,
                    Avatar::Rags_Female_Dark_Loose_Brown,
                    Avatar::Rags_Female_Dark_Loose_Brunette,
                    Avatar::Rags_Female_Dark_Ponytail_Black,
                    Avatar::Rags_Female_Dark_Ponytail_Brown,
                    Avatar::Rags_Female_Dark_Ponytail_Brunette,
                    Avatar::Rags_Female_Dark_Unkempt_Black,
                    Avatar::Rags_Female_Dark_Unkempt_Brown,
                    Avatar::Rags_Female_Dark_Unkempt_Brunette,
                    Avatar::Rags_Female_Light_Bangsshort_Black,
                    Avatar::Rags_Female_Light_Bangsshort_Blonde2,
                    Avatar::Rags_Female_Light_Bangsshort_Brown,
                    Avatar::Rags_Female_Light_Bangsshort_Brunette,
                    Avatar::Rags_Female_Light_Bangsshort_Brunette2,
                    Avatar::Rags_Female_Light_Bangsshort_LightBlonde2,
                    Avatar::Rags_Female_Light_Loose_Black,
                    Avatar::Rags_Female_Light_Loose_Blonde2,
                    Avatar::Rags_Female_Light_Loose_Brown,
                    Avatar::Rags_Female_Light_Loose_Brunette,
                    Avatar::Rags_Female_Light_Loose_Brunette2,
                    Avatar::Rags_Female_Light_Loose_LightBlonde2,
                    Avatar::Rags_Female_Light_Ponytail_Black,
                    Avatar::Rags_Female_Light_Ponytail_Blonde2,
                    Avatar::Rags_Female_Light_Ponytail_Brown,
                    Avatar::Rags_Female_Light_Ponytail_Brunette,
                    Avatar::Rags_Female_Light_Ponytail_Brunette2,
                    Avatar::Rags_Female_Light_Ponytail_LightBlonde2,
                    Avatar::Rags_Female_Light_Unkempt_Black,
                    Avatar::Rags_Female_Light_Unkempt_Blonde2,
                    Avatar::Rags_Female_Light_Unkempt_Brown,
                    Avatar::Rags_Female_Light_Unkempt_Brunette,
                    Avatar::Rags_Female_Light_Unkempt_Brunette2,
                    Avatar::Rags_Female_Light_Unkempt_LightBlonde2,
                    Avatar::Rags_Male_Dark_Bedhead_Brunette,
                    Avatar::Rags_Male_Dark_Bedhead_Raven,
                    Avatar::Rags_Male_Dark_Messy2_Brunette,
                    Avatar::Rags_Male_Dark_Messy2_Raven,
                    Avatar::Rags_Male_Dark_Parted_Brunette,
                    Avatar::Rags_Male_Dark_Parted_Raven,
                    Avatar::Rags_Male_Dark_Plain_Brunette,
                    Avatar::Rags_Male_Dark_Plain_Raven,
                    Avatar::Rags_Male_Light_Bedhead_Blonde,
                    Avatar::Rags_Male_Light_Bedhead_Brunette,
                    Avatar::Rags_Male_Light_Bedhead_Raven,
                    Avatar::Rags_Male_Light_Bedhead_White,
                    Avatar::Rags_Male_Light_Messy2_Blonde,
                    Avatar::Rags_Male_Light_Messy2_Brunette,
                    Avatar::Rags_Male_Light_Messy2_Raven,
                    Avatar::Rags_Male_Light_Messy2_White,
                    Avatar::Rags_Male_Light_Parted_Blonde,
                    Avatar::Rags_Male_Light_Parted_Brunette,
                    Avatar::Rags_Male_Light_Parted_Raven,
                    Avatar::Rags_Male_Light_Parted_White,
                    Avatar::Rags_Male_Light_Plain_Blonde,
                    Avatar::Rags_Male_Light_Plain_Brunette,
                    Avatar::Rags_Male_Light_Plain_Raven,
                    Avatar::Rags_Male_Light_Plain_White };
            }
            case Common:
            {
                return {
                    Avatar::Leather_Common1_Dark_Brunette,
                    Avatar::Leather_Common1_Dark_Raven,
                    Avatar::Leather_Common1_Dark_White,
                    Avatar::Leather_Common1_Light_Blonde,
                    Avatar::Leather_Common1_Light_Brunette,
                    Avatar::Leather_Common1_Light_Raven,
                    Avatar::Leather_Common2_Dark_Brunette,
                    Avatar::Leather_Common2_Dark_Raven,
                    Avatar::Leather_Common2_Dark_White,
                    Avatar::Leather_Common2_Light_Blonde,
                    Avatar::Leather_Common2_Light_Brunette,
                    Avatar::Leather_Common2_Light_Raven,
                    Avatar::Leather_Common3_Dark_Brunette,
                    Avatar::Leather_Common3_Dark_Raven,
                    Avatar::Leather_Common3_Dark_White,
                    Avatar::Leather_Common3_Light_Blonde,
                    Avatar::Leather_Common3_Light_Brunette,
                    Avatar::Leather_Common3_Light_Raven };
            }
            case Tunic:
            {
                return {
                    Avatar::Tunic_Female_Dark_Bangsshort_Black,
                    Avatar::Tunic_Female_Dark_Bangsshort_Brown,
                    Avatar::Tunic_Female_Dark_Bangsshort_Brunette,
                    Avatar::Tunic_Female_Dark_Loose_Black,
                    Avatar::Tunic_Female_Dark_Loose_Brown,
                    Avatar::Tunic_Female_Dark_Loose_Brunette,
                    Avatar::Tunic_Female_Dark_Ponytail_Black,
                    Avatar::Tunic_Female_Dark_Ponytail_Brown,
                    Avatar::Tunic_Female_Dark_Ponytail_Brunette,
                    Avatar::Tunic_Female_Dark_Unkempt_Black,
                    Avatar::Tunic_Female_Dark_Unkempt_Brown,
                    Avatar::Tunic_Female_Dark_Unkempt_Brunette,
                    Avatar::Tunic_Female_Light_Bangsshort_Black,
                    Avatar::Tunic_Female_Light_Bangsshort_Blonde2,
                    Avatar::Tunic_Female_Light_Bangsshort_Brown,
                    Avatar::Tunic_Female_Light_Bangsshort_Brunette,
                    Avatar::Tunic_Female_Light_Bangsshort_Brunette2,
                    Avatar::Tunic_Female_Light_Bangsshort_LightBlonde2,
                    Avatar::Tunic_Female_Light_Loose_Black,
                    Avatar::Tunic_Female_Light_Loose_Blonde2,
                    Avatar::Tunic_Female_Light_Loose_Brown,
                    Avatar::Tunic_Female_Light_Loose_Brunette,
                    Avatar::Tunic_Female_Light_Loose_Brunette2,
                    Avatar::Tunic_Female_Light_Loose_LightBlonde2,
                    Avatar::Tunic_Female_Light_Ponytail_Black,
                    Avatar::Tunic_Female_Light_Ponytail_Blonde2,
                    Avatar::Tunic_Female_Light_Ponytail_Brown,
                    Avatar::Tunic_Female_Light_Ponytail_Brunette,
                    Avatar::Tunic_Female_Light_Ponytail_Brunette2,
                    Avatar::Tunic_Female_Light_Ponytail_LightBlonde2,
                    Avatar::Tunic_Female_Light_Unkempt_Black,
                    Avatar::Tunic_Female_Light_Unkempt_Blonde2,
                    Avatar::Tunic_Female_Light_Unkempt_Brown,
                    Avatar::Tunic_Female_Light_Unkempt_Brunette,
                    Avatar::Tunic_Female_Light_Unkempt_Brunette2,
                    Avatar::Tunic_Female_Light_Unkempt_LightBlonde2 };
            }
            case Pants:
            {
                return {
                    Avatar::Pants_Male_Dark_Bedhead_Brunette,
                    Avatar::Pants_Male_Dark_Bedhead_Raven,
                    Avatar::Pants_Male_Dark_Messy2_Brunette,
                    Avatar::Pants_Male_Dark_Messy2_Raven,
                    Avatar::Pants_Male_Dark_Parted_Brunette,
                    Avatar::Pants_Male_Dark_Parted_Raven,
                    Avatar::Pants_Male_Dark_Plain_Brunette,
                    Avatar::Pants_Male_Dark_Plain_Raven,
                    Avatar::Pants_Male_Light_Bedhead_Blonde,
                    Avatar::Pants_Male_Light_Bedhead_Brunette,
                    Avatar::Pants_Male_Light_Bedhead_Raven,
                    Avatar::Pants_Male_Light_Bedhead_White,
                    Avatar::Pants_Male_Light_Messy2_Blonde,
                    Avatar::Pants_Male_Light_Messy2_Brunette,
                    Avatar::Pants_Male_Light_Messy2_Raven,
                    Avatar::Pants_Male_Light_Messy2_White,
                    Avatar::Pants_Male_Light_Parted_Blonde,
                    Avatar::Pants_Male_Light_Parted_Brunette,
                    Avatar::Pants_Male_Light_Parted_Raven,
                    Avatar::Pants_Male_Light_Parted_White,
                    Avatar::Pants_Male_Light_Plain_Blonde,
                    Avatar::Pants_Male_Light_Plain_Brunette,
                    Avatar::Pants_Male_Light_Plain_Raven,
                    Avatar::Pants_Male_Light_Plain_White };
            }
            case Maid:
            {
                return {
                    Avatar::Maid_Dark_Pixie_Black,
                    Maid_Dark_Pixie_Brown,
                    Maid_Dark_Pixie_Brunette,
                    Maid_Dark_Pixie_Gray,
                    Maid_Dark_Pixie_WhiteBlonde2,
                    Maid_Dark_Shoulderl_Black,
                    Maid_Dark_Shoulderl_Brown,
                    Maid_Dark_Shoulderl_Brunette,
                    Maid_Dark_Shoulderl_Gray,
                    Maid_Dark_Shoulderl_WhiteBlonde2,
                    Maid_Dark_Shoulderr_Black,
                    Maid_Dark_Shoulderr_Brown,
                    Maid_Dark_Shoulderr_Brunette,
                    Maid_Dark_Shoulderr_Gray,
                    Maid_Dark_Shoulderr_WhiteBlonde2,
                    Maid_Dark_Swoop_Black,
                    Maid_Dark_Swoop_Brown,
                    Maid_Dark_Swoop_Brunette,
                    Maid_Dark_Swoop_Gray,
                    Maid_Dark_Swoop_WhiteBlonde2,
                    Maid_Light_Pixie_Black,
                    Maid_Light_Pixie_Blonde2,
                    Maid_Light_Pixie_Brown,
                    Maid_Light_Pixie_Brunette,
                    Maid_Light_Pixie_Brunette2,
                    Maid_Light_Shoulderl_Black,
                    Maid_Light_Shoulderl_Blonde2,
                    Maid_Light_Shoulderl_Brown,
                    Maid_Light_Shoulderl_Brunette,
                    Maid_Light_Shoulderl_Brunette2,
                    Maid_Light_Shoulderr_Black,
                    Maid_Light_Shoulderr_Blonde2,
                    Maid_Light_Shoulderr_Brown,
                    Maid_Light_Shoulderr_Brunette,
                    Maid_Light_Shoulderr_Brunette2,
                    Maid_Light_Swoop_Black,
                    Maid_Light_Swoop_Blonde2,
                    Maid_Light_Swoop_Brown,
                    Maid_Light_Swoop_Brunette,
                    Maid_Light_Swoop_Brunette2 };
            }
            case Robes:
            {
                return {
                    Avatar::Robes_Male_Dark_Bedhead_Brunette,
                    Avatar::Robes_Male_Dark_Bedhead_Raven,
                    Avatar::Robes_Male_Dark_Messy2_Brunette,
                    Avatar::Robes_Male_Dark_Messy2_Raven,
                    Avatar::Robes_Male_Dark_Parted_Brunette,
                    Avatar::Robes_Male_Dark_Parted_Raven,
                    Avatar::Robes_Male_Dark_Plain_Brunette,
                    Avatar::Robes_Male_Dark_Plain_Raven,
                    Avatar::Robes_Male_Light_Bedhead_Blonde,
                    Avatar::Robes_Male_Light_Bedhead_Brunette,
                    Avatar::Robes_Male_Light_Bedhead_Raven,
                    Avatar::Robes_Male_Light_Bedhead_White,
                    Avatar::Robes_Male_Light_Messy2_Blonde,
                    Avatar::Robes_Male_Light_Messy2_Brunette,
                    Avatar::Robes_Male_Light_Messy2_Raven,
                    Avatar::Robes_Male_Light_Messy2_White,
                    Avatar::Robes_Male_Light_Parted_Blonde,
                    Avatar::Robes_Male_Light_Parted_Brunette,
                    Avatar::Robes_Male_Light_Parted_Raven,
                    Avatar::Robes_Male_Light_Parted_White,
                    Avatar::Robes_Male_Light_Plain_Blonde,
                    Avatar::Robes_Male_Light_Plain_Brunette,
                    Avatar::Robes_Male_Light_Plain_Raven,
                    Avatar::Robes_Male_Light_Plain_White };
            }
            case Whitebeard:
            {
                return { Avatar::Robes_Whitebeard };
            }
            case Dress:
            {
                return {
                    Avatar::Dress_Dark_Pixie_Black,
                    Avatar::Dress_Dark_Pixie_Brown,
                    Avatar::Dress_Dark_Pixie_Brunette,
                    Avatar::Dress_Dark_Pixie_Gray,
                    Avatar::Dress_Dark_Pixie_WhiteBlonde2,
                    Avatar::Dress_Dark_Shoulderl_Black,
                    Avatar::Dress_Dark_Shoulderl_Brown,
                    Avatar::Dress_Dark_Shoulderl_Brunette,
                    Avatar::Dress_Dark_Shoulderl_Gray,
                    Avatar::Dress_Dark_Shoulderl_WhiteBlonde2,
                    Avatar::Dress_Dark_Shoulderr_Brown,
                    Avatar::Dress_Dark_Shoulderr_Black,
                    Avatar::Dress_Dark_Shoulderr_Brunette,
                    Avatar::Dress_Dark_Shoulderr_Gray,
                    Avatar::Dress_Dark_Shoulderr_WhiteBlonde2,
                    Avatar::Dress_Dark_Swoop_Black,
                    Avatar::Dress_Dark_Swoop_Brown,
                    Avatar::Dress_Dark_Swoop_Brunette,
                    Avatar::Dress_Dark_Swoop_Gray,
                    Avatar::Dress_Dark_Swoop_WhiteBlonde2,
                    Avatar::Dress_Light_Pixie_Black,
                    Avatar::Dress_Light_Pixie_Blonde2,
                    Avatar::Dress_Light_Pixie_Brown,
                    Avatar::Dress_Light_Pixie_Brunette,
                    Avatar::Dress_Light_Pixie_Brunette2,
                    Avatar::Dress_Light_Pixie_Gray,
                    Avatar::Dress_Light_Shoulderl_Black,
                    Avatar::Dress_Light_Shoulderl_Blonde2,
                    Avatar::Dress_Light_Shoulderl_Brown,
                    Avatar::Dress_Light_Shoulderl_Brunette,
                    Avatar::Dress_Light_Shoulderl_Brunette2,
                    Avatar::Dress_Light_Shoulderl_Gray,
                    Avatar::Dress_Light_Shoulderr_Black,
                    Avatar::Dress_Light_Shoulderr_Blonde2,
                    Avatar::Dress_Light_Shoulderr_Brown,
                    Avatar::Dress_Light_Shoulderr_Brunette,
                    Avatar::Dress_Light_Shoulderr_Brunette2,
                    Avatar::Dress_Light_Shoulderr_Gray,
                    Avatar::Dress_Light_Swoop_Black,
                    Avatar::Dress_Light_Swoop_Blonde2,
                    Avatar::Dress_Light_Swoop_Brown,
                    Avatar::Dress_Light_Swoop_Brunette,
                    Avatar::Dress_Light_Swoop_Brunette2,
                    Avatar::Dress_Light_Swoop_Gray };
            }
            case Private:
            {
                return {
                    Avatar::Leather_Private1_Dark_Brunette,
                    Avatar::Leather_Private1_Dark_Raven,
                    Avatar::Leather_Private1_Dark_White,
                    Avatar::Leather_Private1_Light_Blonde,
                    Avatar::Leather_Private1_Light_Brunette,
                    Avatar::Leather_Private1_Light_Raven,
                    Avatar::Leather_Private2_Dark_Brunette,
                    Avatar::Leather_Private2_Dark_Raven,
                    Avatar::Leather_Private2_Dark_White,
                    Avatar::Leather_Private2_Light_Blonde,
                    Avatar::Leather_Private2_Light_Brunette,
                    Avatar::Leather_Private2_Light_Raven,
                    Avatar::Leather_Private3_Dark_Brunette,
                    Avatar::Leather_Private3_Dark_Raven,
                    Avatar::Leather_Private3_Dark_White,
                    Avatar::Leather_Private3_Light_Blonde,
                    Avatar::Leather_Private3_Light_Brunette,
                    Avatar::Leather_Private3_Light_Raven,
                    Avatar::Leather_Private4_Dark_Brunette,
                    Avatar::Leather_Private4_Dark_Raven,
                    Avatar::Leather_Private4_Dark_White,
                    Avatar::Leather_Private4_Light_Blonde,
                    Avatar::Leather_Private4_Light_Brunette,
                    Avatar::Leather_Private4_Light_Raven };
            }
            case Private2nd:
            {
                return {
                    Avatar::Leather_Private2nd1_Dark,
                    Avatar::Leather_Private2nd1_Light,
                    Avatar::Leather_Private2nd2_Dark,
                    Avatar::Leather_Private2nd2_Light,
                    Avatar::Leather_Private2nd3_Dark,
                    Avatar::Leather_Private2nd3_Light,
                    Avatar::Leather_Private2nd4_Dark,
                    Avatar::Leather_Private2nd4_Light };
            }
            case Private1st:
            {
                return {
                    Avatar::Leather_Private1st1_Dark,
                    Avatar::Leather_Private1st1_Light,
                    Avatar::Leather_Private1st2_Dark,
                    Avatar::Leather_Private1st2_Light,
                    Avatar::Leather_Private1st3_Dark,
                    Avatar::Leather_Private1st3_Light,
                    Avatar::Leather_Private1st4_Dark,
                    Avatar::Leather_Private1st4_Light };
            }
            case Specialist:
            {
                return {
                    Avatar::Leather_Specialist1_Dark,
                    Avatar::Leather_Specialist1_Light,
                    Avatar::Leather_Specialist2_Dark,
                    Avatar::Leather_Specialist2_Light };
            }
            case Corporal:
            {
                return {
                    Avatar::Leather_Corporal1_Dark,
                    Avatar::Leather_Corporal1_Light,
                    Avatar::Leather_Corporal2_Dark,
                    Avatar::Leather_Corporal2_Light };
            }
            case Sergeant:
            {
                return {
                    Avatar::Plate_Sergeant_Dark,
                    Avatar::Plate_Sergeant_Light };
            }
            case Major:
            {
                return {
                    Avatar::Plate_Major_Dark,
                    Avatar::Plate_Major_Light };
            }
            case Knight:
            {
                return { Avatar::Plate_Knight };
            }
            case KnightGold:
            {
                return { Avatar::Plate_Knight_Gold };
            }
            case Thug:
            {
                return {
                    Avatar::Leather_Thug1_Dark_Brunette,
                    Avatar::Leather_Thug1_Dark_Raven,
                    Avatar::Leather_Thug1_Dark_White,
                    Avatar::Leather_Thug1_Light_Blonde,
                    Avatar::Leather_Thug1_Light_Brunette,
                    Avatar::Leather_Thug1_Light_Raven,
                    Avatar::Leather_Thug2_Dark_Brunette,
                    Avatar::Leather_Thug2_Dark_Raven,
                    Avatar::Leather_Thug2_Dark_White,
                    Avatar::Leather_Thug2_Light_Blonde,
                    Avatar::Leather_Thug2_Light_Brunette,
                    Avatar::Leather_Thug2_Light_Raven };
            }
            case ManAtArms:
            {
                return {
                    Avatar::Plate_ManAtArms_Dark_Brunette,
                    Avatar::Plate_ManAtArms_Dark_Raven,
                    Avatar::Plate_ManAtArms_Dark_White,
                    Avatar::Plate_ManAtArms_Light_Blonde,
                    Avatar::Plate_ManAtArms_Light_Brunette,
                    Avatar::Plate_ManAtArms_Light_Raven };
            }
            case Gladiator:
            {
                return {
                    Avatar::Plate_Gladiator_Dark_Brunette,
                    Avatar::Plate_Gladiator_Dark_Raven,
                    Avatar::Plate_Gladiator_Dark_White,
                    Avatar::Plate_Gladiator_Light_Blonde,
                    Avatar::Plate_Gladiator_Light_Brunette,
                    Avatar::Plate_Gladiator_Light_Raven };
            }
            case Iron:
            {
                return {
                    Avatar::Leather_IronArms_Dark_Brunette,
                    Avatar::Leather_IronArms_Dark_Raven,
                    Avatar::Leather_IronArms_Dark_White,
                    Avatar::Leather_IronArms_Light_Blonde,
                    Avatar::Leather_IronArms_Light_Brunette,
                    Avatar::Leather_IronArms_Light_Raven,
                    Avatar::Leather_IronRich_Dark_Brunette,
                    Avatar::Leather_IronRich_Dark_Raven,
                    Avatar::Leather_IronRich_Dark_White,
                    Avatar::Leather_IronRich_Light_Blonde,
                    Avatar::Leather_IronRich_Light_Brunette,
                    Avatar::Leather_IronRich_Light_Raven };
            }
            case Monk:
            {
                return {
                    Avatar::Monk_BeltLeather_Dark_bracer,
                    Avatar::Monk_BeltLeather_Dark_nobracer,
                    Avatar::Monk_BeltLeather_Tanned2_bracer,
                    Avatar::Monk_BeltLeather_Tanned2_nobracer,
                    Avatar::Monk_Beltnone_Dark_bracer,
                    Avatar::Monk_Beltnone_Dark_nobracer,
                    Avatar::Monk_Beltnone_Tanned2_bracer,
                    Avatar::Monk_Beltnone_Tanned2_nobracer,
                    Avatar::Monk_Beltwhite_Dark_bracer,
                    Avatar::Monk_Beltwhite_Dark_nobracer,
                    Avatar::Monk_Beltwhite_Tanned2_bracer,
                    Avatar::Monk_Beltwhite_Tanned2_nobracer,
                    Avatar::Monk_Shirt_Beltnone_Dark,
                    Avatar::Monk_Shirt_Beltnone_Tanned2,
                    Avatar::Monk_Shirt_Beltwhite_Dark,
                    Avatar::Monk_Shirt_Beltwhite_Tanned2 };
            }
            default:
            {
                std::ostringstream ss;
                ss << "avatar::Avatar::Animations(NameEnum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string Avatar::ToString(const Avatar::Enum E)
    {
        switch (E)
        {
            case Metal_Female_Dark:                             { return "Metal-Female-Dark"; }
            case Metal_Female_Light:                            { return "Metal-Female-Light"; }
            case Metal_Male_Dark:                               { return "Metal-Male-Dark"; }
            case Metal_Male_Light:                              { return "Metal-Male-Light"; }
            case Puck_Female_Dark:                              { return "Puck-Female-Dark"; }
            case Puck_Female_Light:                             { return "Puck-Female-Light"; }
            case Puck_Male_Dark:                                { return "Puck-Male-Dark"; }
            case Puck_Male_Light:                               { return "Puck-Male-Light"; }
            case Sara_BlueWhite:                                { return "Sara-BlueWhite"; }
            case Sara_Cyan:                                     { return "Sara-Cyan"; }
            case Sara_Green:                                    { return "Sara-Green"; }
            case Sara_White:                                    { return "Sara-White"; }
            case Dress_Dark_Pixie_Black:                        { return "Dress-Dark-Pixie-Black"; }
            case Dress_Dark_Pixie_Brown:                        { return "Dress-Dark-Pixie-Brown"; }
            case Dress_Dark_Pixie_Brunette:                     { return "Dress-Dark-Pixie-Brunette"; }
            case Dress_Dark_Pixie_Gray:                         { return "Dress-Dark-Pixie-Gray"; }
            case Dress_Dark_Pixie_WhiteBlonde2:                 { return "Dress-Dark-Pixie-WhiteBlonde2"; }
            case Dress_Dark_Shoulderl_Black:                    { return "Dress-Dark-Shoulderl-Black"; }
            case Dress_Dark_Shoulderl_Brown:                    { return "Dress-Dark-Shoulderl-Brown"; }
            case Dress_Dark_Shoulderl_Brunette:                 { return "Dress-Dark-Shoulderl-Brunette"; }
            case Dress_Dark_Shoulderl_Gray:                     { return "Dress-Dark-Shoulderl-Gray"; }
            case Dress_Dark_Shoulderl_WhiteBlonde2:             { return "Dress-Dark-Shoulderl-WhiteBlonde2"; }
            case Dress_Dark_Shoulderr_Brown:                    { return "Dress-Dark-Shoulderr-Brown"; }
            case Dress_Dark_Shoulderr_Black:                    { return "Dress-Dark-Shoulderr-Black"; }
            case Dress_Dark_Shoulderr_Brunette:                 { return "Dress-Dark-Shoulderr-Brunette"; }
            case Dress_Dark_Shoulderr_Gray:                     { return "Dress-Dark-Shoulderr-Gray"; }
            case Dress_Dark_Shoulderr_WhiteBlonde2:             { return "Dress-Dark-Shoulderr-WhiteBlonde2"; }
            case Dress_Dark_Swoop_Black:                        { return "Dress-Dark-Swoop-Black"; }
            case Dress_Dark_Swoop_Brown:                        { return "Dress-Dark-Swoop-Brown"; }
            case Dress_Dark_Swoop_Brunette:                     { return "Dress-Dark-Swoop-Brunette"; }
            case Dress_Dark_Swoop_Gray:                         { return "Dress-Dark-Swoop-Gray"; }
            case Dress_Dark_Swoop_WhiteBlonde2:                 { return "Dress-Dark-Swoop-WhiteBlonde2"; }
            case Dress_Light_Pixie_Black:                       { return "Dress-Light-Pixie-Black"; }
            case Dress_Light_Pixie_Blonde2:                     { return "Dress-Light-Pixie-Blonde2"; }
            case Dress_Light_Pixie_Brown:                       { return "Dress-Light-Pixie-Brown"; }
            case Dress_Light_Pixie_Brunette:                    { return "Dress-Light-Pixie-Brunette"; }
            case Dress_Light_Pixie_Brunette2:                   { return "Dress-Light-Pixie-Brunette2"; }
            case Dress_Light_Pixie_Gray:                        { return "Dress-Light-Pixie-Gray"; }
            case Dress_Light_Shoulderl_Black:                   { return "Dress-Light-Shoulderl-Black"; }
            case Dress_Light_Shoulderl_Blonde2:                 { return "Dress-Light-Shoulderl-Blonde2"; }
            case Dress_Light_Shoulderl_Brown:                   { return "Dress-Light-Shoulderl-Brown"; }
            case Dress_Light_Shoulderl_Brunette:                { return "Dress-Light-Shoulderl-Brunette"; }
            case Dress_Light_Shoulderl_Brunette2:               { return "Dress-Light-Shoulderl-Brunette2"; }
            case Dress_Light_Shoulderl_Gray:                    { return "Dress-Light-Shoulderl-Gray"; }
            case Dress_Light_Shoulderr_Black:                   { return "Dress-Light-Shoulderr-Black"; }
            case Dress_Light_Shoulderr_Blonde2:                 { return "Dress-Light-Shoulderr-Blonde2"; }
            case Dress_Light_Shoulderr_Brown:                   { return "Dress-Light-Shoulderr-Brown"; }
            case Dress_Light_Shoulderr_Brunette:                { return "Dress-Light-Shoulderr-Brunette"; }
            case Dress_Light_Shoulderr_Brunette2:               { return "Dress-Light-Shoulderr-Brunette2"; }
            case Dress_Light_Shoulderr_Gray:                    { return "Dress-Light-Shoulderr-Gray"; }
            case Dress_Light_Swoop_Black:                       { return "Dress-Light-Swoop-Black"; }
            case Dress_Light_Swoop_Blonde2:                     { return "Dress-Light-Swoop-Blonde2"; }
            case Dress_Light_Swoop_Brown:                       { return "Dress-Light-Swoop-Brown"; }
            case Dress_Light_Swoop_Brunette:                    { return "Dress-Light-Swoop-Brunette"; }
            case Dress_Light_Swoop_Brunette2:                   { return "Dress-Light-Swoop-Brunette2"; }
            case Dress_Light_Swoop_Gray:                        { return "Dress-Light-Swoop-Gray"; }
            case Girl_Dark_Loose_Black:                         { return "Girl-Dark-Loose-Black"; }
            case Girl_Dark_Loose_Brown:                         { return "Girl-Dark-Loose-Brown"; }
            case Girl_Dark_Loose_Brunette:                      { return "Girl-Dark-Loose-Brunette"; }
            case Girl_Dark_Loose_Brunette2:                     { return "Girl-Dark-Loose-Brunette2"; }
            case Girl_Dark_Loose_WhiteBlonde2:                  { return "Girl-Dark-Loose-WhiteBlonde2"; }
            case Girl_Dark_Ponytail2_Black:                     { return "Girl-Dark-Ponytail2-Black"; }
            case Girl_Dark_Ponytail2_Brown:                     { return "Girl-Dark-Ponytail2-Brown"; }
            case Girl_Dark_Ponytail2_Brunette:                  { return "Girl-Dark-Ponytail2-Brunette"; }
            case Girl_Dark_Ponytail2_Brunette2:                 { return "Girl-Dark-Ponytail2-Brunette2"; }
            case Girl_Dark_Ponytail2_WhiteBlonde2:              { return "Girl-Dark-Ponytail2-WhiteBlonde2"; }
            case Girl_Dark_Princess_Black:                      { return "Girl-Dark-Princess-Black"; }
            case Girl_Dark_Princess_Brown:                      { return "Girl-Dark-Princess-Brown"; }
            case Girl_Dark_Princess_Brunette:                   { return "Girl-Dark-Princess-Brunette"; }
            case Girl_Dark_Princess_Brunette2:                  { return "Girl-Dark-Princess-Brunette2"; }
            case Girl_Dark_Princess_WhiteBlonde2:               { return "Girl-Dark-Princess-WhiteBlonde2"; }
            case Girl_Dark_Unkempt_Black:                       { return "Girl-Dark-Unkempt-Black"; }
            case Girl_Dark_Unkempt_Brown:                       { return "Girl-Dark-Unkempt-Brown"; }
            case Girl_Dark_Unkempt_Brunette:                    { return "Girl-Dark-Unkempt-Brunette"; }
            case Girl_Dark_Unkempt_Brunette2:                   { return "Girl-Dark-Unkempt-Brunette2"; }
            case Girl_Dark_Unkempt_WhiteBlonde2:                { return "Girl-Dark-Unkempt-WhiteBlonde2"; }
            case Girl_Light_Loose_Blonde:                       { return "Girl-Light-Loose-Blonde"; }
            case Girl_Light_Loose_Blonde2:                      { return "Girl-Light-Loose-Blonde2"; }
            case Girl_Light_Loose_Brown:                        { return "Girl-Light-Loose-Brown"; }
            case Girl_Light_Loose_Brunette:                     { return "Girl-Light-Loose-Brunette"; }
            case Girl_Light_Loose_Brunette2:                    { return "Girl-Light-Loose-Brunette2"; }
            case Girl_Light_Loose_DarkBlonde:                   { return "Girl-Light-Loose-DarkBlonde"; }
            case Girl_Light_Loose_LightBlonde:                  { return "Girl-Light-Loose-LightBlonde"; }
            case Girl_Light_Loose_LightBlonde2:                 { return "Girl-Light-Loose-LightBlonde2"; }
            case Girl_Light_Ponytail2_Blonde:                   { return "Girl-Light-Ponytail2-Blonde"; }
            case Girl_Light_Ponytail2_Blonde2:                  { return "Girl-Light-Ponytail2-Blonde2"; }
            case Girl_Light_Ponytail2_Brown:                    { return "Girl-Light-Ponytail2-Brown"; }
            case Girl_Light_Ponytail2_Brunette:                 { return "Girl-Light-Ponytail2-Brunette"; }
            case Girl_Light_Ponytail2_Brunette2:                { return "Girl-Light-Ponytail2-Brunette2"; }
            case Girl_Light_Ponytail2_DarkBlonde:               { return "Girl-Light-Ponytail2-DarkBlonde"; }
            case Girl_Light_Ponytail2_LightBlonde:              { return "Girl-Light-Ponytail2-LightBlonde"; }
            case Girl_Light_Ponytail2_LightBlonde2:             { return "Girl-Light-Ponytail2-LightBlonde2"; }
            case Girl_Light_Princess_Blonde:                    { return "Girl-Light-Princess-Blonde"; }
            case Girl_Light_Princess_Blonde2:                   { return "Girl-Light-Princess-Blonde2"; }
            case Girl_Light_Princess_Brown:                     { return "Girl-Light-Princess-Brown"; }
            case Girl_Light_Princess_Brunette:                  { return "Girl-Light-Princess-Brunette"; }
            case Girl_Light_Princess_Brunette2:                 { return "Girl-Light-Princess-Brunette2"; }
            case Girl_Light_Princess_DarkBlonde:                { return "Girl-Light-Princess-DarkBlonde"; }
            case Girl_Light_Princess_LightBlonde:               { return "Girl-Light-Princess-LightBlonde"; }
            case Girl_Light_Princess_LightBlonde2:              { return "Girl-Light-Princess-LightBlonde2"; }
            case Girl_Light_Unkempt_Blonde:                     { return "Girl-Light-Unkempt-Blonde"; }
            case Girl_Light_Unkempt_Blonde2:                    { return "Girl-Light-Unkempt-Blonde2"; }
            case Girl_Light_Unkempt_Brown:                      { return "Girl-Light-Unkempt-Brown"; }
            case Girl_Light_Unkempt_Brunette:                   { return "Girl-Light-Unkempt-Brunette"; }
            case Girl_Light_Unkempt_Brunette2:                  { return "Girl-Light-Unkempt-Brunette2"; }
            case Girl_Light_Unkempt_DarkBlonde:                 { return "Girl-Light-Unkempt-DarkBlonde"; }
            case Girl_Light_Unkempt_LightBlonde:                { return "Girl-Light-Unkempt-LightBlonde"; }
            case Girl_Light_Unkempt_LightBlonde2:               { return "Girl-Light-Unkempt-LightBlonde2"; }
            case Leather_Circus1_Dark_Brunette:                 { return "Leather-Circus1-Dark-Brunette"; }
            case Leather_Circus1_Dark_Raven:                    { return "Leather-Circus1-Dark-Raven"; }
            case Leather_Circus1_Dark_White:                    { return "Leather-Circus1-Dark-White"; }
            case Leather_Circus1_Light_Blonde:                  { return "Leather-Circus1-Light-Blonde"; }
            case Leather_Circus1_Light_Brunette:                { return "Leather-Circus1-Light-Brunette"; }
            case Leather_Circus1_Light_Raven:                   { return "Leather-Circus1-Light-Raven"; }
            case Leather_Circus2_Dark_Brunette:                 { return "Leather-Circus2-Dark-Brunette"; }
            case Leather_Circus2_Dark_Raven:                    { return "Leather-Circus2-Dark-Raven"; }
            case Leather_Circus2_Dark_White:                    { return "Leather-Circus2-Dark-White"; }
            case Leather_Circus2_Light_Blonde:                  { return "Leather-Circus2-Light-Blonde"; }
            case Leather_Circus2_Light_Brunette:                { return "Leather-Circus2-Light-Brunette"; }
            case Leather_Circus2_Light_Raven:                   { return "Leather-Circus2-Light-Raven"; }
            case Leather_Common1_Dark_Brunette:                 { return "Leather-Common1-Dark-Brunette"; }
            case Leather_Common1_Dark_Raven:                    { return "Leather-Common1-Dark-Raven"; }
            case Leather_Common1_Dark_White:                    { return "Leather-Common1-Dark-White"; }
            case Leather_Common1_Light_Blonde:                  { return "Leather-Common1-Light-Blonde"; }
            case Leather_Common1_Light_Brunette:                { return "Leather-Common1-Light-Brunette"; }
            case Leather_Common1_Light_Raven:                   { return "Leather-Common1-Light-Raven"; }
            case Leather_Common2_Dark_Brunette:                 { return "Leather-Common2-Dark-Brunette"; }
            case Leather_Common2_Dark_Raven:                    { return "Leather-Common2-Dark-Raven"; }
            case Leather_Common2_Dark_White:                    { return "Leather-Common2-Dark-White"; }
            case Leather_Common2_Light_Blonde:                  { return "Leather-Common2-Light-Blonde"; }
            case Leather_Common2_Light_Brunette:                { return "Leather-Common2-Light-Brunette"; }
            case Leather_Common2_Light_Raven:                   { return "Leather-Common2-Light-Raven"; }
            case Leather_Common3_Dark_Brunette:                 { return "Leather-Common3-Dark-Brunette"; }
            case Leather_Common3_Dark_Raven:                    { return "Leather-Common3-Dark-Raven"; }
            case Leather_Common3_Dark_White:                    { return "Leather-Common3-Dark-White"; }
            case Leather_Common3_Light_Blonde:                  { return "Leather-Common3-Light-Blonde"; }
            case Leather_Common3_Light_Brunette:                { return "Leather-Common3-Light-Brunette"; }
            case Leather_Common3_Light_Raven:                   { return "Leather-Common3-Light-Raven"; }
            case Leather_Corporal1_Dark:                        { return "Leather-Corporal1-Dark"; }
            case Leather_Corporal1_Light:                       { return "Leather-Corporal1-Light"; }
            case Leather_Corporal2_Dark:                        { return "Leather-Corporal2-Dark"; }
            case Leather_Corporal2_Light:                       { return "Leather-Corporal2-Light"; }
            case Leather_IronArms_Dark_Brunette:                { return "Leather-IronArms-Dark-Brunette"; }
            case Leather_IronArms_Dark_Raven:                   { return "Leather-IronArms-Dark-Raven"; }
            case Leather_IronArms_Dark_White:                   { return "Leather-IronArms-Dark-White"; }
            case Leather_IronArms_Light_Blonde:                 { return "Leather-IronArms-Light-Blonde"; }
            case Leather_IronArms_Light_Brunette:               { return "Leather-IronArms-Light-Brunette"; }
            case Leather_IronArms_Light_Raven:                  { return "Leather-IronArms-Light-Raven"; }
            case Leather_IronRich_Dark_Brunette:                { return "Leather-IronRich-Dark-Brunette"; }
            case Leather_IronRich_Dark_Raven:                   { return "Leather-IronRich-Dark-Raven"; }
            case Leather_IronRich_Dark_White:                   { return "Leather-IronRich-Dark-White"; }
            case Leather_IronRich_Light_Blonde:                 { return "Leather-IronRich-Light-Blonde"; }
            case Leather_IronRich_Light_Brunette:               { return "Leather-IronRich-Light-Brunette"; }
            case Leather_IronRich_Light_Raven:                  { return "Leather-IronRich-Light-Raven"; }
            case Leather_Poor_Dark_Brunette:                    { return "Leather-Poor-Dark-Brunette"; }
            case Leather_Poor_Dark_Raven:                       { return "Leather-Poor-Dark-Raven"; }
            case Leather_Poor_Dark_White:                       { return "Leather-Poor-Dark-White"; }
            case Leather_Poor_Light_Blonde:                     { return "Leather-Poor-Light-Blonde"; }
            case Leather_Poor_Light_Brunette:                   { return "Leather-Poor-Light-Brunette"; }
            case Leather_Poor_Light_Raven:                      { return "Leather-Poor-Light-Raven"; }
            case Leather_Private1_Dark_Brunette:                { return "Leather-Private1-Dark-Brunette"; }
            case Leather_Private1_Dark_Raven:                   { return "Leather-Private1-Dark-Raven"; }
            case Leather_Private1_Dark_White:                   { return "Leather-Private1-Dark-White"; }
            case Leather_Private1_Light_Blonde:                 { return "Leather-Private1-Light-Blonde"; }
            case Leather_Private1_Light_Brunette:               { return "Leather-Private1-Light-Brunette"; }
            case Leather_Private1_Light_Raven:                  { return "Leather-Private1-Light-Raven"; }
            case Leather_Private2_Dark_Brunette:                { return "Leather-Private2-Dark-Brunette"; }
            case Leather_Private2_Dark_Raven:                   { return "Leather-Private2-Dark-Raven"; }
            case Leather_Private2_Dark_White:                   { return "Leather-Private2-Dark-White"; }
            case Leather_Private2_Light_Blonde:                 { return "Leather-Private2-Light-Blonde"; }
            case Leather_Private2_Light_Brunette:               { return "Leather-Private2-Light-Brunette"; }
            case Leather_Private2_Light_Raven:                  { return "Leather-Private2-Light-Raven"; }
            case Leather_Private3_Dark_Brunette:                { return "Leather-Private3-Dark-Brunette"; }
            case Leather_Private3_Dark_Raven:                   { return "Leather-Private3-Dark-Raven"; }
            case Leather_Private3_Dark_White:                   { return "Leather-Private3-Dark-White"; }
            case Leather_Private3_Light_Blonde:                 { return "Leather-Private3-Light-Blonde"; }
            case Leather_Private3_Light_Brunette:               { return "Leather-Private3-Light-Brunette"; }
            case Leather_Private3_Light_Raven:                  { return "Leather-Private3-Light-Raven"; }
            case Leather_Private4_Dark_Brunette:                { return "Leather-Private4-Dark-Brunette"; }
            case Leather_Private4_Dark_Raven:                   { return "Leather-Private4-Dark-Raven"; }
            case Leather_Private4_Dark_White:                   { return "Leather-Private4-Dark-White"; }
            case Leather_Private4_Light_Blonde:                 { return "Leather-Private4-Light-Blonde"; }
            case Leather_Private4_Light_Brunette:               { return "Leather-Private4-Light-Brunette"; }
            case Leather_Private4_Light_Raven:                  { return "Leather-Private4-Light-Raven"; }
            case Leather_Private2nd1_Dark:                      { return "Leather-Private2nd1-Dark"; }
            case Leather_Private2nd1_Light:                     { return "Leather-Private2nd1-Light"; }
            case Leather_Private2nd2_Dark:                      { return "Leather-Private2nd2-Dark"; }
            case Leather_Private2nd2_Light:                     { return "Leather-Private2nd2-Light"; }
            case Leather_Private2nd3_Dark:                      { return "Leather-Private2nd3-Dark"; }
            case Leather_Private2nd3_Light:                     { return "Leather-Private2nd3-Light"; }
            case Leather_Private2nd4_Dark:                      { return "Leather-Private2nd4-Dark"; }
            case Leather_Private2nd4_Light:                     { return "Leather-Private2nd4-Light"; }
            case Leather_Private1st1_Dark:                      { return "Leather-Private1st1-Dark"; }
            case Leather_Private1st1_Light:                     { return "Leather-Private1st1-Light"; }
            case Leather_Private1st2_Dark:                      { return "Leather-Private1st2-Dark"; }
            case Leather_Private1st2_Light:                     { return "Leather-Private1st2-Light"; }
            case Leather_Private1st3_Dark:                      { return "Leather-Private1st3-Dark"; }
            case Leather_Private1st3_Light:                     { return "Leather-Private1st3-Light"; }
            case Leather_Private1st4_Dark:                      { return "Leather-Private1st4-Dark"; }
            case Leather_Private1st4_Light:                     { return "Leather-Private1st4-Light"; }
            case Leather_Specialist1_Dark:                      { return "Leather-Specialist1-Dark"; }
            case Leather_Specialist1_Light:                     { return "Leather-Specialist1-Light"; }
            case Leather_Specialist2_Dark:                      { return "Leather-Specialist2-Dark"; }
            case Leather_Specialist2_Light:                     { return "Leather-Specialist2-Light"; }
            case Leather_Thug1_Dark_Brunette:                   { return "Leather-Thug1-Dark-Brunette"; }
            case Leather_Thug1_Dark_Raven:                      { return "Leather-Thug1-Dark-Raven"; }
            case Leather_Thug1_Dark_White:                      { return "Leather-Thug1-Dark-White"; }
            case Leather_Thug1_Light_Blonde:                    { return "Leather-Thug1-Light-Blonde"; }
            case Leather_Thug1_Light_Brunette:                  { return "Leather-Thug1-Light-Brunette"; }
            case Leather_Thug1_Light_Raven:                     { return "Leather-Thug1-Light-Raven"; }
            case Leather_Thug2_Dark_Brunette:                   { return "Leather-Thug2-Dark-Brunette"; }
            case Leather_Thug2_Dark_Raven:                      { return "Leather-Thug2-Dark-Raven"; }
            case Leather_Thug2_Dark_White:                      { return "Leather-Thug2-Dark-White"; }
            case Leather_Thug2_Light_Blonde:                    { return "Leather-Thug2-Light-Blonde"; }
            case Leather_Thug2_Light_Brunette:                  { return "Leather-Thug2-Light-Brunette"; }
            case Leather_Thug2_Light_Raven:                     { return "Leather-Thug2-Light-Raven"; }
            case Maid_Dark_Pixie_Black:                         { return "Maid-Dark-Pixie-Black"; }
            case Maid_Dark_Pixie_Brown:                         { return "Maid-Dark-Pixie-Brown"; }
            case Maid_Dark_Pixie_Brunette:                      { return "Maid-Dark-Pixie-Brunette"; }
            case Maid_Dark_Pixie_Gray:                          { return "Maid-Dark-Pixie-Gray"; }
            case Maid_Dark_Pixie_WhiteBlonde2:                  { return "Maid-Dark-Pixie-WhiteBlonde2"; }
            case Maid_Dark_Shoulderl_Black:                     { return "Maid-Dark-Shoulderl-Black"; }
            case Maid_Dark_Shoulderl_Brown:                     { return "Maid-Dark-Shoulderl-Brown"; }
            case Maid_Dark_Shoulderl_Brunette:                  { return "Maid-Dark-Shoulderl-Brunette"; }
            case Maid_Dark_Shoulderl_Gray:                      { return "Maid-Dark-Shoulderl-Gray"; }
            case Maid_Dark_Shoulderl_WhiteBlonde2:              { return "Maid-Dark-Shoulderl-WhiteBlonde2"; }
            case Maid_Dark_Shoulderr_Black:                     { return "Maid-Dark-Shoulderr-Black"; }
            case Maid_Dark_Shoulderr_Brown:                     { return "Maid-Dark-Shoulderr-Brown"; }
            case Maid_Dark_Shoulderr_Brunette:                  { return "Maid-Dark-Shoulderr-Brunette"; }
            case Maid_Dark_Shoulderr_Gray:                      { return "Maid-Dark-Shoulderr-Gray"; }
            case Maid_Dark_Shoulderr_WhiteBlonde2:              { return "Maid-Dark-Shoulderr-WhiteBlonde2"; }
            case Maid_Dark_Swoop_Black:                         { return "Maid-Dark-Swoop-Black"; }
            case Maid_Dark_Swoop_Brown:                         { return "Maid-Dark-Swoop-Brown"; }
            case Maid_Dark_Swoop_Brunette:                      { return "Maid-Dark-Swoop-Brunette"; }
            case Maid_Dark_Swoop_Gray:                          { return "Maid-Dark-Swoop-Gray"; }
            case Maid_Dark_Swoop_WhiteBlonde2:                  { return "Maid-Dark-Swoop-WhiteBlonde2"; }
            case Maid_Light_Pixie_Black:                        { return "Maid-Light-Pixie-Black"; }
            case Maid_Light_Pixie_Blonde2:                      { return "Maid-Light-Pixie-Blonde2"; }
            case Maid_Light_Pixie_Brown:                        { return "Maid-Light-Pixie-Brown"; }
            case Maid_Light_Pixie_Brunette:                     { return "Maid-Light-Pixie-Brunette"; }
            case Maid_Light_Pixie_Brunette2:                    { return "Maid-Light-Pixie-Brunette2"; }
            case Maid_Light_Shoulderl_Black:                    { return "Maid-Light-Shoulderl-Black"; }
            case Maid_Light_Shoulderl_Blonde2:                  { return "Maid-Light-Shoulderl-Blonde2"; }
            case Maid_Light_Shoulderl_Brown:                    { return "Maid-Light-Shoulderl-Brown"; }
            case Maid_Light_Shoulderl_Brunette:                 { return "Maid-Light-Shoulderl-Brunette"; }
            case Maid_Light_Shoulderl_Brunette2:                { return "Maid-Light-Shoulderl-Brunette2"; }
            case Maid_Light_Shoulderr_Black:                    { return "Maid-Light-Shoulderr-Black"; }
            case Maid_Light_Shoulderr_Blonde2:                  { return "Maid-Light-Shoulderr-Blonde2"; }
            case Maid_Light_Shoulderr_Brown:                    { return "Maid-Light-Shoulderr-Brown"; }
            case Maid_Light_Shoulderr_Brunette:                 { return "Maid-Light-Shoulderr-Brunette"; }
            case Maid_Light_Shoulderr_Brunette2:                { return "Maid-Light-Shoulderr-Brunette2"; }
            case Maid_Light_Swoop_Black:                        { return "Maid-Light-Swoop-Black"; }
            case Maid_Light_Swoop_Blonde2:                      { return "Maid-Light-Swoop-Blonde2"; }
            case Maid_Light_Swoop_Brown:                        { return "Maid-Light-Swoop-Brown"; }
            case Maid_Light_Swoop_Brunette:                     { return "Maid-Light-Swoop-Brunette"; }
            case Maid_Light_Swoop_Brunette2:                    { return "Maid-Light-Swoop-Brunette2"; }
            case Monk_BeltLeather_Dark_bracer:                  { return "Monk-BeltLeather-Dark-bracer"; }
            case Monk_BeltLeather_Dark_nobracer:                { return "Monk-BeltLeather-Dark-nobracer"; }
            case Monk_BeltLeather_Tanned2_bracer:               { return "Monk-BeltLeather-Tanned2-bracer"; }
            case Monk_BeltLeather_Tanned2_nobracer:             { return "Monk-BeltLeather-Tanned2-nobracer"; }
            case Monk_Beltnone_Dark_bracer:                     { return "Monk-Beltnone-Dark-bracer"; }
            case Monk_Beltnone_Dark_nobracer:                   { return "Monk-Beltnone-Dark-nobracer"; }
            case Monk_Beltnone_Tanned2_bracer:                  { return "Monk-Beltnone-Tanned2-bracer"; }
            case Monk_Beltnone_Tanned2_nobracer:                { return "Monk-Beltnone-Tanned2-nobracer"; }
            case Monk_Beltwhite_Dark_bracer:                    { return "Monk-Beltwhite-Dark-bracer"; }
            case Monk_Beltwhite_Dark_nobracer:                  { return "Monk-Beltwhite-Dark-nobracer"; }
            case Monk_Beltwhite_Tanned2_bracer:                 { return "Monk-Beltwhite-Tanned2-bracer"; }
            case Monk_Beltwhite_Tanned2_nobracer:               { return "Monk-Beltwhite-Tanned2-nobracer"; }
            case Monk_Shirt_Beltnone_Dark:                      { return "Monk-Shirt-Beltnone-Dark"; }
            case Monk_Shirt_Beltnone_Tanned2:                   { return "Monk-Shirt-Beltnone-Tanned2"; }
            case Monk_Shirt_Beltwhite_Dark:                     { return "Monk-Shirt-Beltwhite-Dark"; }
            case Monk_Shirt_Beltwhite_Tanned2:                  { return "Monk-Shirt-Beltwhite-Tanned2"; }
            case Pants_Male_Dark_Bedhead_Brunette:              { return "Pants-Male-Dark-Bedhead-Brunette"; }
            case Pants_Male_Dark_Bedhead_Raven:                 { return "Pants-Male-Dark-Bedhead-Raven"; }
            case Pants_Male_Dark_Messy2_Brunette:               { return "Pants-Male-Dark-Messy2-Brunette"; }
            case Pants_Male_Dark_Messy2_Raven:                  { return "Pants-Male-Dark-Messy2-Raven"; }
            case Pants_Male_Dark_Parted_Brunette:               { return "Pants-Male-Dark-Parted-Brunette"; }
            case Pants_Male_Dark_Parted_Raven:                  { return "Pants-Male-Dark-Parted-Raven"; }
            case Pants_Male_Dark_Plain_Brunette:                { return "Pants-Male-Dark-Plain-Brunette"; }
            case Pants_Male_Dark_Plain_Raven:                   { return "Pants-Male-Dark-Plain-Raven"; }
            case Pants_Male_Light_Bedhead_Blonde:               { return "Pants-Male-Light-Bedhead-Blonde"; }
            case Pants_Male_Light_Bedhead_Brunette:             { return "Pants-Male-Light-Bedhead-Brunette"; }
            case Pants_Male_Light_Bedhead_Raven:                { return "Pants-Male-Light-Bedhead-Raven"; }
            case Pants_Male_Light_Bedhead_White:                { return "Pants-Male-Light-Bedhead-White"; }
            case Pants_Male_Light_Messy2_Blonde:                { return "Pants-Male-Light-Messy2-Blonde"; }
            case Pants_Male_Light_Messy2_Brunette:              { return "Pants-Male-Light-Messy2-Brunette"; }
            case Pants_Male_Light_Messy2_Raven:                 { return "Pants-Male-Light-Messy2-Raven"; }
            case Pants_Male_Light_Messy2_White:                 { return "Pants-Male-Light-Messy2-White"; }
            case Pants_Male_Light_Parted_Blonde:                { return "Pants-Male-Light-Parted-Blonde"; }
            case Pants_Male_Light_Parted_Brunette:              { return "Pants-Male-Light-Parted-Brunette"; }
            case Pants_Male_Light_Parted_Raven:                 { return "Pants-Male-Light-Parted-Raven"; }
            case Pants_Male_Light_Parted_White:                 { return "Pants-Male-Light-Parted-White"; }
            case Pants_Male_Light_Plain_Blonde:                 { return "Pants-Male-Light-Plain-Blonde"; }
            case Pants_Male_Light_Plain_Brunette:               { return "Pants-Male-Light-Plain-Brunette"; }
            case Pants_Male_Light_Plain_Raven:                  { return "Pants-Male-Light-Plain-Raven"; }
            case Pants_Male_Light_Plain_White:                  { return "Pants-Male-Light-Plain-White"; }
            case Plate_Gladiator_Dark_Brunette:                 { return "Plate-Gladiator-Dark-Brunette"; }
            case Plate_Gladiator_Dark_Raven:                    { return "Plate-Gladiator-Dark-Raven"; }
            case Plate_Gladiator_Dark_White:                    { return "Plate-Gladiator-Dark-White"; }
            case Plate_Gladiator_Light_Blonde:                  { return "Plate-Gladiator-Light-Blonde"; }
            case Plate_Gladiator_Light_Brunette:                { return "Plate-Gladiator-Light-Brunette"; }
            case Plate_Gladiator_Light_Raven:                   { return "Plate-Gladiator-Light-Raven"; }
            case Plate_Knight:                                  { return "Plate-Knight"; }
            case Plate_Knight_Gold:                             { return "Plate-Knight-Gold"; }
            case Plate_Major_Dark:                              { return "Plate-Major-Dark"; }
            case Plate_Major_Light:                             { return "Plate-Major-Light"; }
            case Plate_ManAtArms_Dark_Brunette:                 { return "Plate-ManAtArms-Dark-Brunette"; }
            case Plate_ManAtArms_Dark_Raven:                    { return "Plate-ManAtArms-Dark-Raven"; }
            case Plate_ManAtArms_Dark_White:                    { return "Plate-ManAtArms-Dark-White"; }
            case Plate_ManAtArms_Light_Blonde:                  { return "Plate-ManAtArms-Light-Blonde"; }
            case Plate_ManAtArms_Light_Brunette:                { return "Plate-ManAtArms-Light-Brunette"; }
            case Plate_ManAtArms_Light_Raven:                   { return "Plate-ManAtArms-Light-Raven"; }
            case Plate_Sergeant_Dark:                           { return "Plate-Sergeant-Dark"; }
            case Plate_Sergeant_Light:                          { return "Plate-Sergeant-Light"; }
            case Rags_Female_Dark_Bangsshort_Black:             { return "Rags-Female-Dark-Bangsshort-Black"; }
            case Rags_Female_Dark_Bangsshort_Brown:             { return "Rags-Female-Dark-Bangsshort-Brown"; }
            case Rags_Female_Dark_Bangsshort_Brunette:          { return "Rags-Female-Dark-Bangsshort-Brunette"; }
            case Rags_Female_Dark_Loose_Black:                  { return "Rags-Female-Dark-Loose-Black"; }
            case Rags_Female_Dark_Loose_Brown:                  { return "Rags-Female-Dark-Loose-Brown"; }
            case Rags_Female_Dark_Loose_Brunette:               { return "Rags-Female-Dark-Loose-Brunette"; }
            case Rags_Female_Dark_Ponytail_Black:               { return "Rags-Female-Dark-Ponytail-Black"; }
            case Rags_Female_Dark_Ponytail_Brown:               { return "Rags-Female-Dark-Ponytail-Brown"; }
            case Rags_Female_Dark_Ponytail_Brunette:            { return "Rags-Female-Dark-Ponytail-Brunette"; }
            case Rags_Female_Dark_Unkempt_Black:                { return "Rags-Female-Dark-Unkempt-Black"; }
            case Rags_Female_Dark_Unkempt_Brown:                { return "Rags-Female-Dark-Unkempt-Brown"; }
            case Rags_Female_Dark_Unkempt_Brunette:             { return "Rags-Female-Dark-Unkempt-Brunette"; }
            case Rags_Female_Light_Bangsshort_Black:            { return "Rags-Female-Light-Bangsshort-Black"; }
            case Rags_Female_Light_Bangsshort_Blonde2:          { return "Rags-Female-Light-Bangsshort-Blonde2"; }
            case Rags_Female_Light_Bangsshort_Brown:            { return "Rags-Female-Light-Bangsshort-Brown"; }
            case Rags_Female_Light_Bangsshort_Brunette:         { return "Rags-Female-Light-Bangsshort-Brunette"; }
            case Rags_Female_Light_Bangsshort_Brunette2:        { return "Rags-Female-Light-Bangsshort-Brunette2"; }
            case Rags_Female_Light_Bangsshort_LightBlonde2:     { return "Rags-Female-Light-Bangsshort-LightBlonde2"; }
            case Rags_Female_Light_Loose_Black:                 { return "Rags-Female-Light-Loose-Black"; }
            case Rags_Female_Light_Loose_Blonde2:               { return "Rags-Female-Light-Loose-Blonde2"; }
            case Rags_Female_Light_Loose_Brown:                 { return "Rags-Female-Light-Loose-Brown"; }
            case Rags_Female_Light_Loose_Brunette:              { return "Rags-Female-Light-Loose-Brunette"; }
            case Rags_Female_Light_Loose_Brunette2:             { return "Rags-Female-Light-Loose-Brunette2"; }
            case Rags_Female_Light_Loose_LightBlonde2:          { return "Rags-Female-Light-Loose-LightBlonde2"; }
            case Rags_Female_Light_Ponytail_Black:              { return "Rags-Female-Light-Ponytail-Black"; }
            case Rags_Female_Light_Ponytail_Blonde2:            { return "Rags-Female-Light-Ponytail-Blonde2"; }
            case Rags_Female_Light_Ponytail_Brown:              { return "Rags-Female-Light-Ponytail-Brown"; }
            case Rags_Female_Light_Ponytail_Brunette:           { return "Rags-Female-Light-Ponytail-Brunette"; }
            case Rags_Female_Light_Ponytail_Brunette2:          { return "Rags-Female-Light-Ponytail-Brunette2"; }
            case Rags_Female_Light_Ponytail_LightBlonde2:       { return "Rags-Female-Light-Ponytail-LightBlonde2"; }
            case Rags_Female_Light_Unkempt_Black:               { return "Rags-Female-Light-Unkempt-Black"; }
            case Rags_Female_Light_Unkempt_Blonde2:             { return "Rags-Female-Light-Unkempt-Blonde2"; }
            case Rags_Female_Light_Unkempt_Brown:               { return "Rags-Female-Light-Unkempt-Brown"; }
            case Rags_Female_Light_Unkempt_Brunette:            { return "Rags-Female-Light-Unkempt-Brunette"; }
            case Rags_Female_Light_Unkempt_Brunette2:           { return "Rags-Female-Light-Unkempt-Brunette2"; }
            case Rags_Female_Light_Unkempt_LightBlonde2:        { return "Rags-Female-Light-Unkempt-LightBlonde2"; }
            case Rags_Male_Dark_Bedhead_Brunette:               { return "Rags-Male-Dark-Bedhead-Brunette"; }
            case Rags_Male_Dark_Bedhead_Raven:                  { return "Rags-Male-Dark-Bedhead-Raven"; }
            case Rags_Male_Dark_Messy2_Brunette:                { return "Rags-Male-Dark-Messy2-Brunette"; }
            case Rags_Male_Dark_Messy2_Raven:                   { return "Rags-Male-Dark-Messy2-Raven"; }
            case Rags_Male_Dark_Parted_Brunette:                { return "Rags-Male-Dark-Parted-Brunette"; }
            case Rags_Male_Dark_Parted_Raven:                   { return "Rags-Male-Dark-Parted-Raven"; }
            case Rags_Male_Dark_Plain_Brunette:                 { return "Rags-Male-Dark-Plain-Brunette"; }
            case Rags_Male_Dark_Plain_Raven:                    { return "Rags-Male-Dark-Plain-Raven"; }
            case Rags_Male_Light_Bedhead_Blonde:                { return "Rags-Male-Light-Bedhead-Blonde"; }
            case Rags_Male_Light_Bedhead_Brunette:              { return "Rags-Male-Light-Bedhead-Brunette"; }
            case Rags_Male_Light_Bedhead_Raven:                 { return "Rags-Male-Light-Bedhead-Raven"; }
            case Rags_Male_Light_Bedhead_White:                 { return "Rags-Male-Light-Bedhead-White"; }
            case Rags_Male_Light_Messy2_Blonde:                 { return "Rags-Male-Light-Messy2-Blonde"; }
            case Rags_Male_Light_Messy2_Brunette:               { return "Rags-Male-Light-Messy2-Brunette"; }
            case Rags_Male_Light_Messy2_Raven:                  { return "Rags-Male-Light-Messy2-Raven"; }
            case Rags_Male_Light_Messy2_White:                  { return "Rags-Male-Light-Messy2-White"; }
            case Rags_Male_Light_Parted_Blonde:                 { return "Rags-Male-Light-Parted-Blonde"; }
            case Rags_Male_Light_Parted_Brunette:               { return "Rags-Male-Light-Parted-Brunette"; }
            case Rags_Male_Light_Parted_Raven:                  { return "Rags-Male-Light-Parted-Raven"; }
            case Rags_Male_Light_Parted_White:                  { return "Rags-Male-Light-Parted-White"; }
            case Rags_Male_Light_Plain_Blonde:                  { return "Rags-Male-Light-Plain-Blonde"; }
            case Rags_Male_Light_Plain_Brunette:                { return "Rags-Male-Light-Plain-Brunette"; }
            case Rags_Male_Light_Plain_Raven:                   { return "Rags-Male-Light-Plain-Raven"; }
            case Rags_Male_Light_Plain_White:                   { return "Rags-Male-Light-Plain-White"; }
            case Robes_Male_Dark_Bedhead_Brunette:              { return "Robes-Male-Dark-Bedhead-Brunette"; }
            case Robes_Male_Dark_Bedhead_Raven:                 { return "Robes-Male-Dark-Bedhead-Raven"; }
            case Robes_Male_Dark_Messy2_Brunette:               { return "Robes-Male-Dark-Messy2-Brunette"; }
            case Robes_Male_Dark_Messy2_Raven:                  { return "Robes-Male-Dark-Messy2-Raven"; }
            case Robes_Male_Dark_Parted_Brunette:               { return "Robes-Male-Dark-Parted-Brunette"; }
            case Robes_Male_Dark_Parted_Raven:                  { return "Robes-Male-Dark-Parted-Raven"; }
            case Robes_Male_Dark_Plain_Brunette:                { return "Robes-Male-Dark-Plain-Brunette"; }
            case Robes_Male_Dark_Plain_Raven:                   { return "Robes-Male-Dark-Plain-Raven"; }
            case Robes_Male_Light_Bedhead_Blonde:               { return "Robes-Male-Light-Bedhead-Blonde"; }
            case Robes_Male_Light_Bedhead_Brunette:             { return "Robes-Male-Light-Bedhead-Brunette"; }
            case Robes_Male_Light_Bedhead_Raven:                { return "Robes-Male-Light-Bedhead-Raven"; }
            case Robes_Male_Light_Bedhead_White:                { return "Robes-Male-Light-Bedhead-White"; }
            case Robes_Male_Light_Messy2_Blonde:                { return "Robes-Male-Light-Messy2-Blonde"; }
            case Robes_Male_Light_Messy2_Brunette:              { return "Robes-Male-Light-Messy2-Brunette"; }
            case Robes_Male_Light_Messy2_Raven:                 { return "Robes-Male-Light-Messy2-Raven"; }
            case Robes_Male_Light_Messy2_White:                 { return "Robes-Male-Light-Messy2-White"; }
            case Robes_Male_Light_Parted_Blonde:                { return "Robes-Male-Light-Parted-Blonde"; }
            case Robes_Male_Light_Parted_Brunette:              { return "Robes-Male-Light-Parted-Brunette"; }
            case Robes_Male_Light_Parted_Raven:                 { return "Robes-Male-Light-Parted-Raven"; }
            case Robes_Male_Light_Parted_White:                 { return "Robes-Male-Light-Parted-White"; }
            case Robes_Male_Light_Plain_Blonde:                 { return "Robes-Male-Light-Plain-Blonde"; }
            case Robes_Male_Light_Plain_Brunette:               { return "Robes-Male-Light-Plain-Brunette"; }
            case Robes_Male_Light_Plain_Raven:                  { return "Robes-Male-Light-Plain-Raven"; }
            case Robes_Male_Light_Plain_White:                  { return "Robes-Male-Light-Plain-White"; }
            case Robes_Whitebeard:                              { return "Robes-Whitebeard"; }
            case Tunic_Female_Dark_Bangsshort_Black:            { return "Tunic-Female-Dark-Bangsshort-Black"; }
            case Tunic_Female_Dark_Bangsshort_Brown:            { return "Tunic-Female-Dark-Bangsshort-Brown"; }
            case Tunic_Female_Dark_Bangsshort_Brunette:         { return "Tunic-Female-Dark-Bangsshort-Brunette"; }
            case Tunic_Female_Dark_Loose_Black:                 { return "Tunic-Female-Dark-Loose-Black"; }
            case Tunic_Female_Dark_Loose_Brown:                 { return "Tunic-Female-Dark-Loose-Brown"; }
            case Tunic_Female_Dark_Loose_Brunette:              { return "Tunic-Female-Dark-Loose-Brunette"; }
            case Tunic_Female_Dark_Ponytail_Black:              { return "Tunic-Female-Dark-Ponytail-Black"; }
            case Tunic_Female_Dark_Ponytail_Brown:              { return "Tunic-Female-Dark-Ponytail-Brown"; }
            case Tunic_Female_Dark_Ponytail_Brunette:           { return "Tunic-Female-Dark-Ponytail-Brunette"; }
            case Tunic_Female_Dark_Unkempt_Black:               { return "Tunic-Female-Dark-Unkempt-Black"; }
            case Tunic_Female_Dark_Unkempt_Brown:               { return "Tunic-Female-Dark-Unkempt-Brown"; }
            case Tunic_Female_Dark_Unkempt_Brunette:            { return "Tunic-Female-Dark-Unkempt-Brunette"; }
            case Tunic_Female_Light_Bangsshort_Black:           { return "Tunic-Female-Light-Bangsshort-Black"; }
            case Tunic_Female_Light_Bangsshort_Blonde2:         { return "Tunic-Female-Light-Bangsshort-Blonde2"; }
            case Tunic_Female_Light_Bangsshort_Brown:           { return "Tunic-Female-Light-Bangsshort-Brown"; }
            case Tunic_Female_Light_Bangsshort_Brunette:        { return "Tunic-Female-Light-Bangsshort-Brunette"; }
            case Tunic_Female_Light_Bangsshort_Brunette2:       { return "Tunic-Female-Light-Bangsshort-Brunette2"; }
            case Tunic_Female_Light_Bangsshort_LightBlonde2:    { return "Tunic-Female-Light-Bangsshort-LightBlonde2"; }
            case Tunic_Female_Light_Loose_Black:                { return "Tunic-Female-Light-Loose-Black"; }
            case Tunic_Female_Light_Loose_Blonde2:              { return "Tunic-Female-Light-Loose-Blonde2"; }
            case Tunic_Female_Light_Loose_Brown:                { return "Tunic-Female-Light-Loose-Brown"; }
            case Tunic_Female_Light_Loose_Brunette:             { return "Tunic-Female-Light-Loose-Brunette"; }
            case Tunic_Female_Light_Loose_Brunette2:            { return "Tunic-Female-Light-Loose-Brunette2"; }
            case Tunic_Female_Light_Loose_LightBlonde2:         { return "Tunic-Female-Light-Loose-LightBlonde2"; }
            case Tunic_Female_Light_Ponytail_Black:             { return "Tunic-Female-Light-Ponytail-Black"; }
            case Tunic_Female_Light_Ponytail_Blonde2:           { return "Tunic-Female-Light-Ponytail-Blonde2"; }
            case Tunic_Female_Light_Ponytail_Brown:             { return "Tunic-Female-Light-Ponytail-Brown"; }
            case Tunic_Female_Light_Ponytail_Brunette:          { return "Tunic-Female-Light-Ponytail-Brunette"; }
            case Tunic_Female_Light_Ponytail_Brunette2:         { return "Tunic-Female-Light-Ponytail-Brunette2"; }
            case Tunic_Female_Light_Ponytail_LightBlonde2:      { return "Tunic-Female-Light-Ponytail-LightBlonde2"; }
            case Tunic_Female_Light_Unkempt_Black:              { return "Tunic-Female-Light-Unkempt-Black"; }
            case Tunic_Female_Light_Unkempt_Blonde2:            { return "Tunic-Female-Light-Unkempt-Blonde2"; }
            case Tunic_Female_Light_Unkempt_Brown:              { return "Tunic-Female-Light-Unkempt-Brown"; }
            case Tunic_Female_Light_Unkempt_Brunette:           { return "Tunic-Female-Light-Unkempt-Brunette"; }
            case Tunic_Female_Light_Unkempt_Brunette2:          { return "Tunic-Female-Light-Unkempt-Brunette2"; }
            case Tunic_Female_Light_Unkempt_LightBlonde2:       { return "Tunic-Female-Light-Unkempt-LightBlonde2"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "avatar::Avatar::ToString(Enum=" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool Avatar::IsPlayer(const Avatar::Enum E)
    {
        return ((E >= Avatar::Player_First) && (E <= Avatar::Player_Last));
    }


    const std::string Avatar::ImagePath(const Avatar::Enum E)
    {
        std::string keyStr{ ((Avatar::IsPlayer(E)) ?
            game::GameDataFile::Instance()->GetMediaPath("media-images-avatar-player") :
            game::GameDataFile::Instance()->GetMediaPath("media-images-avatar-nonplayer")) };

        keyStr += boost::algorithm::to_lower_copy( Avatar::ToString(E) );
        keyStr += ".png";

        return keyStr;
    }

}
}
