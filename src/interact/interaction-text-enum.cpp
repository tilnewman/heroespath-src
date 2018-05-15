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
#include "sfml-util/font-manager.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace interact
{

    const std::string Text::ToString(const Text::Enum TEXT_TYPE)
    {
        switch (TEXT_TYPE)
        {
            case Text::System:
                return "System";
            case Text::Dialog:
                return "Dialog";
            case Text::Count:
            default:
            {
                std::ostringstream ss;
                ss << "interact::Text::Enum::ToString(" << TEXT_TYPE << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    const sfml_util::FontPtr_t Text::Font(const Text::Enum TEXT_TYPE)
    {
        switch (TEXT_TYPE)
        {
            case Text::System:
                return sfml_util::FontManager::Instance()->Font_Default1();
            case Text::Dialog:
                return sfml_util::FontManager::Instance()->Font_Dialog1();
            case Text::Count:
            default:
            {
                std::ostringstream ss;
                ss << "interact::Text::Enum::Font(" << TEXT_TYPE << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

} // namespace interact
} // namespace heroespath
