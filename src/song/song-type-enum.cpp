// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// song-enum.cpp
//
#include "song-type-enum.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace song
{

    const std::string SongType::ToString(const Enum E)
    {
        switch (E)
        {
            case Guitar:
            {
                return "Guitar";
            }
            case Drum:
            {
                return "Drum";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "song::SongType::ToString(" << E << ")_InvalidValueError.";
                throw std::runtime_error(ss.str());
            }
        }
    }
} // namespace song
} // namespace heroespath
