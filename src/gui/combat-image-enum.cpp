// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-image-enum.hpp
//
#include "combat-image-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"

namespace heroespath
{
namespace gui
{

    const std::string CombatImageType::ToString(const Enum IMAGE)
    {
        switch (IMAGE)
        {
            case Wing: { return "Wing";
            }
            case Arrow: { return "Arrow";
            }
            case Bolt: { return "Bolt";
            }
            case Dart: { return "Dart";
            }
            case Stone: { return "Stone";
            }
            case Note: { return "Note";
            }
            case Spark: { return "Spark";
            }
            case Run: { return "Run";
            }
            case CrossSwords: { return "CrossSwords";
            }
            case CrossBones: { return "CrossBones";
            }
            case Count: { return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    std::size_t CombatImageType::ImageCount(const Enum IMAGE)
    {
        switch (IMAGE)
        {
            case Wing: { return 1;
            }
            case Arrow: { return 5;
            }
            case Bolt: { return 2;
            }
            case Dart: { return 2;
            }
            case Stone: { return 4;
            }
            case Note: { return 5;
            }
            case Spark: { return 4;
            }
            case Run: { return 1;
            }
            case CrossSwords: { return 1;
            }
            case CrossBones: { return 1;
            }
            case Count:
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return 0;
            }
        }
    }

    const std::string CombatImageType::RandomConfigFileKey(const Enum IMAGE)
    {
        const auto CONFIG_FILE_KEY_PREFIX = [&]() {
            switch (IMAGE)
            {
                case Wing: { return "media-image-combat-wing";
                }
                case Arrow: { return "media-image-combat-arrow-";
                }
                case Bolt: { return "media-image-combat-bolt-";
                }
                case Dart: { return "media-image-combat-dart-";
                }
                case Stone: { return "media-image-combat-stone-";
                }
                case Note: { return "media-image-combat-note-";
                }
                case Spark: { return "media-image-combat-spark-";
                }
                case Run: { return "media-image-combat-run";
                }
                case CrossSwords: { return "media-image-combat-crossswords";
                }
                case CrossBones: { return "media-image-combat-crossbones";
                }
                case Count:
                default:
                {
                    M_HP_LOG_ERR(
                        "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE)
                                      << " is invalid. (count="
                                      << static_cast<EnumUnderlying_t>(Count) << ")");

                    return "";
                }
            }
        }();

        const auto IMAGE_COUNT { ImageCount(IMAGE) };

        if (IMAGE_COUNT == 1)
        {
            return CONFIG_FILE_KEY_PREFIX;
        }

        const auto IMAGE_NUMBER { misc::Random<std::size_t>(1, IMAGE_COUNT) };

        return CONFIG_FILE_KEY_PREFIX + misc::ToString(IMAGE_NUMBER);
    }

} // namespace gui
} // namespace heroespath
