// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_LISTBOX_HELPERS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LISTBOX_HELPERS_HPP_INCLUDED
//
// list-box-helpers.hpp
//

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class ListBox;

        namespace listbox
        {

            void SortByItemName(ListBox & listBox, bool & isSortReversed);
            void SortByItemPrice(ListBox & listBox, bool & isSortReversed);
            void SortByItemWeight(ListBox & listBox, bool & isSortReversed);

        } // namespace listbox
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_HELPERS_HPP_INCLUDED
