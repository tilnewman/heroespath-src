// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
#define HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
//
// level-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace map
{

    struct LevelType : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Town = 0,
            Hideaway,
            Inside,
            Realm,
            Count
        };
    };

    struct Level : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Thornberry = 0,
            Thornberry_GuardPostWest,
            Thornberry_GuardPostEast,
            ThornberryMeadows,
            ThornberryHighlands,
            // Mudgate,
            // Bridgeway,
            Count
        };

        static constexpr std::string_view const FILENAME_EXTENSION { ".tmx" };

        static const std::string Path(const Level::Enum LEVEL);

        static constexpr LevelType::Enum Type(const Level::Enum LEVEL) noexcept
        {
            switch (LEVEL)
            {
                case Thornberry: return LevelType::Town;
                case Thornberry_GuardPostWest: return LevelType::Inside;
                case Thornberry_GuardPostEast: return LevelType::Inside;
                case ThornberryMeadows: return LevelType::Realm;
                case ThornberryHighlands: return LevelType::Realm;
                case Count:
                default: return LevelType::Count;
            }
        }
    };

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_LEVELENUM_HPP_INCLUDED
