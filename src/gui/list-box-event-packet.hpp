// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_LISTBOX_EVENT_PACKET_HPP_INCLUDED
#define HEROESPATH_GUI_LISTBOX_EVENT_PACKET_HPP_INCLUDED
//
// list-box-event-packet.hpp
//
#include "gui/gui-event-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    template <typename Element_t>
    class ListElement;

    template <typename Element_t>
    using ListElementPtr_t = misc::NotNull<ListElement<Element_t> *>;

    template <typename Stage_t, typename Element_t>
    class ListBox;

    // Callbacks will not occur if the listbox is empty. Since this object is only created
    // when not empty, then selection must be valid. If gui_event is Click or DoubleClick
    // then mouse_pos_opt is initialized
    template <typename Stage_t, typename Element_t>
    struct ListBoxEventPacket
    {
        ListBoxEventPacket(
            misc::NotNull<ListBox<Stage_t, Element_t> *> LISTBOX_PTR,
            const GuiEvent::Enum GUI_EVENT,
            const sf::Event::KeyEvent & KEYPRESS_EVENT = sf::Event::KeyEvent())
            : listbox_ptr(LISTBOX_PTR)
            , gui_event(GUI_EVENT)
            , keypress_event(KEYPRESS_EVENT)
            , selected_element_ptr(LISTBOX_PTR->Selection())
        {}

        ListBoxEventPacket(const ListBoxEventPacket &) = default;
        ListBoxEventPacket(ListBoxEventPacket &&) = default;
        ListBoxEventPacket & operator=(const ListBoxEventPacket &) = default;
        ListBoxEventPacket & operator=(ListBoxEventPacket &&) = default;

        misc::NotNull<ListBox<Stage_t, Element_t> *> listbox_ptr;
        GuiEvent::Enum gui_event;
        sf::Event::KeyEvent keypress_event;
        ListElementPtr_t<Element_t> selected_element_ptr;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_LISTBOX_EVENT_PACKET_HPP_INCLUDED
