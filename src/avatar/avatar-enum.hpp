// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_AVATARENUM_HPP_INCLUDED
#define HEROESPATH_AVATAR_AVATARENUM_HPP_INCLUDED
//
// avatar_enum.hpp
//
#include "interact/interaction-text-enum.hpp"
#include "interact/statement.hpp"
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace avatar
{

    struct Avatar : public EnumBaseCounting<>
    {
        enum class NameEnum : EnumUnderlying_t
        {
            // player avatars
            Metal,
            Puck,
            Sara,

            // stereotypical young girl in a princess outfit
            Girl,

            // commoners
            Circus,
            Poor, // male only, leather scraps
            Rags, // leather tunic with ratty looking gray pants
            Common, // male only,
            Tunic, // female only, the only female in pants
            Pant, // male only, the only male in pants
            Maid, // female only, the bar wench
            Robes, // male only, brown full length robes
            Whitebeard, // male only, brown robes, single avatar

            // richer woman in a light blue dress
            Dress,

            // military
            Private,
            Private2nd,
            Private1st,
            Specialist,
            Corporal,
            Sergeant,
            Major,
            Knight,
            KnightGold,

            // tough guys
            Thug,
            ManAtArms,
            Gladiator,
            Iron,

            // bald in brown robes, no white guys
            Monk
        };

        enum Enum : EnumUnderlying_t
        {
            Metal_Female_Dark = 0, // Player_First
            Metal_Female_Light,
            Metal_Male_Dark,
            Metal_Male_Light,
            Puck_Female_Dark,
            Puck_Female_Light,
            Puck_Male_Dark,
            Puck_Male_Light,
            Sara_BlueWhite,
            Sara_Cyan,
            Sara_Green,
            Sara_White, // Player_Last
            Dress_Dark_Pixie_Black, // NonPlayer_First
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
            Dress_Light_Swoop_Gray,
            Girl_Dark_Loose_Black,
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
            Girl_Light_Unkempt_LightBlonde2,
            Leather_Circus1_Dark_Brunette,
            Leather_Circus1_Dark_Raven,
            Leather_Circus1_Dark_White,
            Leather_Circus1_Light_Blonde,
            Leather_Circus1_Light_Brunette,
            Leather_Circus1_Light_Raven,
            Leather_Circus2_Dark_Brunette,
            Leather_Circus2_Dark_Raven,
            Leather_Circus2_Dark_White,
            Leather_Circus2_Light_Blonde,
            Leather_Circus2_Light_Brunette,
            Leather_Circus2_Light_Raven,
            Leather_Common1_Dark_Brunette,
            Leather_Common1_Dark_Raven,
            Leather_Common1_Dark_White,
            Leather_Common1_Light_Blonde,
            Leather_Common1_Light_Brunette,
            Leather_Common1_Light_Raven,
            Leather_Common2_Dark_Brunette,
            Leather_Common2_Dark_Raven,
            Leather_Common2_Dark_White,
            Leather_Common2_Light_Blonde,
            Leather_Common2_Light_Brunette,
            Leather_Common2_Light_Raven,
            Leather_Common3_Dark_Brunette,
            Leather_Common3_Dark_Raven,
            Leather_Common3_Dark_White,
            Leather_Common3_Light_Blonde,
            Leather_Common3_Light_Brunette,
            Leather_Common3_Light_Raven,
            Leather_Corporal1_Dark,
            Leather_Corporal1_Light,
            Leather_Corporal2_Dark,
            Leather_Corporal2_Light,
            Leather_IronArms_Dark_Brunette,
            Leather_IronArms_Dark_Raven,
            Leather_IronArms_Dark_White,
            Leather_IronArms_Light_Blonde,
            Leather_IronArms_Light_Brunette,
            Leather_IronArms_Light_Raven,
            Leather_IronRich_Dark_Brunette,
            Leather_IronRich_Dark_Raven,
            Leather_IronRich_Dark_White,
            Leather_IronRich_Light_Blonde,
            Leather_IronRich_Light_Brunette,
            Leather_IronRich_Light_Raven,
            Leather_Poor_Dark_Brunette,
            Leather_Poor_Dark_Raven,
            Leather_Poor_Dark_White,
            Leather_Poor_Light_Blonde,
            Leather_Poor_Light_Brunette,
            Leather_Poor_Light_Raven,
            Leather_Private1_Dark_Brunette,
            Leather_Private1_Dark_Raven,
            Leather_Private1_Dark_White,
            Leather_Private1_Light_Blonde,
            Leather_Private1_Light_Brunette,
            Leather_Private1_Light_Raven,
            Leather_Private2_Dark_Brunette,
            Leather_Private2_Dark_Raven,
            Leather_Private2_Dark_White,
            Leather_Private2_Light_Blonde,
            Leather_Private2_Light_Brunette,
            Leather_Private2_Light_Raven,
            Leather_Private3_Dark_Brunette,
            Leather_Private3_Dark_Raven,
            Leather_Private3_Dark_White,
            Leather_Private3_Light_Blonde,
            Leather_Private3_Light_Brunette,
            Leather_Private3_Light_Raven,
            Leather_Private4_Dark_Brunette,
            Leather_Private4_Dark_Raven,
            Leather_Private4_Dark_White,
            Leather_Private4_Light_Blonde,
            Leather_Private4_Light_Brunette,
            Leather_Private4_Light_Raven,
            Leather_Private2nd1_Dark,
            Leather_Private2nd1_Light,
            Leather_Private2nd2_Dark,
            Leather_Private2nd2_Light,
            Leather_Private2nd3_Dark,
            Leather_Private2nd3_Light,
            Leather_Private2nd4_Dark,
            Leather_Private2nd4_Light,
            Leather_Private1st1_Dark,
            Leather_Private1st1_Light,
            Leather_Private1st2_Dark,
            Leather_Private1st2_Light,
            Leather_Private1st3_Dark,
            Leather_Private1st3_Light,
            Leather_Private1st4_Dark,
            Leather_Private1st4_Light,
            Leather_Specialist1_Dark,
            Leather_Specialist1_Light,
            Leather_Specialist2_Dark,
            Leather_Specialist2_Light,
            Leather_Thug1_Dark_Brunette,
            Leather_Thug1_Dark_Raven,
            Leather_Thug1_Dark_White,
            Leather_Thug1_Light_Blonde,
            Leather_Thug1_Light_Brunette,
            Leather_Thug1_Light_Raven,
            Leather_Thug2_Dark_Brunette,
            Leather_Thug2_Dark_Raven,
            Leather_Thug2_Dark_White,
            Leather_Thug2_Light_Blonde,
            Leather_Thug2_Light_Brunette,
            Leather_Thug2_Light_Raven,
            Maid_Dark_Pixie_Black,
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
            Maid_Light_Swoop_Brunette2,
            Monk_BeltLeather_Dark_bracer,
            Monk_BeltLeather_Dark_nobracer,
            Monk_BeltLeather_Tanned2_bracer,
            Monk_BeltLeather_Tanned2_nobracer,
            Monk_Beltnone_Dark_bracer,
            Monk_Beltnone_Dark_nobracer,
            Monk_Beltnone_Tanned2_bracer,
            Monk_Beltnone_Tanned2_nobracer,
            Monk_Beltwhite_Dark_bracer,
            Monk_Beltwhite_Dark_nobracer,
            Monk_Beltwhite_Tanned2_bracer,
            Monk_Beltwhite_Tanned2_nobracer,
            Monk_Shirt_Beltnone_Dark,
            Monk_Shirt_Beltnone_Tanned2,
            Monk_Shirt_Beltwhite_Dark,
            Monk_Shirt_Beltwhite_Tanned2,
            Pants_Male_Dark_Bedhead_Brunette,
            Pants_Male_Dark_Bedhead_Raven,
            Pants_Male_Dark_Messy2_Brunette,
            Pants_Male_Dark_Messy2_Raven,
            Pants_Male_Dark_Parted_Brunette,
            Pants_Male_Dark_Parted_Raven,
            Pants_Male_Dark_Plain_Brunette,
            Pants_Male_Dark_Plain_Raven,
            Pants_Male_Light_Bedhead_Blonde,
            Pants_Male_Light_Bedhead_Brunette,
            Pants_Male_Light_Bedhead_Raven,
            Pants_Male_Light_Bedhead_White,
            Pants_Male_Light_Messy2_Blonde,
            Pants_Male_Light_Messy2_Brunette,
            Pants_Male_Light_Messy2_Raven,
            Pants_Male_Light_Messy2_White,
            Pants_Male_Light_Parted_Blonde,
            Pants_Male_Light_Parted_Brunette,
            Pants_Male_Light_Parted_Raven,
            Pants_Male_Light_Parted_White,
            Pants_Male_Light_Plain_Blonde,
            Pants_Male_Light_Plain_Brunette,
            Pants_Male_Light_Plain_Raven,
            Pants_Male_Light_Plain_White,
            Plate_Gladiator_Dark_Brunette,
            Plate_Gladiator_Dark_Raven,
            Plate_Gladiator_Dark_White,
            Plate_Gladiator_Light_Blonde,
            Plate_Gladiator_Light_Brunette,
            Plate_Gladiator_Light_Raven,
            Plate_Knight,
            Plate_Knight_Gold,
            Plate_Major_Dark,
            Plate_Major_Light,
            Plate_ManAtArms_Dark_Brunette,
            Plate_ManAtArms_Dark_Raven,
            Plate_ManAtArms_Dark_White,
            Plate_ManAtArms_Light_Blonde,
            Plate_ManAtArms_Light_Brunette,
            Plate_ManAtArms_Light_Raven,
            Plate_Sergeant_Dark,
            Plate_Sergeant_Light,
            Rags_Female_Dark_Bangsshort_Black,
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
            Rags_Male_Light_Plain_White,
            Robes_Male_Dark_Bedhead_Brunette,
            Robes_Male_Dark_Bedhead_Raven,
            Robes_Male_Dark_Messy2_Brunette,
            Robes_Male_Dark_Messy2_Raven,
            Robes_Male_Dark_Parted_Brunette,
            Robes_Male_Dark_Parted_Raven,
            Robes_Male_Dark_Plain_Brunette,
            Robes_Male_Dark_Plain_Raven,
            Robes_Male_Light_Bedhead_Blonde,
            Robes_Male_Light_Bedhead_Brunette,
            Robes_Male_Light_Bedhead_Raven,
            Robes_Male_Light_Bedhead_White,
            Robes_Male_Light_Messy2_Blonde,
            Robes_Male_Light_Messy2_Brunette,
            Robes_Male_Light_Messy2_Raven,
            Robes_Male_Light_Messy2_White,
            Robes_Male_Light_Parted_Blonde,
            Robes_Male_Light_Parted_Brunette,
            Robes_Male_Light_Parted_Raven,
            Robes_Male_Light_Parted_White,
            Robes_Male_Light_Plain_Blonde,
            Robes_Male_Light_Plain_Brunette,
            Robes_Male_Light_Plain_Raven,
            Robes_Male_Light_Plain_White,
            Robes_Whitebeard,
            Tunic_Female_Dark_Bangsshort_Black,
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
            Tunic_Female_Light_Unkempt_LightBlonde2, // NonPlayer_Last
            Count
        };

        static constexpr Enum Player_First = Metal_Female_Dark;
        static constexpr Enum Player_Last = Sara_White;
        static constexpr Enum NonPlayer_First = Dress_Dark_Pixie_Black;
        static constexpr Enum NonPlayer_Last = Tunic_Female_Light_Unkempt_LightBlonde2;

        static constexpr bool IsPlayer(const NameEnum ENUM) noexcept
        {
            return (
                (ENUM == NameEnum::Metal) || (ENUM == NameEnum::Puck) || (ENUM == NameEnum::Sara));
        }

        static const std::vector<Enum> Avatars(const NameEnum);

        static constexpr float Scale(const NameEnum ENUM) noexcept
        {
            if (NameEnum::Girl == ENUM)
            {
                return 0.75f;
            }
            else
            {
                return 1.0f;
            }
        }

        static constexpr interact::talk::Category TalkCategory(const NameEnum TYPE) noexcept
        {
            switch (TYPE)
            {
                case NameEnum::Metal:
                case NameEnum::Puck:
                case NameEnum::Circus:
                case NameEnum::Poor:
                case NameEnum::Rags:
                case NameEnum::Common:
                case NameEnum::Tunic:
                case NameEnum::Pant:
                case NameEnum::Maid:
                case NameEnum::Robes:
                case NameEnum::Whitebeard:
                case NameEnum::Dress:
                case NameEnum::Thug:
                case NameEnum::ManAtArms:
                case NameEnum::Gladiator:
                case NameEnum::Iron:
                case NameEnum::Sara: return interact::talk::Category::Common;

                case NameEnum::Girl: return interact::talk::Category::Child;

                case NameEnum::Private:
                case NameEnum::Private2nd:
                case NameEnum::Private1st:
                case NameEnum::Specialist:
                case NameEnum::Corporal:
                case NameEnum::Sergeant:
                case NameEnum::Major:
                case NameEnum::Knight:
                case NameEnum::KnightGold: return interact::talk::Category::Guard;

                case NameEnum::Monk: return interact::talk::Category::Monk;

                default: return interact::talk::Category::Common;
            }
        }

        static constexpr bool IsPlayer(const Avatar::Enum ENUM) noexcept
        {
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4296)
#endif
            return ((ENUM >= Player_First) && (ENUM <= Player_Last));
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        }

        static const std::string ImagePath(const Enum);

        static constexpr NameEnum Name(const Avatar::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Metal_Female_Dark:
                case Metal_Female_Light:
                case Metal_Male_Dark:
                case Metal_Male_Light: return NameEnum::Metal;

                case Puck_Female_Dark:
                case Puck_Female_Light:
                case Puck_Male_Dark:
                case Puck_Male_Light: return NameEnum::Puck;

                case Sara_BlueWhite:
                case Sara_Cyan:
                case Sara_Green:
                case Sara_White: return NameEnum::Sara;

                case Dress_Dark_Pixie_Black:
                case Dress_Dark_Pixie_Brown:
                case Dress_Dark_Pixie_Brunette:
                case Dress_Dark_Pixie_Gray:
                case Dress_Dark_Pixie_WhiteBlonde2:
                case Dress_Dark_Shoulderl_Black:
                case Dress_Dark_Shoulderl_Brown:
                case Dress_Dark_Shoulderl_Brunette:
                case Dress_Dark_Shoulderl_Gray:
                case Dress_Dark_Shoulderl_WhiteBlonde2:
                case Dress_Dark_Shoulderr_Brown:
                case Dress_Dark_Shoulderr_Black:
                case Dress_Dark_Shoulderr_Brunette:
                case Dress_Dark_Shoulderr_Gray:
                case Dress_Dark_Shoulderr_WhiteBlonde2:
                case Dress_Dark_Swoop_Black:
                case Dress_Dark_Swoop_Brown:
                case Dress_Dark_Swoop_Brunette:
                case Dress_Dark_Swoop_Gray:
                case Dress_Dark_Swoop_WhiteBlonde2:
                case Dress_Light_Pixie_Black:
                case Dress_Light_Pixie_Blonde2:
                case Dress_Light_Pixie_Brown:
                case Dress_Light_Pixie_Brunette:
                case Dress_Light_Pixie_Brunette2:
                case Dress_Light_Pixie_Gray:
                case Dress_Light_Shoulderl_Black:
                case Dress_Light_Shoulderl_Blonde2:
                case Dress_Light_Shoulderl_Brown:
                case Dress_Light_Shoulderl_Brunette:
                case Dress_Light_Shoulderl_Brunette2:
                case Dress_Light_Shoulderl_Gray:
                case Dress_Light_Shoulderr_Black:
                case Dress_Light_Shoulderr_Blonde2:
                case Dress_Light_Shoulderr_Brown:
                case Dress_Light_Shoulderr_Brunette:
                case Dress_Light_Shoulderr_Brunette2:
                case Dress_Light_Shoulderr_Gray:
                case Dress_Light_Swoop_Black:
                case Dress_Light_Swoop_Blonde2:
                case Dress_Light_Swoop_Brown:
                case Dress_Light_Swoop_Brunette:
                case Dress_Light_Swoop_Brunette2:
                case Dress_Light_Swoop_Gray: return NameEnum::Dress;

                case Girl_Dark_Loose_Black:
                case Girl_Dark_Loose_Brown:
                case Girl_Dark_Loose_Brunette:
                case Girl_Dark_Loose_Brunette2:
                case Girl_Dark_Loose_WhiteBlonde2:
                case Girl_Dark_Ponytail2_Black:
                case Girl_Dark_Ponytail2_Brown:
                case Girl_Dark_Ponytail2_Brunette:
                case Girl_Dark_Ponytail2_Brunette2:
                case Girl_Dark_Ponytail2_WhiteBlonde2:
                case Girl_Dark_Princess_Black:
                case Girl_Dark_Princess_Brown:
                case Girl_Dark_Princess_Brunette:
                case Girl_Dark_Princess_Brunette2:
                case Girl_Dark_Princess_WhiteBlonde2:
                case Girl_Dark_Unkempt_Black:
                case Girl_Dark_Unkempt_Brown:
                case Girl_Dark_Unkempt_Brunette:
                case Girl_Dark_Unkempt_Brunette2:
                case Girl_Dark_Unkempt_WhiteBlonde2:
                case Girl_Light_Loose_Blonde:
                case Girl_Light_Loose_Blonde2:
                case Girl_Light_Loose_Brown:
                case Girl_Light_Loose_Brunette:
                case Girl_Light_Loose_Brunette2:
                case Girl_Light_Loose_DarkBlonde:
                case Girl_Light_Loose_LightBlonde:
                case Girl_Light_Loose_LightBlonde2:
                case Girl_Light_Ponytail2_Blonde:
                case Girl_Light_Ponytail2_Blonde2:
                case Girl_Light_Ponytail2_Brown:
                case Girl_Light_Ponytail2_Brunette:
                case Girl_Light_Ponytail2_Brunette2:
                case Girl_Light_Ponytail2_DarkBlonde:
                case Girl_Light_Ponytail2_LightBlonde:
                case Girl_Light_Ponytail2_LightBlonde2:
                case Girl_Light_Princess_Blonde:
                case Girl_Light_Princess_Blonde2:
                case Girl_Light_Princess_Brown:
                case Girl_Light_Princess_Brunette:
                case Girl_Light_Princess_Brunette2:
                case Girl_Light_Princess_DarkBlonde:
                case Girl_Light_Princess_LightBlonde:
                case Girl_Light_Princess_LightBlonde2:
                case Girl_Light_Unkempt_Blonde:
                case Girl_Light_Unkempt_Blonde2:
                case Girl_Light_Unkempt_Brown:
                case Girl_Light_Unkempt_Brunette:
                case Girl_Light_Unkempt_Brunette2:
                case Girl_Light_Unkempt_DarkBlonde:
                case Girl_Light_Unkempt_LightBlonde:
                case Girl_Light_Unkempt_LightBlonde2: return NameEnum::Girl;

                case Leather_Circus1_Dark_Brunette:
                case Leather_Circus1_Dark_Raven:
                case Leather_Circus1_Dark_White:
                case Leather_Circus1_Light_Blonde:
                case Leather_Circus1_Light_Brunette:
                case Leather_Circus1_Light_Raven:
                case Leather_Circus2_Dark_Brunette:
                case Leather_Circus2_Dark_Raven:
                case Leather_Circus2_Dark_White:
                case Leather_Circus2_Light_Blonde:
                case Leather_Circus2_Light_Brunette:
                case Leather_Circus2_Light_Raven: return NameEnum::Circus;

                case Leather_Common1_Dark_Brunette:
                case Leather_Common1_Dark_Raven:
                case Leather_Common1_Dark_White:
                case Leather_Common1_Light_Blonde:
                case Leather_Common1_Light_Brunette:
                case Leather_Common1_Light_Raven:
                case Leather_Common2_Dark_Brunette:
                case Leather_Common2_Dark_Raven:
                case Leather_Common2_Dark_White:
                case Leather_Common2_Light_Blonde:
                case Leather_Common2_Light_Brunette:
                case Leather_Common2_Light_Raven:
                case Leather_Common3_Dark_Brunette:
                case Leather_Common3_Dark_Raven:
                case Leather_Common3_Dark_White:
                case Leather_Common3_Light_Blonde:
                case Leather_Common3_Light_Brunette:
                case Leather_Common3_Light_Raven: return NameEnum::Common;

                case Leather_Corporal1_Dark:
                case Leather_Corporal1_Light:
                case Leather_Corporal2_Dark:
                case Leather_Corporal2_Light: return NameEnum::Corporal;

                case Leather_IronArms_Dark_Brunette:
                case Leather_IronArms_Dark_Raven:
                case Leather_IronArms_Dark_White:
                case Leather_IronArms_Light_Blonde:
                case Leather_IronArms_Light_Brunette:
                case Leather_IronArms_Light_Raven:
                case Leather_IronRich_Dark_Brunette:
                case Leather_IronRich_Dark_Raven:
                case Leather_IronRich_Dark_White:
                case Leather_IronRich_Light_Blonde:
                case Leather_IronRich_Light_Brunette:
                case Leather_IronRich_Light_Raven: return NameEnum::Iron;

                case Leather_Poor_Dark_Brunette:
                case Leather_Poor_Dark_Raven:
                case Leather_Poor_Dark_White:
                case Leather_Poor_Light_Blonde:
                case Leather_Poor_Light_Brunette:
                case Leather_Poor_Light_Raven: return NameEnum::Poor;

                case Leather_Private1_Dark_Brunette:
                case Leather_Private1_Dark_Raven:
                case Leather_Private1_Dark_White:
                case Leather_Private1_Light_Blonde:
                case Leather_Private1_Light_Brunette:
                case Leather_Private1_Light_Raven:
                case Leather_Private2_Dark_Brunette:
                case Leather_Private2_Dark_Raven:
                case Leather_Private2_Dark_White:
                case Leather_Private2_Light_Blonde:
                case Leather_Private2_Light_Brunette:
                case Leather_Private2_Light_Raven:
                case Leather_Private3_Dark_Brunette:
                case Leather_Private3_Dark_Raven:
                case Leather_Private3_Dark_White:
                case Leather_Private3_Light_Blonde:
                case Leather_Private3_Light_Brunette:
                case Leather_Private3_Light_Raven:
                case Leather_Private4_Dark_Brunette:
                case Leather_Private4_Dark_Raven:
                case Leather_Private4_Dark_White:
                case Leather_Private4_Light_Blonde:
                case Leather_Private4_Light_Brunette:
                case Leather_Private4_Light_Raven: return NameEnum::Private;

                case Leather_Private2nd1_Dark:
                case Leather_Private2nd1_Light:
                case Leather_Private2nd2_Dark:
                case Leather_Private2nd2_Light:
                case Leather_Private2nd3_Dark:
                case Leather_Private2nd3_Light:
                case Leather_Private2nd4_Dark:
                case Leather_Private2nd4_Light: return NameEnum::Private2nd;

                case Leather_Private1st1_Dark:
                case Leather_Private1st1_Light:
                case Leather_Private1st2_Dark:
                case Leather_Private1st2_Light:
                case Leather_Private1st3_Dark:
                case Leather_Private1st3_Light:
                case Leather_Private1st4_Dark:
                case Leather_Private1st4_Light: return NameEnum::Private1st;

                case Leather_Specialist1_Dark:
                case Leather_Specialist1_Light:
                case Leather_Specialist2_Dark:
                case Leather_Specialist2_Light: return NameEnum::Specialist;

                case Leather_Thug1_Dark_Brunette:
                case Leather_Thug1_Dark_Raven:
                case Leather_Thug1_Dark_White:
                case Leather_Thug1_Light_Blonde:
                case Leather_Thug1_Light_Brunette:
                case Leather_Thug1_Light_Raven:
                case Leather_Thug2_Dark_Brunette:
                case Leather_Thug2_Dark_Raven:
                case Leather_Thug2_Dark_White:
                case Leather_Thug2_Light_Blonde:
                case Leather_Thug2_Light_Brunette:
                case Leather_Thug2_Light_Raven: return NameEnum::Thug;

                case Maid_Dark_Pixie_Black:
                case Maid_Dark_Pixie_Brown:
                case Maid_Dark_Pixie_Brunette:
                case Maid_Dark_Pixie_Gray:
                case Maid_Dark_Pixie_WhiteBlonde2:
                case Maid_Dark_Shoulderl_Black:
                case Maid_Dark_Shoulderl_Brown:
                case Maid_Dark_Shoulderl_Brunette:
                case Maid_Dark_Shoulderl_Gray:
                case Maid_Dark_Shoulderl_WhiteBlonde2:
                case Maid_Dark_Shoulderr_Black:
                case Maid_Dark_Shoulderr_Brown:
                case Maid_Dark_Shoulderr_Brunette:
                case Maid_Dark_Shoulderr_Gray:
                case Maid_Dark_Shoulderr_WhiteBlonde2:
                case Maid_Dark_Swoop_Black:
                case Maid_Dark_Swoop_Brown:
                case Maid_Dark_Swoop_Brunette:
                case Maid_Dark_Swoop_Gray:
                case Maid_Dark_Swoop_WhiteBlonde2:
                case Maid_Light_Pixie_Black:
                case Maid_Light_Pixie_Blonde2:
                case Maid_Light_Pixie_Brown:
                case Maid_Light_Pixie_Brunette:
                case Maid_Light_Pixie_Brunette2:
                case Maid_Light_Shoulderl_Black:
                case Maid_Light_Shoulderl_Blonde2:
                case Maid_Light_Shoulderl_Brown:
                case Maid_Light_Shoulderl_Brunette:
                case Maid_Light_Shoulderl_Brunette2:
                case Maid_Light_Shoulderr_Black:
                case Maid_Light_Shoulderr_Blonde2:
                case Maid_Light_Shoulderr_Brown:
                case Maid_Light_Shoulderr_Brunette:
                case Maid_Light_Shoulderr_Brunette2:
                case Maid_Light_Swoop_Black:
                case Maid_Light_Swoop_Blonde2:
                case Maid_Light_Swoop_Brown:
                case Maid_Light_Swoop_Brunette:
                case Maid_Light_Swoop_Brunette2: return NameEnum::Maid;

                case Monk_BeltLeather_Dark_bracer:
                case Monk_BeltLeather_Dark_nobracer:
                case Monk_BeltLeather_Tanned2_bracer:
                case Monk_BeltLeather_Tanned2_nobracer:
                case Monk_Beltnone_Dark_bracer:
                case Monk_Beltnone_Dark_nobracer:
                case Monk_Beltnone_Tanned2_bracer:
                case Monk_Beltnone_Tanned2_nobracer:
                case Monk_Beltwhite_Dark_bracer:
                case Monk_Beltwhite_Dark_nobracer:
                case Monk_Beltwhite_Tanned2_bracer:
                case Monk_Beltwhite_Tanned2_nobracer:
                case Monk_Shirt_Beltnone_Dark:
                case Monk_Shirt_Beltnone_Tanned2:
                case Monk_Shirt_Beltwhite_Dark:
                case Monk_Shirt_Beltwhite_Tanned2: return NameEnum::Monk;

                case Pants_Male_Dark_Bedhead_Brunette:
                case Pants_Male_Dark_Bedhead_Raven:
                case Pants_Male_Dark_Messy2_Brunette:
                case Pants_Male_Dark_Messy2_Raven:
                case Pants_Male_Dark_Parted_Brunette:
                case Pants_Male_Dark_Parted_Raven:
                case Pants_Male_Dark_Plain_Brunette:
                case Pants_Male_Dark_Plain_Raven:
                case Pants_Male_Light_Bedhead_Blonde:
                case Pants_Male_Light_Bedhead_Brunette:
                case Pants_Male_Light_Bedhead_Raven:
                case Pants_Male_Light_Bedhead_White:
                case Pants_Male_Light_Messy2_Blonde:
                case Pants_Male_Light_Messy2_Brunette:
                case Pants_Male_Light_Messy2_Raven:
                case Pants_Male_Light_Messy2_White:
                case Pants_Male_Light_Parted_Blonde:
                case Pants_Male_Light_Parted_Brunette:
                case Pants_Male_Light_Parted_Raven:
                case Pants_Male_Light_Parted_White:
                case Pants_Male_Light_Plain_Blonde:
                case Pants_Male_Light_Plain_Brunette:
                case Pants_Male_Light_Plain_Raven:
                case Pants_Male_Light_Plain_White: return NameEnum::Pant;

                case Plate_Gladiator_Dark_Brunette:
                case Plate_Gladiator_Dark_Raven:
                case Plate_Gladiator_Dark_White:
                case Plate_Gladiator_Light_Blonde:
                case Plate_Gladiator_Light_Brunette:
                case Plate_Gladiator_Light_Raven: return NameEnum::Gladiator;

                case Plate_Knight: return NameEnum::Knight;

                case Plate_Knight_Gold: return NameEnum::KnightGold;

                case Plate_Major_Dark:
                case Plate_Major_Light: return NameEnum::Major;

                case Plate_ManAtArms_Dark_Brunette:
                case Plate_ManAtArms_Dark_Raven:
                case Plate_ManAtArms_Dark_White:
                case Plate_ManAtArms_Light_Blonde:
                case Plate_ManAtArms_Light_Brunette:
                case Plate_ManAtArms_Light_Raven: return NameEnum::ManAtArms;

                case Plate_Sergeant_Dark:
                case Plate_Sergeant_Light: return NameEnum::Sergeant;

                case Rags_Female_Dark_Bangsshort_Black:
                case Rags_Female_Dark_Bangsshort_Brown:
                case Rags_Female_Dark_Bangsshort_Brunette:
                case Rags_Female_Dark_Loose_Black:
                case Rags_Female_Dark_Loose_Brown:
                case Rags_Female_Dark_Loose_Brunette:
                case Rags_Female_Dark_Ponytail_Black:
                case Rags_Female_Dark_Ponytail_Brown:
                case Rags_Female_Dark_Ponytail_Brunette:
                case Rags_Female_Dark_Unkempt_Black:
                case Rags_Female_Dark_Unkempt_Brown:
                case Rags_Female_Dark_Unkempt_Brunette:
                case Rags_Female_Light_Bangsshort_Black:
                case Rags_Female_Light_Bangsshort_Blonde2:
                case Rags_Female_Light_Bangsshort_Brown:
                case Rags_Female_Light_Bangsshort_Brunette:
                case Rags_Female_Light_Bangsshort_Brunette2:
                case Rags_Female_Light_Bangsshort_LightBlonde2:
                case Rags_Female_Light_Loose_Black:
                case Rags_Female_Light_Loose_Blonde2:
                case Rags_Female_Light_Loose_Brown:
                case Rags_Female_Light_Loose_Brunette:
                case Rags_Female_Light_Loose_Brunette2:
                case Rags_Female_Light_Loose_LightBlonde2:
                case Rags_Female_Light_Ponytail_Black:
                case Rags_Female_Light_Ponytail_Blonde2:
                case Rags_Female_Light_Ponytail_Brown:
                case Rags_Female_Light_Ponytail_Brunette:
                case Rags_Female_Light_Ponytail_Brunette2:
                case Rags_Female_Light_Ponytail_LightBlonde2:
                case Rags_Female_Light_Unkempt_Black:
                case Rags_Female_Light_Unkempt_Blonde2:
                case Rags_Female_Light_Unkempt_Brown:
                case Rags_Female_Light_Unkempt_Brunette:
                case Rags_Female_Light_Unkempt_Brunette2:
                case Rags_Female_Light_Unkempt_LightBlonde2:
                case Rags_Male_Dark_Bedhead_Brunette:
                case Rags_Male_Dark_Bedhead_Raven:
                case Rags_Male_Dark_Messy2_Brunette:
                case Rags_Male_Dark_Messy2_Raven:
                case Rags_Male_Dark_Parted_Brunette:
                case Rags_Male_Dark_Parted_Raven:
                case Rags_Male_Dark_Plain_Brunette:
                case Rags_Male_Dark_Plain_Raven:
                case Rags_Male_Light_Bedhead_Blonde:
                case Rags_Male_Light_Bedhead_Brunette:
                case Rags_Male_Light_Bedhead_Raven:
                case Rags_Male_Light_Bedhead_White:
                case Rags_Male_Light_Messy2_Blonde:
                case Rags_Male_Light_Messy2_Brunette:
                case Rags_Male_Light_Messy2_Raven:
                case Rags_Male_Light_Messy2_White:
                case Rags_Male_Light_Parted_Blonde:
                case Rags_Male_Light_Parted_Brunette:
                case Rags_Male_Light_Parted_Raven:
                case Rags_Male_Light_Parted_White:
                case Rags_Male_Light_Plain_Blonde:
                case Rags_Male_Light_Plain_Brunette:
                case Rags_Male_Light_Plain_Raven:
                case Rags_Male_Light_Plain_White: return NameEnum::Rags;

                case Robes_Male_Dark_Bedhead_Brunette:
                case Robes_Male_Dark_Bedhead_Raven:
                case Robes_Male_Dark_Messy2_Brunette:
                case Robes_Male_Dark_Messy2_Raven:
                case Robes_Male_Dark_Parted_Brunette:
                case Robes_Male_Dark_Parted_Raven:
                case Robes_Male_Dark_Plain_Brunette:
                case Robes_Male_Dark_Plain_Raven:
                case Robes_Male_Light_Bedhead_Blonde:
                case Robes_Male_Light_Bedhead_Brunette:
                case Robes_Male_Light_Bedhead_Raven:
                case Robes_Male_Light_Bedhead_White:
                case Robes_Male_Light_Messy2_Blonde:
                case Robes_Male_Light_Messy2_Brunette:
                case Robes_Male_Light_Messy2_Raven:
                case Robes_Male_Light_Messy2_White:
                case Robes_Male_Light_Parted_Blonde:
                case Robes_Male_Light_Parted_Brunette:
                case Robes_Male_Light_Parted_Raven:
                case Robes_Male_Light_Parted_White:
                case Robes_Male_Light_Plain_Blonde:
                case Robes_Male_Light_Plain_Brunette:
                case Robes_Male_Light_Plain_Raven:
                case Robes_Male_Light_Plain_White: return NameEnum::Robes;

                case Robes_Whitebeard: return NameEnum::Whitebeard;

                case Tunic_Female_Dark_Bangsshort_Black:
                case Tunic_Female_Dark_Bangsshort_Brown:
                case Tunic_Female_Dark_Bangsshort_Brunette:
                case Tunic_Female_Dark_Loose_Black:
                case Tunic_Female_Dark_Loose_Brown:
                case Tunic_Female_Dark_Loose_Brunette:
                case Tunic_Female_Dark_Ponytail_Black:
                case Tunic_Female_Dark_Ponytail_Brown:
                case Tunic_Female_Dark_Ponytail_Brunette:
                case Tunic_Female_Dark_Unkempt_Black:
                case Tunic_Female_Dark_Unkempt_Brown:
                case Tunic_Female_Dark_Unkempt_Brunette:
                case Tunic_Female_Light_Bangsshort_Black:
                case Tunic_Female_Light_Bangsshort_Blonde2:
                case Tunic_Female_Light_Bangsshort_Brown:
                case Tunic_Female_Light_Bangsshort_Brunette:
                case Tunic_Female_Light_Bangsshort_Brunette2:
                case Tunic_Female_Light_Bangsshort_LightBlonde2:
                case Tunic_Female_Light_Loose_Black:
                case Tunic_Female_Light_Loose_Blonde2:
                case Tunic_Female_Light_Loose_Brown:
                case Tunic_Female_Light_Loose_Brunette:
                case Tunic_Female_Light_Loose_Brunette2:
                case Tunic_Female_Light_Loose_LightBlonde2:
                case Tunic_Female_Light_Ponytail_Black:
                case Tunic_Female_Light_Ponytail_Blonde2:
                case Tunic_Female_Light_Ponytail_Brown:
                case Tunic_Female_Light_Ponytail_Brunette:
                case Tunic_Female_Light_Ponytail_Brunette2:
                case Tunic_Female_Light_Ponytail_LightBlonde2:
                case Tunic_Female_Light_Unkempt_Black:
                case Tunic_Female_Light_Unkempt_Blonde2:
                case Tunic_Female_Light_Unkempt_Brown:
                case Tunic_Female_Light_Unkempt_Brunette:
                case Tunic_Female_Light_Unkempt_Brunette2:

                case Tunic_Female_Light_Unkempt_LightBlonde2: return NameEnum::Tunic;

                case Count: return NameEnum::Monk;

                default: return NameEnum::Monk;
            }
        }

        using NameVec_t = std::vector<NameEnum>;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_AVATARENUM_HPP_INCLUDED
