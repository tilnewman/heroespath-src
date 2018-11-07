// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// layer-type-enum.cpp
//
#include "layer-type-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace map
{

    const std::string LayerType::ToString(const LayerType::Enum LAYER)
    {
        switch (LAYER)
        {
            case Ground:
            {
                return "Ground";
            }
            case Object:
            {
                return "Object";
            }
            case Shadow:
            {
                return "Shadow";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(LAYER));
                return "";
            }
        }
    }

} // namespace map
} // namespace heroespath
