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

#include "misc/random.hpp"

namespace heroespath
{
namespace gui
{

    const std::string CombatImageType::RandomConfigFileKey(const Enum IMAGE)
    {
        std::string str(ImageConfigKeyPrefix(IMAGE));

        if (!((IMAGE == Enum::Wing) || (IMAGE == Enum::Run) || (IMAGE == Enum::CrossSwords)
              || (IMAGE == Enum::CrossBones)))
        {
            str += std::to_string(misc::Random<std::size_t>(1, ImageCount(IMAGE)));
        }

        return str;
    }

} // namespace gui
} // namespace heroespath
