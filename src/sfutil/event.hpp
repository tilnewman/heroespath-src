// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
#define HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
//
// event.hpp
//
#include <SFML/Window/Event.hpp>

#include <iosfwd>

namespace heroespath
{
namespace sfutil
{
    namespace helpers
    {

        const char * eventToNameString(const sf::Event & EVENT);
        const char * eventToDescriptionString(const sf::Event & EVENT);
        void eventToDetailsStream(std::ostream & os, const sf::Event & EVENT);

    } // namespace helpers

} // namespace sfutil

} // namespace heroespath

namespace sf
{

std::ostream & operator<<(std::ostream & os, const sf::Event & EVENT);
std::ostream & operator<<(std::ostream & os, const sf::Event::KeyEvent & KEY_EVENT);

} // namespace sf

#endif // HEROESPATH_SFUTIL_EVENT_HPP_INCLUDED
