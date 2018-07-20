// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_LIST_NO_ELEMENT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LIST_NO_ELEMENT_HPP_INCLUDED
//
// list-no-element.hpp
//

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // responsible for being a type that indicates a ListElement contains no element, only
        // either text or an image (or both)
        struct NoElement_t
        {};

        inline bool operator==(const NoElement_t &, const NoElement_t &) { return true; }
        inline bool operator!=(const NoElement_t &, const NoElement_t &) { return false; }
        inline bool operator<(const NoElement_t &, const NoElement_t &) { return false; }
        inline bool operator>(const NoElement_t & L, const NoElement_t & R) { return (R < L); }
        inline bool operator<=(const NoElement_t & L, const NoElement_t & R) { return !(L > R); }
        inline bool operator>=(const NoElement_t & L, const NoElement_t & R) { return !(L < R); }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LIST_NO_ELEMENT_HPP_INCLUDED
