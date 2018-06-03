// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_PORTRAITFACTORY_HPP_INCLUDED
#define HEROESPATH_AVATAR_PORTRAITFACTORY_HPP_INCLUDED
//
// portrait-factory.hpp
//
#include "avatar/avatar-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace avatar
{

    // Responsible for making default portrait images textures of Avatars/NPCs.
    class PortraitFactory
    {
    public:
        static void Make(const Avatar::Enum, sf::Texture &);
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_PORTRAITFACTORY_HPP_INCLUDED
