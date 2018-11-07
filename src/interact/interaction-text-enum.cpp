// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-text-enum.cpp
//
#include "interaction-text-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace interact
{

    const std::string Text::ToString(const Text::Enum TEXT_TYPE)
    {
        switch (TEXT_TYPE)
        {
            case Text::System: return "System";
            case Text::Dialog: return "Dialog";
            case Text::Count: return "(Count)";
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(TEXT_TYPE));
                return "";
            }
        }
    }

    gui::GuiFont::Enum Text::Font(const Text::Enum TEXT_TYPE)
    {
        switch (TEXT_TYPE)
        {
            case Text::System: return gui::GuiFont::Default;
            case Text::Dialog: return gui::GuiFont::DialogMedieval;
            case Text::Count:
            default:
            {
                M_HP_LOG_ERR(ValueOutOfRangeErrorString(TEXT_TYPE));
                return gui::GuiFont::Count;
            }
        }
    }

} // namespace interact
} // namespace heroespath
