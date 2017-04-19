#ifndef APPBASE_SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
#define APPBASE_SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
//
// list-box-item.hpp
//  A hack'ish extension of the TextRegion class that holds
//  everything that the ListBox class might ever need to list...
//
#include "sfml-util/gui/text-region.hpp"

#include <list>
#include <memory>


namespace heroespath
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
    using ConditionSPtr_t = std::shared_ptr<Condition>;
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
    public:
        ListBoxItem(const std::string & NAME);

        //used by the Combat Stage for a ListBox of text lines
        ListBoxItem(const std::string &              NAME,
                    const sfml_util::gui::TextInfo & TEXT_INFO);

        //used by the Party Stage for a ListBox of Characters
        ListBoxItem(const std::string &                         NAME,
                    const sfml_util::gui::TextInfo &            TEXT_INFO,
                    const heroespath::player::CharacterSPtr_t & CHARACTER_SPTR);

        //used by the LoadGame Stage for a ListBox of GameStates
        ListBoxItem(const std::string &                        NAME,
                    const sfml_util::gui::TextInfo &           TEXT_INFO,
                    const heroespath::state::GameStateSPtr_t & GAMESTATE_SPTR);

        //used by the inventory stage to list items
        ListBoxItem(const std::string &                   NAME,
                    const sfml_util::gui::TextInfo &      TEXT_INFO,
                    const heroespath::item::ItemSPtr_t & IITEM_SPTR);

        //used by the inventory stage to list conditions
        ListBoxItem(const std::string &                           NAME,
                    const sfml_util::gui::TextInfo &              TEXT_INFO,
                    const heroespath::creature::ConditionSPtr_t & CONDITION_SPTR_PARAM);

        //used by the inventory stage to list titles
        ListBoxItem(const std::string &                     NAME,
                    const sfml_util::gui::TextInfo &        TEXT_INFO,
                    const heroespath::creature::TitlePtrC_t TITLE_CPTRC_PARAM);

        //used by the inventory stage to list spells
        ListBoxItem(const std::string &                  NAME,
                    const sfml_util::gui::TextInfo &     TEXT_INFO,
                    const heroespath::spell::SpellPtrC_t SPELL_CPTRC_PARAM);

        heroespath::player::CharacterSPtr_t         character_sptr;
        heroespath::state::GameStateSPtr_t          gamestate_sptr;
        heroespath::item::ItemSPtr_t                iitem_sptr;
        heroespath::creature::ConditionSPtr_t       cond_sptr;
        const heroespath::creature::TitlePtrC_t     TITLE_CPTRC;
        const heroespath::spell::SpellPtrC_t        SPELL_PTRC;

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
#endif //APPBASE_SFMLUTIL_GUI_LISTBOXITEM_INCLUDED
