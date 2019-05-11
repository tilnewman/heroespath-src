// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// body-type.cpp
//
#include "body-type.hpp"

#include "misc/log-macros.hpp"

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace creature
{

    BodyType::BodyType(
        const std::size_t NUM_HEADS,
        const std::size_t NUM_ARMS,
        const std::size_t NUM_LEGS,
        const std::size_t NUM_EYES,
        const bool HAS_FANGS,
        const bool HAS_CLAWS,
        const bool HAS_WINGS,
        const bool HAS_FINGERS,
        const bool HAS_TAIL,
        const bool HAS_SPIKES,
        const bool HAS_HORNS,
        const std::size_t NUM_TENTACLES,
        const bool HAS_BREATH)
        : has_wings_(HAS_WINGS)
        , has_fangs_(HAS_FANGS)
        , has_claws_(HAS_CLAWS)
        , has_fingers_(HAS_FINGERS)
        , has_tail_(HAS_TAIL)
        , has_spikes_(HAS_SPIKES)
        , has_horns_(HAS_HORNS)
        , has_breath_(HAS_BREATH)
        , num_arms_(NUM_ARMS)
        , num_legs_(NUM_LEGS)
        , num_heads_(NUM_HEADS)
        , num_eyes_(NUM_EYES)
        , num_tentacles_(NUM_TENTACLES)
    {}

    const std::string BodyType::ToString() const
    {
        static std::string cachedStr;

        if (cachedStr.empty())
        {
            cachedStr += 'A';

            if (0 == num_heads_)
            {
                cachedStr += " headless";
            }
            else
            {
                if (num_heads_ > 1)
                {
                    cachedStr += ' ';
                    cachedStr += std::to_string(num_heads_);
                    cachedStr += "-headed";

                    if (0 == num_eyes_)
                    {
                        cachedStr += " each with no eyes";
                    }
                    else if (num_eyes_ != 2)
                    {
                        cachedStr += " each with";
                        cachedStr += std::to_string(num_eyes_);
                        cachedStr += " eyes";
                    }
                }
                else
                {
                    if (0 == num_eyes_)
                    {
                        cachedStr += "n eyeless";
                    }
                    else if (num_eyes_ != 2)
                    {
                        cachedStr += ' ';
                        cachedStr += std::to_string(num_eyes_);
                        cachedStr += "-eyed";
                    }
                }
            }

            if (0 == num_arms_)
            {
                cachedStr += " armless";
            }
            else if (2 != num_arms_)
            {
                cachedStr += ' ';
                cachedStr += std::to_string(num_arms_);
                cachedStr += "-armed";
            }

            if (has_spikes_)
            {
                cachedStr += " spiked";
            }

            if (has_fangs_)
            {
                cachedStr += " fanged";
            }

            if (has_claws_)
            {
                cachedStr += " clawed";
            }

            if (has_horns_)
            {
                cachedStr += " horned";
            }

            if (has_breath_)
            {
                cachedStr += " with magical breath";
            }

            if (0 == num_legs_)
            {
                cachedStr += " legless";
            }
            else
            {
                if (IsBiped())
                {
                    cachedStr += " biped";
                }
                else
                {
                    if (IsQuadruped())
                    {
                        cachedStr += " quadruped";
                    }
                    else
                    {
                        cachedStr += ' ';
                        cachedStr += std::to_string(num_legs_);
                        cachedStr += "-legged creature";
                    }
                }
            }

            if (has_wings_)
            {
                cachedStr += " with wings";
            }

            if (has_tail_)
            {
                cachedStr += " with a tail";
            }

            if (num_tentacles_ > 0)
            {
                cachedStr += " with ";
                cachedStr += std::to_string(num_tentacles_);
                cachedStr += " tentacles";
            }

            cachedStr += '.';
        }

        return cachedStr;
    }

    const BodyType BodyType::Make_Humanoid(
        const bool HAS_FANGS, const bool HAS_CLAWS, const bool HAS_TAIL, const bool HAS_HORNS)
    {
        return BodyType(1, 2, 2, 2, HAS_FANGS, HAS_CLAWS, false, true, HAS_TAIL, false, HAS_HORNS);
    }

    const BodyType BodyType::Make_Wolfen(const role::Enum ROLE_ENUM)
    {
        return BodyType(
            ((ROLE_ENUM == role::TwoHeaded) ? 2 : 1), 0, 4, 2, true, true, false, false, true);
    }

    const BodyType BodyType::Make_Dragon(const bool HAS_BREATH)
    {
        return BodyType(1, 0, 4, 2, true, true, true, false, true, true, true, 0, HAS_BREATH);
    }

    const BodyType BodyType::Make_Pixie() { return BodyType(1, 2, 2, 2, false, false, true, true); }

    const BodyType BodyType::Make_FromRaceAndRole(
        const creature::race::Enum RACE, const creature::role::Enum ROLE)
    {
        switch (RACE)
        {
            case creature::race::Human:
            case creature::race::Troll:
            case creature::race::Ogre:
            case creature::race::Halfling:
            case creature::race::Pug:
            case creature::race::Witch:
            case creature::race::Golem:
            case creature::race::Gnome: { return Make_Humanoid();
            }
            case creature::race::Pixie: { return Make_Pixie();
            }
            case creature::race::Lion:
            case creature::race::Wolfen: { return Make_Wolfen(ROLE);
            }
            case creature::race::Dragon: { return Make_Dragon(true);
            }
            case creature::race::Goblin:
            case creature::race::Orc: { return Make_Humanoid(true, true);
            }
            case creature::race::Newt: { return Make_Humanoid(false, true);
            }
            case creature::race::Spider: { return BodyType(1, 0, 8, 6, true);
            }
            case creature::race::Bog:
            {
                if (ROLE == role::Spike)
                {
                    return BodyType(1, 2, 4, 2, false, true, false, false, true, true);
                }

                if (ROLE == role::Tendrilus)
                {
                    return BodyType(1, 0, 4, 2, false, true, false, true, true, false, false, 4);
                }

                if (ROLE == role::Whelp)
                {
                    return BodyType(1, 0, 4, 2, true, true, false, false, true, true);
                }

                if (ROLE == role::Wing)
                {
                    return BodyType(1, 2, 2, 2, true, true, true, false, true, true, true);
                }

                // all others
                return BodyType::Make_Humanoid();
            }
            case creature::race::CaveCrawler: { return BodyType(1, 0, 6, 4, true, true);
            }
            case creature::race::Hydra:
            {
                return BodyType(6, 0, 4, 12, true, true, true, false, true, false, false, 0, true);
            }
            case creature::race::LizardWalker: { return Make_Humanoid(true, true, true);
            }
            case creature::race::Minotaur: { return Make_Humanoid(false, false, true, true);
            }
            case creature::race::Plant:
            {
                if (ROLE == role::Pod)
                {
                    return BodyType(1, 0, 0, 1, true, true, false, false, false, true, false, 4);
                }

                if (ROLE == role::Smasher)
                {
                    return BodyType(1, 2, 2, 2, false, true, false, false, false, true, true);
                }

                if (ROLE == role::Strangler)
                {
                    return BodyType(1, 0, 2, 2, false, false, false, false, false, false, false, 2);
                }

                if (ROLE == role::Tendrilus)
                {
                    return BodyType(1, 2, 2, 0, true, false, false, false, false, true, true, 8);
                }

                break;
            }
            case creature::race::Shade: { return BodyType(1, 2, 0, 2, false, true, false, true);
            }
            case creature::race::Skeleton:
            {
                if (ROLE == role::Chieftain)
                {
                    return BodyType(1, 2, 2, 2, false, true, false, true, false, false, true);
                }

                if (ROLE == role::FourArmed)
                {
                    return BodyType(1, 4, 2, 2, false, true, false, true);
                }

                if (ROLE == role::Mountain)
                {
                    return BodyType(1, 2, 2, 2, false, true, false, true, false, true);
                }

                // um...default skeleton
                return BodyType(1, 2, 2, 2, false, true, false, true);
            }
            case creature::race::Beetle: { return BodyType(1, 0, 4, 4, true, true);
            }
            case creature::race::Boar:
            {
                return BodyType(1, 0, 4, 2, true, false, false, false, true);
            }
            case creature::race::Demon:
            {
                if (ROLE == role::Skeleton)
                {
                    return BodyType(1, 2, 2, 2, false, true, true, true, true);
                }

                if (ROLE == role::Spike)
                {
                    return BodyType(1, 2, 2, 2, false, true, false, true, true, true, true);
                }

                if (ROLE == role::Strangler)
                {
                    return BodyType(1, 2, 2, 2, true, true, false, true, true);
                }

                if (ROLE == role::Whelp)
                {
                    return BodyType(1, 0, 8, 4, false, true, true);
                }

                if (ROLE == role::Wing)
                {
                    return BodyType(1, 2, 2, 2, true, true, true, false, true, true, true);
                }

                if (ROLE == role::Grunt)
                {
                    return BodyType(1, 2, 2, 2, true, true, true, true, true, true, true);
                }

                break;
            }
            case creature::race::Harpy: { return BodyType(1, 2, 2, 2, false, true, true);
            }
            case creature::race::Griffin:
            {
                if (ROLE == role::Whelp)
                {
                    return BodyType(1, 0, 2, 2, true, true, true, false, true, false, true);
                }

                if (ROLE == role::Wing)
                {
                    return BodyType(1, 0, 4, 2, true, true, true, false, true, false, true);
                }

                break;
            }
            case creature::race::LionBoar:
            {
                return BodyType(1, 0, 4, 2, true, false, false, false, true, false, true);
            }
            case creature::race::Naga: { return BodyType(1, 2, 0, 2, true, true, false, true, true);
            }
            case creature::race::Ramonaut:
            {
                return BodyType(1, 0, 4, 2, false, false, false, false, true, true, true);
            }
            case creature::race::Serpent:
            case creature::race::Cobra:
            {
                return BodyType(1, 0, 0, 2, true, false, false, false, true);
            }
            case creature::race::Werebear:
            {
                return BodyType(1, 2, 2, 2, true, true, false, true, false, true);
            }
            case creature::race::Wereboar:
            {
                return BodyType(1, 0, 4, 2, true, true, false, false, false, true, true);
            }
            case creature::race::Werecat:
            {
                return BodyType(1, 0, 4, 2, true, true, false, false, true);
            }
            case creature::race::ThreeHeadedHound:
            case creature::race::Werewolf:
            {
                return BodyType(1, 2, 2, 2, true, true, false, true, true);
            }
            case creature::race::Wyvern:
            {
                BodyType wyvernBodyType(Make_Dragon(false));
                wyvernBodyType.num_arms_ = 0;
                return wyvernBodyType;
            }
            case creature::race::Bat:
            case creature::race::Werebat: { return BodyType(1, 2, 2, 2, true, true, true);
            }
            case creature::race::Ghoul:
            {
                BodyType ghoulBodyType { Make_Humanoid() };

                if (ROLE == role::Spike)
                {
                    ghoulBodyType.has_claws_ = true;
                    ghoulBodyType.has_spikes_ = true;
                }
                else if (ROLE == role::Strangler)
                {
                    ghoulBodyType.has_claws_ = true;
                }

                return ghoulBodyType;
            }
            case creature::race::Giant:
            {
                BodyType giantBodyType { Make_Humanoid() };

                if (ROLE == role::Warlord)
                {
                    giantBodyType.has_claws_ = true;
                    giantBodyType.has_spikes_ = true;
                }
                else if ((ROLE == role::Mountain) || (ROLE == role::Strangler))
                {
                    giantBodyType.has_claws_ = true;
                }

                return giantBodyType;
            }
            case creature::race::Count:
            default: { break;
            }
        }

        M_HP_LOG_ERR(
            "creature::BodyType::Make_FromRace(race="
            << RACE << ", role=" << ROLE << ") -these values failed to create a BodyType.");

        return BodyType();
    }

    bool operator<(const BodyType & L, const BodyType & R)
    {
        return std::tie(
                   L.has_wings_,
                   L.has_fangs_,
                   L.has_claws_,
                   L.has_fingers_,
                   L.has_tail_,
                   L.has_spikes_,
                   L.has_horns_,
                   L.has_breath_,
                   L.num_arms_,
                   L.num_legs_,
                   L.num_heads_,
                   L.num_eyes_,
                   L.num_tentacles_)
            < std::tie(
                   R.has_wings_,
                   R.has_fangs_,
                   R.has_claws_,
                   R.has_fingers_,
                   R.has_tail_,
                   R.has_spikes_,
                   R.has_horns_,
                   R.has_breath_,
                   R.num_arms_,
                   R.num_legs_,
                   R.num_heads_,
                   R.num_eyes_,
                   R.num_tentacles_);
    }

    bool operator==(const BodyType & L, const BodyType & R)
    {
        return std::tie(
                   L.has_wings_,
                   L.has_fangs_,
                   L.has_claws_,
                   L.has_fingers_,
                   L.has_tail_,
                   L.has_spikes_,
                   L.has_horns_,
                   L.has_breath_,
                   L.num_arms_,
                   L.num_legs_,
                   L.num_heads_,
                   L.num_eyes_,
                   L.num_tentacles_)
            == std::tie(
                   R.has_wings_,
                   R.has_fangs_,
                   R.has_claws_,
                   R.has_fingers_,
                   R.has_tail_,
                   R.has_spikes_,
                   R.has_horns_,
                   R.has_breath_,
                   R.num_arms_,
                   R.num_legs_,
                   R.num_heads_,
                   R.num_eyes_,
                   R.num_tentacles_);
    }

} // namespace creature
} // namespace heroespath
