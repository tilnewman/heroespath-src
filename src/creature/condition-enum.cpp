// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string Conditions::ToString(const Conditions::Enum ENUM)
    {
        switch (ENUM)
        {
            case Good:
            {
                return "Good";
            }
            case Bold:
            {
                return "Bold";
            }
            case Heroic:
            {
                return "Heroic";
            }
            case Daunted:
            {
                return "Daunted";
            }
            case Panic:
            {
                return "Panic";
            }
            case Dazed:
            {
                return "Dazed";
            }
            case Tripped:
            {
                return "Tripped";
            }
            case AsleepNatural:
            {
                return "AsleepNatural";
            }
            case Poisoned:
            {
                return "Poisoned";
            }
            case Pounced:
            {
                return "Pounced";
            }
            case AsleepMagical:
            {
                return "AsleepMagical";
            }
            case Unconscious:
            {
                return "Unconscious";
            }
            case Stone:
            {
                return "Stone";
            }
            case Dead:
            {
                return "Dead";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string Conditions::Name(const Conditions::Enum ENUM)
    {
        if (ENUM == AsleepNatural)
        {
            return "Asleep";
        }
        else if (ENUM == AsleepMagical)
        {
            return "Magical Sleep";
        }
        else
        {
            return ToString(ENUM);
        }
    }

    const std::string Conditions::Desc(const Conditions::Enum ENUM)
    {
        std::ostringstream keySS;
        keySS << "heroespath-creature-condition-" << ToString(ENUM) << "-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Conditions::ImageFilename(const Conditions::Enum ENUM)
    {
        return misc::ToLowerCopy(ToString(ENUM) + ".png");
    }

    const std::string Conditions::ImageDirectory()
    {
        static std::string imageDir;

        if (imageDir.empty())
        {
            imageDir = misc::ConfigFile::Instance()->GetMediaPath("media-images-conditions-dir");
        }

        return imageDir;
    }

    const std::string Conditions::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePaths(ImageDirectory(), ImageFilename(ENUM));
    }

    namespace condition
    {
        // see comments in header
        const std::size_t Severity::ALL { 0 };
        const std::size_t Severity::GOOD { 1 };
        const std::size_t Severity::LEAST_BENEFITIAL { 2 };
        const std::size_t Severity::MOST_BENEFITIAL { 1000 };
        const std::size_t Severity::LEAST_HARMFUL { 1001 };
        const std::size_t Severity::MOST_HARMFUL { 2000 };

        // see comments in header
        std::size_t Severity::Get(const Conditions::Enum ENUM)
        {
            switch (ENUM)
            {
                case Conditions::Good:
                {
                    return GOOD;
                }
                case Conditions::Bold:
                {
                    return 100;
                }
                case Conditions::Heroic:
                {
                    return 400;
                }
                case Conditions::Daunted:
                {
                    return 1001;
                }
                case Conditions::Panic:
                {
                    return 1050;
                }
                case Conditions::Dazed:
                {
                    return 1100;
                }
                case Conditions::Tripped:
                {
                    return 1200;
                }
                case Conditions::AsleepNatural:
                {
                    return 1300;
                }
                case Conditions::Poisoned:
                {
                    return 1400;
                }
                case Conditions::Pounced:
                {
                    return 1450;
                }
                case Conditions::AsleepMagical:
                {
                    return 1500;
                }
                case Conditions::Unconscious:
                {
                    return 1800;
                }
                case Conditions::Stone:
                {
                    return 1900;
                }
                case Conditions::Dead:
                {
                    return 2000;
                }
                case Conditions::Count:
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(ENUM)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Conditions::Count) << ")");

                    return 0;
                }
            }
        }

    } // namespace condition
} // namespace creature
} // namespace heroespath
