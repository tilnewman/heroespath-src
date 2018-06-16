// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-box-item-factory.cpp
//
#include "list-box-item-factory.hpp"

#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/title.hpp"
#include "game/game-state.hpp"
#include "item/item.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        const std::string ListBoxItemFactory::NAME_PREFIX_{ "ListBoxItem_In_" };

        ListBoxItemUPtr_t ListBoxItemFactory::MakeCopy(
            const std::string & LISTBOX_NAME, const ListBoxItemPtr_t LISTBOX_ITEM_PTR)
        {
            auto textInfo{ LISTBOX_ITEM_PTR->GetTextInfo() };
            textInfo.text = LISTBOX_ITEM_PTR->GetText();

            ListBoxItemUPtr_t newListBoxItemUPtr;
            if (LISTBOX_ITEM_PTR->CharacterPtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->CharacterPtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else if (LISTBOX_ITEM_PTR->ConditionPtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->ConditionPtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else if (LISTBOX_ITEM_PTR->GameStatePtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->GameStatePtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else if (LISTBOX_ITEM_PTR->ItemPtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->ItemPtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else if (LISTBOX_ITEM_PTR->SongPtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->SongPtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else if (LISTBOX_ITEM_PTR->TitlePtrOpt())
            {
                newListBoxItemUPtr = Make(
                    LISTBOX_NAME,
                    textInfo,
                    LISTBOX_ITEM_PTR->TitlePtrOpt().value(),
                    LISTBOX_ITEM_PTR->IsValid());
            }
            else
            {
                newListBoxItemUPtr = Make(LISTBOX_NAME, textInfo, LISTBOX_ITEM_PTR->IsValid());
            }

            return newListBoxItemUPtr;
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const bool IS_VALID)
        {
            std::string name{ NAME_PREFIX_ + LISTBOX_NAME + "_TextInfoOnlyThatSays_" };

            const std::size_t NAME_TEXT_MAX_LENGTH{ 12 };
            if (TEXT_INFO.text.size() > NAME_TEXT_MAX_LENGTH)
            {
                name += TEXT_INFO.text.substr(0, NAME_TEXT_MAX_LENGTH);
            }
            else
            {
                name += TEXT_INFO.text;
            }

            return std::make_unique<ListBoxItem>(name, TEXT_INFO, IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::CreaturePtr_t CHARACTER_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Creature_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                CHARACTER_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const game::GameStatePtr_t GAMESTATE_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_GameStateWithAvatar_"
                    + avatar::Avatar::ToString(GAMESTATE_PTR->Party().Avatar()),
                TEXT_INFO,
                GAMESTATE_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const item::ItemPtr_t ITEM_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Item_" + ITEM_PTR->Name(),
                TEXT_INFO,
                ITEM_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::ConditionPtr_t CONDITION_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Condition_" + CONDITION_PTR->Name(),
                TEXT_INFO,
                CONDITION_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::TitlePtr_t & TITLE_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Title_" + TITLE_PTR->Name(),
                TEXT_INFO,
                TITLE_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const spell::SpellPtr_t SPELL_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Spell_" + SPELL_PTR->Name(),
                TEXT_INFO,
                SPELL_PTR,
                IS_VALID);
        }

        ListBoxItemUPtr_t ListBoxItemFactory::Make(
            const std::string & LISTBOX_NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const song::SongPtr_t SONG_PTR,
            const bool IS_VALID)
        {
            return std::make_unique<ListBoxItem>(
                NAME_PREFIX_ + LISTBOX_NAME + "_Song_" + SONG_PTR->Name(),
                TEXT_INFO,
                SONG_PTR,
                IS_VALID);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
