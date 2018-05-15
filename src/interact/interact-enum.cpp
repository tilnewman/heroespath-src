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
#include "interact-enum.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace interact
{

    const std::string Interact::ToString(const Interact::Enum INTERACTION_TYPE)
    {
        switch (INTERACTION_TYPE)
        {
            case Interact::Lock:
            {
                return "Lock";
            }
            case Interact::Conversation:
            {
                return "Conversation";
            }
            case Interact::Count:
            default:
            {
                std::ostringstream ss;
                ss << "map::Interact::Enum::ToString(" << INTERACTION_TYPE
                   << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    const std::string Interact::ImageKey(const Interact::Enum INTERACTION_TYPE)
    {
        switch (INTERACTION_TYPE)
        {
            case Interact::Lock:
            {
                return "media-images-misc-lock";
            }
            case Interact::Conversation:
            {
                return "media-images-misc-talk";
            }
            case Interact::Count:
            default:
            {
                std::ostringstream ss;
                ss << "map::Interact::Enum::ImageKey(" << INTERACTION_TYPE
                   << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }
} // namespace interact
} // namespace heroespath
