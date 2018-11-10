// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace spell
{

    const std::string Spells::ToString(const Spells::Enum ENUM)
    {
        switch (ENUM)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "ClearMind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "PoisonCloud";
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

    const std::string Spells::Name(const Spells::Enum ENUM)
    {
        switch (ENUM)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "Clear Mind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "Poison Cloud";
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

    const std::string Spells::ShortDesc(const Spells::Enum ENUM)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(ENUM) << "-short-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Spells::ExtraDesc(const Spells::Enum ENUM)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(ENUM) << "-extra-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Spells::ImageFilename(const Spells::Enum ENUM)
    {
        return misc::ToLowerCopy(ToString(ENUM) + ".png");
    }

    const std::string Spells::ImageDirectory()
    {
        return misc::ConfigFile::Instance()->GetMediaPath("media-images-spells-dir");
    }

    const std::string Spells::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePathsThenClean(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace spell
} // namespace heroespath
