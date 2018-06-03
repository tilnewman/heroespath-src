// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPtrOpt_t = boost::optional<SpellPtr_t>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongPtrOpt_t = boost::optional<SongPtr_t>;
} // namespace song
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;

    class Title;
    using TitlePtr_t = misc::NotNull<Title *>;
    using TitlePtrOpt_t = boost::optional<TitlePtr_t>;

    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;
    using ConditionPtrOpt_t = boost::optional<ConditionPtr_t>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
} // namespace item
namespace game
{
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
    using GameStatePtrOpt_t = boost::optional<GameStatePtr_t>;
} // namespace game

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
                const game::GameStatePtr_t GAMESTATE_PTR,
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
                const creature::ConditionPtr_t CONDITION_PTR,
                const bool IS_VALID = true);

            // used by the inventory stage to list titles
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const creature::TitlePtr_t & TITLE_PTR,
                const bool IS_VALID = true);

            // used by the inventory stage to list spells
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const spell::SpellPtr_t SPELL_PTR,
                const bool IS_VALID = true);

            // used by the inventory stage to list spells
            ListBoxItem(
                const std::string & NAME,
                const sfml_util::gui::TextInfo & TEXT_INFO,
                const song::SongPtr_t SONG_PTR,
                const bool IS_VALID = true);

            virtual ~ListBoxItem();

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            void MoveEntityPos(const float HORIZ, const float VERT) override;

            const creature::CreaturePtrOpt_t CharacterPtrOpt() const { return creaturePtrOpt_; }
            const game::GameStatePtrOpt_t GameStatePtrOpt() const { return gameStatePtrOpt_; }
            const item::ItemPtrOpt_t ItemPtrOpt() const { return itemPtrOpt_; }
            const creature::ConditionPtrOpt_t ConditionPtrOpt() const { return conditionPtrOpt_; }
            const creature::TitlePtrOpt_t TitlePtrOpt() const { return titlePtrOpt_; }
            const spell::SpellPtrOpt_t SpellPtrOpt() const { return spellPtrOpt_; }
            const song::SongPtrOpt_t SongPtrOpt() const { return songPtrOpt_; }
            bool IsValid() const { return isValid_; }
            std::size_t TextureIndex() const { return textureIndex_; }
            sf::Sprite & Sprite() { return sprite_; }
            bool HasImage() const { return (textureIndex_ != 0); }

            friend bool operator==(const ListBoxItem & L, const ListBoxItem & R);
            friend bool operator<(const ListBoxItem & L, const ListBoxItem & R);

        private:
            creature::CreaturePtrOpt_t creaturePtrOpt_;
            game::GameStatePtrOpt_t gameStatePtrOpt_;
            item::ItemPtrOpt_t itemPtrOpt_;
            creature::ConditionPtrOpt_t conditionPtrOpt_;
            creature::TitlePtrOpt_t titlePtrOpt_;
            spell::SpellPtrOpt_t spellPtrOpt_;
            song::SongPtrOpt_t songPtrOpt_;
            bool isValid_;
            std::size_t textureIndex_;
            sf::Sprite sprite_;
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
