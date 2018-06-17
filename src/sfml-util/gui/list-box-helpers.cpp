// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-box-helpers.cpp
//
#include "list-box-helpers.hpp"

#include "item/item.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/list-box.hpp"

#include <algorithm>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        namespace listbox
        {

            void SortByItemName(ListBox & listBox, bool & isSortReversed)
            {
                if ((listBox.Empty() == false) && (listBox.Front()->ItemPtrOpt()))
                {
                    std::sort(
                        std::begin(listBox),
                        std::end(listBox),
                        [&isSortReversed](auto const & A, auto const & B) {
                            if (isSortReversed)
                            {
                                return B->ItemPtrOpt().value()->Name()
                                    < A->ItemPtrOpt().value()->Name();
                            }
                            else
                            {
                                return A->ItemPtrOpt().value()->Name()
                                    < B->ItemPtrOpt().value()->Name();
                            }
                        });

                    isSortReversed = !isSortReversed;
                }
            }

            void SortByItemPrice(ListBox & listBox, bool & isSortReversed)
            {
                if ((listBox.Empty() == false) && (listBox.Front()->ItemPtrOpt()))
                {
                    std::sort(
                        std::begin(listBox),
                        std::end(listBox),
                        [&isSortReversed](auto const & A, auto const & B) {
                            if (isSortReversed)
                            {
                                return B->ItemPtrOpt().value()->Price()
                                    < A->ItemPtrOpt().value()->Price();
                            }
                            else
                            {
                                return A->ItemPtrOpt().value()->Price()
                                    < B->ItemPtrOpt().value()->Price();
                            }
                        });

                    isSortReversed = !isSortReversed;
                }
            }

            void SortByItemWeight(ListBox & listBox, bool & isSortReversed)
            {
                if ((listBox.Empty() == false) && (listBox.Front()->ItemPtrOpt()))
                {
                    std::sort(
                        std::begin(listBox),
                        std::end(listBox),
                        [&isSortReversed](auto const & A, auto const & B) {
                            if (isSortReversed)
                            {
                                return B->ItemPtrOpt().value()->Weight()
                                    < A->ItemPtrOpt().value()->Weight();
                            }
                            else
                            {
                                return A->ItemPtrOpt().value()->Weight()
                                    < B->ItemPtrOpt().value()->Weight();
                            }
                        });

                    isSortReversed = !isSortReversed;
                }
            }

        } // namespace listbox
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
