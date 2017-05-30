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
#ifndef SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
#define SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
//
// list-box-item.hpp
//  A hack'ish extension of the TextRegion class that holds
//  everything that the ListBox class might ever need to list...
//
#include "sfml-util/gui/text-region.hpp"

#include <list>
#include <memory>
#include <string>


namespace game
{
namespace spell
{
    class Spell;
    using SpellPtrC_t = Spell * const;
}
namespace creature
{
    class Title;
    using TitlePtrC_t = Title * const;

    class Condition;
    using ConditionPtr_t  = Condition *;
    using ConditionPtrC_t = Condition * const;
}
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}
namespace state
{
    class GameState;
    using GameStateSPtr_t = std::shared_ptr<GameState>;
}
namespace player
{
    class Character;
    using CharacterPtr_t = Character *;
}
}
namespace sfml_util
{
namespace gui
{

    class TextInfo;


    //TextRegion wrapper for anything the ListBox class may need to list...
    class ListBoxItem : public sfml_util::gui::TextRegion
    {
        ListBoxItem(const ListBoxItem &) =delete;
        ListBoxItem & operator=(const ListBoxItem &) =delete;

    public:
        explicit ListBoxItem(const std::string & NAME,
                             const bool          IS_VALID = true);

        //used by the Combat Stage for a ListBox of text lines
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO,
                    const bool                       IS_VALID = true);

        //used by the Party Stage for a ListBox of Characters
        ListBoxItem(const std::string &                NAME,
                    const sfml_util::gui::TextInfo &   TEXT_INFO,
                    const game::player::CharacterPtr_t CHARACTER_PTR,
                    const bool                         IS_VALID = true);

        //used by the LoadGame Stage for a ListBox of GameStates
        ListBoxItem(const std::string &                  NAME,
                    const sfml_util::gui::TextInfo &     TEXT_INFO,
                    const game::state::GameStateSPtr_t & GAMESTATE_SPTR,
                    const bool                           IS_VALID = true);

        //used by the inventory stage to list items
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO,
                    const game::item::ItemPtr_t      ITEM_PTR,
                    const bool                       IS_VALID = true);

        //used by the inventory stage to list conditions
        ListBoxItem(const std::string &                  NAME,
                    const sfml_util::gui::TextInfo &     TEXT_INFO,
                    const game::creature::ConditionPtr_t CONDITION_PTR_PARAM,
                    const bool                           IS_VALID = true);

        //used by the inventory stage to list titles
        ListBoxItem(const std::string &               NAME,
                    const sfml_util::gui::TextInfo &  TEXT_INFO,
                    const game::creature::TitlePtrC_t TITLE_CPTRC_PARAM,
                    const bool                        IS_VALID = true);

        //used by the inventory stage to list spells
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO,
                    const game::spell::SpellPtrC_t   SPELL_CPTRC_PARAM,
                    const bool                       IS_VALID = true);

        const game::player::CharacterPtr_t    CHARACTER_CPTR;
        game::state::GameStateSPtr_t          gamestate_sptr;
        const game::item::ItemPtr_t           ITEM_CPTR;
        const game::creature::ConditionPtrC_t COND_CPTRC;
        const game::creature::TitlePtrC_t     TITLE_CPTRC;
        const game::spell::SpellPtrC_t        SPELL_CPTRC;
        bool                                  is_valid;

        friend bool operator==(const ListBoxItem & L, const ListBoxItem & R);
        friend bool operator<(const ListBoxItem & L, const ListBoxItem & R);
    };


    using ListBoxItemSPtr_t = std::shared_ptr<ListBoxItem>;
    using ListBoxItemSLst_t = std::list<ListBoxItemSPtr_t>;


    bool operator==(const ListBoxItem & L, const ListBoxItem & R);

    inline bool operator!=(const ListBoxItem & L, const ListBoxItem & R)
    {
        return ! (L == R);
    }

    bool operator<(const ListBoxItem & L, const ListBoxItem & R);

}
}
#endif //SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
