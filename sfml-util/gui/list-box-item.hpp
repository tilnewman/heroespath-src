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
    using ItemSPtr_t = std::shared_ptr<Item>;
}
namespace state
{
    class GameState;
    using GameStateSPtr_t = std::shared_ptr<GameState>;
}
namespace player
{
    class Character;
    using CharacterSPtr_t = std::shared_ptr<Character>;
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
        ListBoxItem(const std::string & NAME);

        //used by the Combat Stage for a ListBox of text lines
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO);

        //used by the Party Stage for a ListBox of Characters
        ListBoxItem(const std::string &                   NAME,
                    const sfml_util::gui::TextInfo &      TEXT_INFO,
                    const game::player::CharacterSPtr_t & CHARACTER_SPTR);

        //used by the LoadGame Stage for a ListBox of GameStates
        ListBoxItem(const std::string &                  NAME,
                    const sfml_util::gui::TextInfo &     TEXT_INFO,
                    const game::state::GameStateSPtr_t & GAMESTATE_SPTR);

        //used by the inventory stage to list items
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO,
                    const game::item::ItemSPtr_t &   IITEM_SPTR);

        //used by the inventory stage to list conditions
        ListBoxItem(const std::string &                  NAME,
                    const sfml_util::gui::TextInfo &     TEXT_INFO,
                    const game::creature::ConditionPtr_t CONDITION_PTR_PARAM);

        //used by the inventory stage to list titles
        ListBoxItem(const std::string &               NAME,
                    const sfml_util::gui::TextInfo &  TEXT_INFO,
                    const game::creature::TitlePtrC_t TITLE_CPTRC_PARAM);

        //used by the inventory stage to list spells
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO,
                    const game::spell::SpellPtrC_t   SPELL_CPTRC_PARAM);

        game::player::CharacterSPtr_t         character_sptr;
        game::state::GameStateSPtr_t          gamestate_sptr;
        game::item::ItemSPtr_t                iitem_sptr;
        const game::creature::ConditionPtrC_t COND_CPTRC;
        const game::creature::TitlePtrC_t     TITLE_CPTRC;
        const game::spell::SpellPtrC_t        SPELL_CPTRC;

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
