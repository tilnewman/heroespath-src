// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// anim-enum.cpp
//
#include "avatar-enum.hpp"

#include "interact/statement.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

#include <stdexcept>

namespace heroespath
{
namespace avatar
{

    const std::vector<Avatar::Enum> Avatar::Avatars(const NameEnum ENUM)
    {
        switch (ENUM)
        {
            case NameEnum::Metal:
            {
                return { Metal_Female_Dark, Metal_Female_Light, Metal_Male_Dark, Metal_Male_Light };
            }
            case NameEnum::Puck:
            {
                return { Puck_Female_Dark, Puck_Female_Light, Puck_Male_Dark, Puck_Male_Light };
            }
            case NameEnum::Sara:
            {
                return { Sara_BlueWhite, Sara_Cyan, Sara_Green, Sara_White };
            }
            case NameEnum::Girl:
            {
                return { Girl_Dark_Loose_Black,
                         Girl_Dark_Loose_Brown,
                         Girl_Dark_Loose_Brunette,
                         Girl_Dark_Loose_Brunette2,
                         Girl_Dark_Loose_WhiteBlonde2,
                         Girl_Dark_Ponytail2_Black,
                         Girl_Dark_Ponytail2_Brown,
                         Girl_Dark_Ponytail2_Brunette,
                         Girl_Dark_Ponytail2_Brunette2,
                         Girl_Dark_Ponytail2_WhiteBlonde2,
                         Girl_Dark_Princess_Black,
                         Girl_Dark_Princess_Brown,
                         Girl_Dark_Princess_Brunette,
                         Girl_Dark_Princess_Brunette2,
                         Girl_Dark_Princess_WhiteBlonde2,
                         Girl_Dark_Unkempt_Black,
                         Girl_Dark_Unkempt_Brown,
                         Girl_Dark_Unkempt_Brunette,
                         Girl_Dark_Unkempt_Brunette2,
                         Girl_Dark_Unkempt_WhiteBlonde2,
                         Girl_Light_Loose_Blonde,
                         Girl_Light_Loose_Blonde2,
                         Girl_Light_Loose_Brown,
                         Girl_Light_Loose_Brunette,
                         Girl_Light_Loose_Brunette2,
                         Girl_Light_Loose_DarkBlonde,
                         Girl_Light_Loose_LightBlonde,
                         Girl_Light_Loose_LightBlonde2,
                         Girl_Light_Ponytail2_Blonde,
                         Girl_Light_Ponytail2_Blonde2,
                         Girl_Light_Ponytail2_Brown,
                         Girl_Light_Ponytail2_Brunette,
                         Girl_Light_Ponytail2_Brunette2,
                         Girl_Light_Ponytail2_DarkBlonde,
                         Girl_Light_Ponytail2_LightBlonde,
                         Girl_Light_Ponytail2_LightBlonde2,
                         Girl_Light_Princess_Blonde,
                         Girl_Light_Princess_Blonde2,
                         Girl_Light_Princess_Brown,
                         Girl_Light_Princess_Brunette,
                         Girl_Light_Princess_Brunette2,
                         Girl_Light_Princess_DarkBlonde,
                         Girl_Light_Princess_LightBlonde,
                         Girl_Light_Princess_LightBlonde2,
                         Girl_Light_Unkempt_Blonde,
                         Girl_Light_Unkempt_Blonde2,
                         Girl_Light_Unkempt_Brown,
                         Girl_Light_Unkempt_Brunette,
                         Girl_Light_Unkempt_Brunette2,
                         Girl_Light_Unkempt_DarkBlonde,
                         Girl_Light_Unkempt_LightBlonde,
                         Girl_Light_Unkempt_LightBlonde2 };
            }
            case NameEnum::Circus:
            {
                return { Leather_Circus1_Dark_Brunette,  Leather_Circus1_Dark_Raven,
                         Leather_Circus1_Dark_White,     Leather_Circus1_Light_Blonde,
                         Leather_Circus1_Light_Brunette, Leather_Circus1_Light_Raven,
                         Leather_Circus2_Dark_Brunette,  Leather_Circus2_Dark_Raven,
                         Leather_Circus2_Dark_White,     Leather_Circus2_Light_Blonde,
                         Leather_Circus2_Light_Brunette, Leather_Circus2_Light_Raven };
            }
            case NameEnum::Poor:
            {
                return { Leather_Poor_Dark_Brunette,  Leather_Poor_Dark_Raven,
                         Leather_Poor_Dark_White,     Leather_Poor_Light_Blonde,
                         Leather_Poor_Light_Brunette, Leather_Poor_Light_Raven };
            }
            case NameEnum::Rags:
            {
                return { Rags_Female_Dark_Bangsshort_Black,
                         Rags_Female_Dark_Bangsshort_Brown,
                         Rags_Female_Dark_Bangsshort_Brunette,
                         Rags_Female_Dark_Loose_Black,
                         Rags_Female_Dark_Loose_Brown,
                         Rags_Female_Dark_Loose_Brunette,
                         Rags_Female_Dark_Ponytail_Black,
                         Rags_Female_Dark_Ponytail_Brown,
                         Rags_Female_Dark_Ponytail_Brunette,
                         Rags_Female_Dark_Unkempt_Black,
                         Rags_Female_Dark_Unkempt_Brown,
                         Rags_Female_Dark_Unkempt_Brunette,
                         Rags_Female_Light_Bangsshort_Black,
                         Rags_Female_Light_Bangsshort_Blonde2,
                         Rags_Female_Light_Bangsshort_Brown,
                         Rags_Female_Light_Bangsshort_Brunette,
                         Rags_Female_Light_Bangsshort_Brunette2,
                         Rags_Female_Light_Bangsshort_LightBlonde2,
                         Rags_Female_Light_Loose_Black,
                         Rags_Female_Light_Loose_Blonde2,
                         Rags_Female_Light_Loose_Brown,
                         Rags_Female_Light_Loose_Brunette,
                         Rags_Female_Light_Loose_Brunette2,
                         Rags_Female_Light_Loose_LightBlonde2,
                         Rags_Female_Light_Ponytail_Black,
                         Rags_Female_Light_Ponytail_Blonde2,
                         Rags_Female_Light_Ponytail_Brown,
                         Rags_Female_Light_Ponytail_Brunette,
                         Rags_Female_Light_Ponytail_Brunette2,
                         Rags_Female_Light_Ponytail_LightBlonde2,
                         Rags_Female_Light_Unkempt_Black,
                         Rags_Female_Light_Unkempt_Blonde2,
                         Rags_Female_Light_Unkempt_Brown,
                         Rags_Female_Light_Unkempt_Brunette,
                         Rags_Female_Light_Unkempt_Brunette2,
                         Rags_Female_Light_Unkempt_LightBlonde2,
                         Rags_Male_Dark_Bedhead_Brunette,
                         Rags_Male_Dark_Bedhead_Raven,
                         Rags_Male_Dark_Messy2_Brunette,
                         Rags_Male_Dark_Messy2_Raven,
                         Rags_Male_Dark_Parted_Brunette,
                         Rags_Male_Dark_Parted_Raven,
                         Rags_Male_Dark_Plain_Brunette,
                         Rags_Male_Dark_Plain_Raven,
                         Rags_Male_Light_Bedhead_Blonde,
                         Rags_Male_Light_Bedhead_Brunette,
                         Rags_Male_Light_Bedhead_Raven,
                         Rags_Male_Light_Bedhead_White,
                         Rags_Male_Light_Messy2_Blonde,
                         Rags_Male_Light_Messy2_Brunette,
                         Rags_Male_Light_Messy2_Raven,
                         Rags_Male_Light_Messy2_White,
                         Rags_Male_Light_Parted_Blonde,
                         Rags_Male_Light_Parted_Brunette,
                         Rags_Male_Light_Parted_Raven,
                         Rags_Male_Light_Parted_White,
                         Rags_Male_Light_Plain_Blonde,
                         Rags_Male_Light_Plain_Brunette,
                         Rags_Male_Light_Plain_Raven,
                         Rags_Male_Light_Plain_White };
            }
            case NameEnum::Common:
            {
                return { Leather_Common1_Dark_Brunette,  Leather_Common1_Dark_Raven,
                         Leather_Common1_Dark_White,     Leather_Common1_Light_Blonde,
                         Leather_Common1_Light_Brunette, Leather_Common1_Light_Raven,
                         Leather_Common2_Dark_Brunette,  Leather_Common2_Dark_Raven,
                         Leather_Common2_Dark_White,     Leather_Common2_Light_Blonde,
                         Leather_Common2_Light_Brunette, Leather_Common2_Light_Raven,
                         Leather_Common3_Dark_Brunette,  Leather_Common3_Dark_Raven,
                         Leather_Common3_Dark_White,     Leather_Common3_Light_Blonde,
                         Leather_Common3_Light_Brunette, Leather_Common3_Light_Raven };
            }
            case NameEnum::Tunic:
            {
                return { Tunic_Female_Dark_Bangsshort_Black,
                         Tunic_Female_Dark_Bangsshort_Brown,
                         Tunic_Female_Dark_Bangsshort_Brunette,
                         Tunic_Female_Dark_Loose_Black,
                         Tunic_Female_Dark_Loose_Brown,
                         Tunic_Female_Dark_Loose_Brunette,
                         Tunic_Female_Dark_Ponytail_Black,
                         Tunic_Female_Dark_Ponytail_Brown,
                         Tunic_Female_Dark_Ponytail_Brunette,
                         Tunic_Female_Dark_Unkempt_Black,
                         Tunic_Female_Dark_Unkempt_Brown,
                         Tunic_Female_Dark_Unkempt_Brunette,
                         Tunic_Female_Light_Bangsshort_Black,
                         Tunic_Female_Light_Bangsshort_Blonde2,
                         Tunic_Female_Light_Bangsshort_Brown,
                         Tunic_Female_Light_Bangsshort_Brunette,
                         Tunic_Female_Light_Bangsshort_Brunette2,
                         Tunic_Female_Light_Bangsshort_LightBlonde2,
                         Tunic_Female_Light_Loose_Black,
                         Tunic_Female_Light_Loose_Blonde2,
                         Tunic_Female_Light_Loose_Brown,
                         Tunic_Female_Light_Loose_Brunette,
                         Tunic_Female_Light_Loose_Brunette2,
                         Tunic_Female_Light_Loose_LightBlonde2,
                         Tunic_Female_Light_Ponytail_Black,
                         Tunic_Female_Light_Ponytail_Blonde2,
                         Tunic_Female_Light_Ponytail_Brown,
                         Tunic_Female_Light_Ponytail_Brunette,
                         Tunic_Female_Light_Ponytail_Brunette2,
                         Tunic_Female_Light_Ponytail_LightBlonde2,
                         Tunic_Female_Light_Unkempt_Black,
                         Tunic_Female_Light_Unkempt_Blonde2,
                         Tunic_Female_Light_Unkempt_Brown,
                         Tunic_Female_Light_Unkempt_Brunette,
                         Tunic_Female_Light_Unkempt_Brunette2,
                         Tunic_Female_Light_Unkempt_LightBlonde2 };
            }
            case NameEnum::Pant:
            {
                return { Pants_Male_Dark_Bedhead_Brunette, Pants_Male_Dark_Bedhead_Raven,
                         Pants_Male_Dark_Messy2_Brunette,  Pants_Male_Dark_Messy2_Raven,
                         Pants_Male_Dark_Parted_Brunette,  Pants_Male_Dark_Parted_Raven,
                         Pants_Male_Dark_Plain_Brunette,   Pants_Male_Dark_Plain_Raven,
                         Pants_Male_Light_Bedhead_Blonde,  Pants_Male_Light_Bedhead_Brunette,
                         Pants_Male_Light_Bedhead_Raven,   Pants_Male_Light_Bedhead_White,
                         Pants_Male_Light_Messy2_Blonde,   Pants_Male_Light_Messy2_Brunette,
                         Pants_Male_Light_Messy2_Raven,    Pants_Male_Light_Messy2_White,
                         Pants_Male_Light_Parted_Blonde,   Pants_Male_Light_Parted_Brunette,
                         Pants_Male_Light_Parted_Raven,    Pants_Male_Light_Parted_White,
                         Pants_Male_Light_Plain_Blonde,    Pants_Male_Light_Plain_Brunette,
                         Pants_Male_Light_Plain_Raven,     Pants_Male_Light_Plain_White };
            }
            case NameEnum::Maid:
            {
                return { Maid_Dark_Pixie_Black,
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
            case NameEnum::Robes:
            {
                return { Robes_Male_Dark_Bedhead_Brunette, Robes_Male_Dark_Bedhead_Raven,
                         Robes_Male_Dark_Messy2_Brunette,  Robes_Male_Dark_Messy2_Raven,
                         Robes_Male_Dark_Parted_Brunette,  Robes_Male_Dark_Parted_Raven,
                         Robes_Male_Dark_Plain_Brunette,   Robes_Male_Dark_Plain_Raven,
                         Robes_Male_Light_Bedhead_Blonde,  Robes_Male_Light_Bedhead_Brunette,
                         Robes_Male_Light_Bedhead_Raven,   Robes_Male_Light_Bedhead_White,
                         Robes_Male_Light_Messy2_Blonde,   Robes_Male_Light_Messy2_Brunette,
                         Robes_Male_Light_Messy2_Raven,    Robes_Male_Light_Messy2_White,
                         Robes_Male_Light_Parted_Blonde,   Robes_Male_Light_Parted_Brunette,
                         Robes_Male_Light_Parted_Raven,    Robes_Male_Light_Parted_White,
                         Robes_Male_Light_Plain_Blonde,    Robes_Male_Light_Plain_Brunette,
                         Robes_Male_Light_Plain_Raven,     Robes_Male_Light_Plain_White };
            }
            case NameEnum::Whitebeard: return { Robes_Whitebeard };
            case NameEnum::Dress:
            {
                return { Dress_Dark_Pixie_Black,
                         Dress_Dark_Pixie_Brown,
                         Dress_Dark_Pixie_Brunette,
                         Dress_Dark_Pixie_Gray,
                         Dress_Dark_Pixie_WhiteBlonde2,
                         Dress_Dark_Shoulderl_Black,
                         Dress_Dark_Shoulderl_Brown,
                         Dress_Dark_Shoulderl_Brunette,
                         Dress_Dark_Shoulderl_Gray,
                         Dress_Dark_Shoulderl_WhiteBlonde2,
                         Dress_Dark_Shoulderr_Brown,
                         Dress_Dark_Shoulderr_Black,
                         Dress_Dark_Shoulderr_Brunette,
                         Dress_Dark_Shoulderr_Gray,
                         Dress_Dark_Shoulderr_WhiteBlonde2,
                         Dress_Dark_Swoop_Black,
                         Dress_Dark_Swoop_Brown,
                         Dress_Dark_Swoop_Brunette,
                         Dress_Dark_Swoop_Gray,
                         Dress_Dark_Swoop_WhiteBlonde2,
                         Dress_Light_Pixie_Black,
                         Dress_Light_Pixie_Blonde2,
                         Dress_Light_Pixie_Brown,
                         Dress_Light_Pixie_Brunette,
                         Dress_Light_Pixie_Brunette2,
                         Dress_Light_Pixie_Gray,
                         Dress_Light_Shoulderl_Black,
                         Dress_Light_Shoulderl_Blonde2,
                         Dress_Light_Shoulderl_Brown,
                         Dress_Light_Shoulderl_Brunette,
                         Dress_Light_Shoulderl_Brunette2,
                         Dress_Light_Shoulderl_Gray,
                         Dress_Light_Shoulderr_Black,
                         Dress_Light_Shoulderr_Blonde2,
                         Dress_Light_Shoulderr_Brown,
                         Dress_Light_Shoulderr_Brunette,
                         Dress_Light_Shoulderr_Brunette2,
                         Dress_Light_Shoulderr_Gray,
                         Dress_Light_Swoop_Black,
                         Dress_Light_Swoop_Blonde2,
                         Dress_Light_Swoop_Brown,
                         Dress_Light_Swoop_Brunette,
                         Dress_Light_Swoop_Brunette2,
                         Dress_Light_Swoop_Gray };
            }
            case NameEnum::Private:
            {
                return { Leather_Private1_Dark_Brunette,  Leather_Private1_Dark_Raven,
                         Leather_Private1_Dark_White,     Leather_Private1_Light_Blonde,
                         Leather_Private1_Light_Brunette, Leather_Private1_Light_Raven,
                         Leather_Private2_Dark_Brunette,  Leather_Private2_Dark_Raven,
                         Leather_Private2_Dark_White,     Leather_Private2_Light_Blonde,
                         Leather_Private2_Light_Brunette, Leather_Private2_Light_Raven,
                         Leather_Private3_Dark_Brunette,  Leather_Private3_Dark_Raven,
                         Leather_Private3_Dark_White,     Leather_Private3_Light_Blonde,
                         Leather_Private3_Light_Brunette, Leather_Private3_Light_Raven,
                         Leather_Private4_Dark_Brunette,  Leather_Private4_Dark_Raven,
                         Leather_Private4_Dark_White,     Leather_Private4_Light_Blonde,
                         Leather_Private4_Light_Brunette, Leather_Private4_Light_Raven };
            }
            case NameEnum::Private2nd:
            {
                return { Leather_Private2nd1_Dark, Leather_Private2nd1_Light,
                         Leather_Private2nd2_Dark, Leather_Private2nd2_Light,
                         Leather_Private2nd3_Dark, Leather_Private2nd3_Light,
                         Leather_Private2nd4_Dark, Leather_Private2nd4_Light };
            }
            case NameEnum::Private1st:
            {
                return { Leather_Private1st1_Dark, Leather_Private1st1_Light,
                         Leather_Private1st2_Dark, Leather_Private1st2_Light,
                         Leather_Private1st3_Dark, Leather_Private1st3_Light,
                         Leather_Private1st4_Dark, Leather_Private1st4_Light };
            }
            case NameEnum::Specialist:
            {
                return { Leather_Specialist1_Dark,
                         Leather_Specialist1_Light,
                         Leather_Specialist2_Dark,
                         Leather_Specialist2_Light };
            }
            case NameEnum::Corporal:
            {
                return { Leather_Corporal1_Dark,
                         Leather_Corporal1_Light,
                         Leather_Corporal2_Dark,
                         Leather_Corporal2_Light };
            }
            case NameEnum::Sergeant: return { Plate_Sergeant_Dark, Plate_Sergeant_Light };
            case NameEnum::Major: return { Plate_Major_Dark, Plate_Major_Light };
            case NameEnum::Knight: return { Plate_Knight };
            case NameEnum::KnightGold: return { Plate_Knight_Gold };
            case NameEnum::Thug:
            {
                return { Leather_Thug1_Dark_Brunette,  Leather_Thug1_Dark_Raven,
                         Leather_Thug1_Dark_White,     Leather_Thug1_Light_Blonde,
                         Leather_Thug1_Light_Brunette, Leather_Thug1_Light_Raven,
                         Leather_Thug2_Dark_Brunette,  Leather_Thug2_Dark_Raven,
                         Leather_Thug2_Dark_White,     Leather_Thug2_Light_Blonde,
                         Leather_Thug2_Light_Brunette, Leather_Thug2_Light_Raven };
            }
            case NameEnum::ManAtArms:
            {
                return { Plate_ManAtArms_Dark_Brunette,  Plate_ManAtArms_Dark_Raven,
                         Plate_ManAtArms_Dark_White,     Plate_ManAtArms_Light_Blonde,
                         Plate_ManAtArms_Light_Brunette, Plate_ManAtArms_Light_Raven };
            }
            case NameEnum::Gladiator:
            {
                return { Plate_Gladiator_Dark_Brunette,  Plate_Gladiator_Dark_Raven,
                         Plate_Gladiator_Dark_White,     Plate_Gladiator_Light_Blonde,
                         Plate_Gladiator_Light_Brunette, Plate_Gladiator_Light_Raven };
            }
            case NameEnum::Iron:
            {
                return { Leather_IronArms_Dark_Brunette,  Leather_IronArms_Dark_Raven,
                         Leather_IronArms_Dark_White,     Leather_IronArms_Light_Blonde,
                         Leather_IronArms_Light_Brunette, Leather_IronArms_Light_Raven,
                         Leather_IronRich_Dark_Brunette,  Leather_IronRich_Dark_Raven,
                         Leather_IronRich_Dark_White,     Leather_IronRich_Light_Blonde,
                         Leather_IronRich_Light_Brunette, Leather_IronRich_Light_Raven };
            }
            case NameEnum::Monk:
            {
                return { Monk_BeltLeather_Dark_bracer,    Monk_BeltLeather_Dark_nobracer,
                         Monk_BeltLeather_Tanned2_bracer, Monk_BeltLeather_Tanned2_nobracer,
                         Monk_Beltnone_Dark_bracer,       Monk_Beltnone_Dark_nobracer,
                         Monk_Beltnone_Tanned2_bracer,    Monk_Beltnone_Tanned2_nobracer,
                         Monk_Beltwhite_Dark_bracer,      Monk_Beltwhite_Dark_nobracer,
                         Monk_Beltwhite_Tanned2_bracer,   Monk_Beltwhite_Tanned2_nobracer,
                         Monk_Shirt_Beltnone_Dark,        Monk_Shirt_Beltnone_Tanned2,
                         Monk_Shirt_Beltwhite_Dark,       Monk_Shirt_Beltwhite_Tanned2 };
            }
            default:
            {
                return {};
            }
        }
    }

    const std::string Avatar::ImagePath(const Avatar::Enum ENUM)
    {
        std::string dirPathStr { misc::ConfigFile::Instance()->GetMediaPath(
            "media-image-avatar-dir") };

        return misc::filesystem::CombinePaths(
            dirPathStr, misc::ToLowerCopy(NAMEOF_ENUM(ENUM)) + ".png");
    }

} // namespace avatar
} // namespace heroespath
