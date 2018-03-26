///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_SFMLUTIL_GUI_LISTBOXITEM_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LISTBOXITEM_HPP_INCLUDED
//
// list-box-item.hpp
//  A hack'ish extension of the TextRegion class that holds
//  everything that the ListBox class might ever need to list...
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/text-region.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtrC_t = Spell * const;
} // namespace spell
namespace song
{
    class Song;
    using SongPtrC_t = Song * const;
} // namespace song
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;

    class Title;
    using TitlePtr_t = misc::NotNull<Title *>;
    using TitlePtrOpt_t = boost::optional<TitlePtr_t>;

    class Condition;
    using ConditionPtr_t = Condition *;
    using ConditionPtrC_t = Condition * const;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
} // namespace item
namespace state
{
    class GameState;
    using GameStatePtr_t = GameState *;
} // namespace state

namespace sfml_util
{
    namespace gui
    {

        class TextInfo;

        // TextRegion wrapper for anything the ListBox class may need to list...
        class ListBoxItem : public sfml_util::gui::TextRegion
        {
        public:
            ListBoxItem(const ListBoxItem &) = delete;
            ListBoxItem(ListBoxItem &&) = delete;
            ListBoxItem & operator=(const ListBoxItem &) = delete;
            ListBoxItem & operator=(ListBoxItem &&) = delete;

        public:
            explicit ListBoxItem(const std::string & NAME, const bool IS_VALID = true);

            // used by the Combat Stage for a ListBox of text lines
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const bool IS_VALID = true);

            // used by the Party Stage for a ListBox of Characters
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool IS_VALID = true);

            // used by the LoadGame Stage for a ListBox of GameStates
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const state::GameStatePtr_t GAMESTATE_PTR,
                const bool IS_VALID = true);

            // used by the inventory stage to list items
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const item::ItemPtr_t ITEM_PTR,
                const bool IS_VALID = true);

            // used by the inventory stage to list conditions
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::ConditionPtr_t CONDITION_PTR_PARAM,
                const bool IS_VALID = true);

            // used by the inventory stage to list titles
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::TitlePtr_t & TITLE_PTR_PARAM,
                const bool IS_VALID = true);

            // used by the inventory stage to list spells
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const spell::SpellPtrC_t SPELL_CPTRC_PARAM,
                const bool IS_VALID = true);

            // used by the inventory stage to list spells
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const song::SongPtrC_t SONG_CPTRC_PARAM,
                const bool IS_VALID = true);

            const creature::CreaturePtr_t CHARACTER_CPTR;
            const state::GameStatePtr_t GAMESTATE_CPTR;
            const item::ItemPtr_t ITEM_CPTR;
            const creature::ConditionPtrC_t COND_CPTRC;
            const creature::TitlePtrOpt_t TITLE_PTR_OPT;
            const spell::SpellPtrC_t SPELL_CPTRC;
            const song::SongPtrC_t SONG_CPTRC;
            bool is_valid;

            friend bool operator==(const ListBoxItem & L, const ListBoxItem & R);
            friend bool operator<(const ListBoxItem & L, const ListBoxItem & R);
        };

        using ListBoxItemSPtr_t = std::shared_ptr<ListBoxItem>;
        using ListBoxItemSVec_t = std::vector<ListBoxItemSPtr_t>;

        bool operator==(const ListBoxItem & L, const ListBoxItem & R);

        inline bool operator!=(const ListBoxItem & L, const ListBoxItem & R) { return !(L == R); }

        bool operator<(const ListBoxItem & L, const ListBoxItem & R);

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOXITEM_HPP_INCLUDED
