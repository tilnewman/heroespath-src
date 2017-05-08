//
// list-box-item.cpp
//
#include "list-box-item.hpp"

#include "game/player/character.hpp"
#include "game/state/game-state.hpp"
#include "game/item/item.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/title.hpp"
#include "game/spell/spell-base.hpp"

#include <tuple>


namespace sfml_util
{
namespace gui
{

    ListBoxItem::ListBoxItem(const std::string & NAME)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemBase")),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &                         NAME,
                             const sfml_util::gui::TextInfo &            TEXT_INFO)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemTextOnly"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &                         NAME,
                             const sfml_util::gui::TextInfo &            TEXT_INFO,
                             const game::player::CharacterSPtr_t & CHARACTER_SPTR)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemPlayerCharacter"), TEXT_INFO, sf::FloatRect()),
            character_sptr(CHARACTER_SPTR),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &                        NAME,
                             const sfml_util::gui::TextInfo &           TEXT_INFO,
                             const game::state::GameStateSPtr_t & GAMESTATE_SPTR)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemGameState"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(GAMESTATE_SPTR),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &                  NAME,
                             const sfml_util::gui::TextInfo &     TEXT_INFO,
                             const game::creature::ConditionPtr_t CONDITION_CPTRC_PARAM)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemCondition"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (CONDITION_CPTRC_PARAM),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &              NAME,
                             const sfml_util::gui::TextInfo & TEXT_INFO,
                             const game::item::ItemSPtr_t &   IITEM_SPTR)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemItem"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (IITEM_SPTR),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &               NAME,
                             const sfml_util::gui::TextInfo &  TEXT_INFO,
                             const game::creature::TitlePtrC_t TITLE_CPTRC_PARAM)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemTitle"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (TITLE_CPTRC_PARAM),
            SPELL_CPTRC   (nullptr)
    {}


    ListBoxItem::ListBoxItem(const std::string &              NAME,
                             const sfml_util::gui::TextInfo & TEXT_INFO,
                             const game::spell::SpellPtrC_t   SPELL_CPTRC_PARAM)
    :
            TextRegion    (std::string(NAME).append("_ListBoxItemSpell"), TEXT_INFO, sf::FloatRect()),
            character_sptr(),
            gamestate_sptr(),
            iitem_sptr    (),
            COND_CPTRC    (nullptr),
            TITLE_CPTRC   (nullptr),
            SPELL_CPTRC   (SPELL_CPTRC_PARAM)
    {}


    bool operator==(const ListBoxItem & L, const ListBoxItem & R)
    {
        return std::tie(L.character_sptr,
                        L.gamestate_sptr,
                        L.iitem_sptr,
                        L.COND_CPTRC,
                        L.TITLE_CPTRC,
                        L.SPELL_CPTRC)
               ==
               std::tie(R.character_sptr,
                        R.gamestate_sptr,
                        R.iitem_sptr,
                        R.COND_CPTRC,
                        R.TITLE_CPTRC,
                        R.SPELL_CPTRC);
    }


    bool operator<(const ListBoxItem & L, const ListBoxItem & R)
    {
        return std::tie(L.character_sptr,
                        L.gamestate_sptr,
                        L.iitem_sptr,
                        L.COND_CPTRC,
                        L.TITLE_CPTRC,
                        L.SPELL_CPTRC)
               <
               std::tie(R.character_sptr,
                        R.gamestate_sptr,
                        R.iitem_sptr,
                        R.COND_CPTRC,
                        R.TITLE_CPTRC,
                        R.SPELL_CPTRC);
    }

}
}
