// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
#define HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
//
// combat-image-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    struct CombatImageType : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            Wing = 0,
            Arrow,
            Bolt,
            Dart,
            Stone,
            Note,
            Spark,
            Run,
            CrossSwords,
            CrossBones,
            Count
        };

        static constexpr std::size_t ImageCount(const Enum IMAGE) noexcept
        {
            switch (IMAGE)
            {
                case Wing: return 1;
                case Arrow: return 5;
                case Bolt: return 2;
                case Dart: return 2;
                case Stone: return 4;
                case Note: return 5;
                case Spark: return 4;
                case Run: return 1;
                case CrossSwords: return 1;
                case CrossBones: return 1;
                case Count:
                default: return 0;
            }
        }

        static const std::string RandomConfigFileKey(const Enum);

    private:
        static constexpr std::string_view ImageConfigKeyPrefix(const Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Wing: return "media-image-combat-wing";
                case Arrow: return "media-image-combat-arrow-";
                case Bolt: return "media-image-combat-bolt-";
                case Dart: return "media-image-combat-dart-";
                case Stone: return "media-image-combat-stone-";
                case Note: return "media-image-combat-note-";
                case Spark: return "media-image-combat-spark-";
                case Run: return "media-image-combat-run";
                case CrossSwords: return "media-image-combat-crossswords";
                case CrossBones: return "media-image-combat-crossbones";
                case Count:
                default: return "gui::CombatImageType::ImageConfigKeyPrefix(ENUM=out_of_range)";
            }
        }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COMBATIMAGEENUM_HPP_INCLUDED
