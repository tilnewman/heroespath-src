// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// list-box-item.cpp
//
#include "list-box-item.hpp"

#include "creature/condition.hpp"
#include "creature/title.hpp"
#include "item/item.hpp"
#include "player/character.hpp"
#include "song/song.hpp"
#include "spell/spell-base.hpp"
#include "state/game-state.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        ListBoxItem::ListBoxItem(const std::string & NAME, const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemBase"))
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemTextOnly"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const player::CharacterPtr_t CHARACTER_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemPlayerCharacter"),
                  TEXT_INFO,
                  sf::FloatRect())
            , CHARACTER_CPTR(CHARACTER_PTR)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const state::GameStatePtr_t GAMESTATE_PTR,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemGameState"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(GAMESTATE_PTR)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::ConditionPtr_t CONDITION_CPTRC_PARAM,
            const bool IS_VALID)
            : TextRegion(
                  std::string(NAME).append("_ListBoxItemCondition"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(CONDITION_CPTRC_PARAM)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const item::ItemPtr_t ITEM_PTR,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemItem"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(ITEM_PTR)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const creature::TitlePtr_t & TITLE_PTR_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemTitle"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(TITLE_PTR_PARAM)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const spell::SpellPtrC_t SPELL_CPTRC_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSpell"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(SPELL_CPTRC_PARAM)
            , SONG_CPTRC(nullptr)
            , is_valid(IS_VALID)
        {}

        ListBoxItem::ListBoxItem(
            const std::string & NAME,
            const sfml_util::gui::TextInfo & TEXT_INFO,
            const song::SongPtrC_t SONG_CPTRC_PARAM,
            const bool IS_VALID)
            : TextRegion(std::string(NAME).append("_ListBoxItemSong"), TEXT_INFO, sf::FloatRect())
            , CHARACTER_CPTR(nullptr)
            , GAMESTATE_CPTR(nullptr)
            , ITEM_CPTR(nullptr)
            , COND_CPTRC(nullptr)
            , TITLE_PTR_OPT(boost::none)
            , SPELL_CPTRC(nullptr)
            , SONG_CPTRC(SONG_CPTRC_PARAM)
            , is_valid(IS_VALID)
        {}

        bool operator==(const ListBoxItem & L, const ListBoxItem & R)
        {
            return std::tie(
                       L.CHARACTER_CPTR,
                       L.GAMESTATE_CPTR,
                       L.ITEM_CPTR,
                       L.COND_CPTRC,
                       L.TITLE_PTR_OPT,
                       L.SPELL_CPTRC,
                       L.SONG_CPTRC,
                       L.is_valid)
                == std::tie(
                       R.CHARACTER_CPTR,
                       R.GAMESTATE_CPTR,
                       R.ITEM_CPTR,
                       R.COND_CPTRC,
                       R.TITLE_PTR_OPT,
                       R.SPELL_CPTRC,
                       R.SONG_CPTRC,
                       R.is_valid);
        }

        bool operator<(const ListBoxItem & L, const ListBoxItem & R)
        {
            return std::tie(
                       L.CHARACTER_CPTR,
                       L.GAMESTATE_CPTR,
                       L.ITEM_CPTR,
                       L.COND_CPTRC,
                       L.TITLE_PTR_OPT,
                       L.SPELL_CPTRC,
                       L.SONG_CPTRC,
                       L.is_valid)
                < std::tie(
                       R.CHARACTER_CPTR,
                       R.GAMESTATE_CPTR,
                       R.ITEM_CPTR,
                       R.COND_CPTRC,
                       R.TITLE_PTR_OPT,
                       R.SPELL_CPTRC,
                       R.SONG_CPTRC,
                       R.is_valid);
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
