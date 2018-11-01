// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_LISTBOX_HELPERS_HPP_INCLUDED
#define HEROESPATH_GUI_LISTBOX_HELPERS_HPP_INCLUDED
//
// list-box-helpers.hpp
//
#include "gui/list-box.hpp"
#include "item/item.hpp"

#include <algorithm>

namespace heroespath
{
namespace gui
{
    namespace listbox
    {

        template <typename Stage_t, typename Element_t>
        void SortByName(ListBox<Stage_t, Element_t> & listBox, bool & isSortReversed)
        {
            if (isSortReversed)
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return B->Element()->Name() < A->Element()->Name();
                });
            }
            else
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return A->Element()->Name() < B->Element()->Name();
                });
            }

            isSortReversed = !isSortReversed;
        }

        template <typename Stage_t>
        void SortByPrice(ListBox<Stage_t, item::ItemPtr_t> & listBox, bool & isSortReversed)
        {
            if (isSortReversed)
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return B->Element()->Price() < A->Element()->Price();
                });
            }
            else
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return A->Element()->Price() < B->Element()->Price();
                });
            }

            isSortReversed = !isSortReversed;
        }

        template <typename Stage_t>
        void SortByWeight(ListBox<Stage_t, item::ItemPtr_t> & listBox, bool & isSortReversed)
        {
            if (isSortReversed)
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return B->Element()->Weight() < A->Element()->Weight();
                });
            }
            else
            {
                listBox.Sort([](const auto & A, const auto & B) {
                    return A->Element()->Weight() < B->Element()->Weight();
                });
            }

            isSortReversed = !isSortReversed;
        }

    } // namespace listbox
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_LISTBOX_HELPERS_HPP_INCLUDED
