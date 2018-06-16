// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_LISTBOX_ITEM_FACTORY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LISTBOX_ITEM_FACTORY_HPP_INCLUDED
//
// list-box-item-factory.hpp
//
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
} // namespace song
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    class Title;
    using TitlePtr_t = misc::NotNull<Title *>;

    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace game
{
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
} // namespace game

namespace sfml_util
{
    namespace gui
    {

        class TextInfo;

        class ListBoxItem;
        using ListBoxItemPtr_t = misc::NotNull<ListBoxItem *>;
        using ListBoxItemUPtr_t = std::unique_ptr<ListBoxItem>;
        using ListBoxItemUVec_t = std::vector<ListBoxItemUPtr_t>;

        // Responsible for making ListBoxItems
        struct ListBoxItemFactory
        {
            ListBoxItemFactory(const ListBoxItemFactory &) = delete;
            ListBoxItemFactory(ListBoxItemFactory &&) = delete;
            ListBoxItemFactory & operator=(const ListBoxItemFactory &) = delete;
            ListBoxItemFactory & operator=(ListBoxItemFactory &&) = delete;

            ListBoxItemFactory() = default;

            ListBoxItemUPtr_t MakeCopy(const std::string & LISTBOX_NAME, const ListBoxItemPtr_t);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::CreaturePtr_t CHARACTER_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const game::GameStatePtr_t GAMESTATE_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const item::ItemPtr_t ITEM_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::ConditionPtr_t CONDITION_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::TitlePtr_t & TITLE_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const spell::SpellPtr_t SPELL_PTR,
                const bool IS_VALID = true);

            ListBoxItemUPtr_t Make(
                const std::string & LISTBOX_NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const song::SongPtr_t SONG_PTR,
                const bool IS_VALID = true);

        private:
            static const std::string NAME_PREFIX_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_ITEM_FACTORY_HPP_INCLUDED
