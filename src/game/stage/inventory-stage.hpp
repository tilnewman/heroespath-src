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
#ifndef GAME_STAGE_INVENTORY_STAGE_HPP_INCLUDED
#define GAME_STAGE_INVENTORY_STAGE_HPP_INCLUDED
//
// inventory-stage.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/four-state-button.hpp"

#include "game/main-menu-title.hpp"
#include "game/horiz-symbol.hpp"
#include "game/i-popup-callback.hpp"

#include <memory>
#include <string>
#include <map>


namespace sfml_util
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    namespace box
    {
        class Box;
        using BoxUPtr_t = std::unique_ptr<Box>;
    }
}
}
namespace game
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    namespace creature
    {
        class Creature;
        using CreaturePtr_t   = Creature *;
        using CreatureCPtr_t  = const Creature *;
        using CreaturePtrC_t  = Creature * const;
        using CreatureCPtrC_t = const Creature * const;
    }

    namespace item
    {
        class Item;
        using ItemPtr_t = Item *;
    }

namespace stage
{

    //displays all the information about a player character including the inventory
    class InventoryStage
    :
        public sfml_util::Stage,
        public game::callback::IPopupHandler_t,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        enum class ViewType
        {
            Items = 0,
            Titles,
            Conditions,
            Spells,
            Count
        };

        enum class ContentType
        {
            Item = 0,
            Coins,
            Gems,
            MeteorShards,
            Count
        };

        enum class ActionType
        {
            Share = 0,
            Gather,
            Drop,
            Give,
            Count
        };

        //used to remember which view each character was last using
        using CharViewMap_t = std::map<std::size_t, ViewType>;

        //prevent copy construction
        InventoryStage(const InventoryStage &) =delete;

        //prevent copy assignment
        InventoryStage & operator=(const InventoryStage &) =delete;

    public:
        explicit InventoryStage(creature::CreaturePtr_t creaturePtr);
        virtual ~InventoryStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);

        void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);

    protected:
        void AfterChangingViewTasks();
        void SetupButtons();
        bool HandleAchievementDisplay();
        bool HandleViewChange(const ViewType);
        bool HandleBack();
        bool HandleEquip();
        bool HandleUnequip();
        bool HandleGive();
        bool HandleShare();
        bool HandleGather();
        bool HandleDrop();
        bool HandlePlayerChangeBeforeOrAfter(const bool IS_NEXT_CREATURE_AFTER);
        bool HandlePlayerChangeIndex(const std::size_t CHARACTER_NUM);//zero indexed
        bool HandlePlayerChangeTo(const creature::CreaturePtrC_t, const bool IS_SLIDING_LEFT);
        void StartSlidingAnimation(const bool IS_SLIDING_LEFT);
        void SetupCreatureImage();
        void SetupCreatureDetails(const bool WILL_UPDATE_POSITION);
        void SetupCreatureStats();
        void SetupCenterText();
        void SetupListBox();
        void SetupDescBox(const bool WILL_MOVE_OFFSCREEN);
        void SetupFirstListBoxTitle();
        void SetupDescBoxTitle();
        void SetDescBoxTextFromListBoxItem(const sfml_util::gui::ListBoxItemSPtr_t &);
        void SetDescBoxText(const std::string &);
        void PopupCharacterSelectWindow(const std::string &);
        void PopupRejectionWindow(const std::string &, const bool WILL_USE_REGULAR_SIZE_POPUP = false);
        void PopupNumberSelectWindow(const std::string & PROMPT_TEXT, const std::size_t NUMBER_MAX);
        void PopupDoneWindow(const std::string &, const bool);
        void PopupContentSelectionWindow(const std::string &);
        void HandleCoinsGive(const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr);
        void HandleGemsGive(const std::size_t COUNT, creature::CreaturePtr_t creatureToGiveToPtr);
        void HandleMeteorShardsGive(const std::size_t COUNT, creature::CreaturePtr_t cretureToGiveToPtr);
        void HandleCoinsGather(const bool WILL_POPUP);
        void HandleGemsGather(const bool WILL_POPUP);
        void HandleMeteorShardsGather(const bool WILL_POPUP);
        void HandleCoinsShare();
        void HandleGemsShare();
        void HandleMeteorShardsShare();
        void EndOfGiveShareGatherTasks();
        float UpdateImageDetailsPosition();//returns the sprite width
        inline bool IsDetailViewFading() const { return (isDetailViewFadingIn_ || isDetailViewFadingOut_); }
        inline bool IsDetailViewFadingOrVisible() const { return (IsDetailViewFading() || isDetailViewDoneFading_); }
        item::ItemPtr_t GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
        const sf::FloatRect GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
        void SetupDetailViewItem(const item::ItemPtr_t);
        void SetupDetailViewCreature(creature::CreatureCPtrC_t CREATURE_CPTRC);
        void StartDetailViewFadeOutTasks();
        void HandleDetailViewMouseInterrupt(const sf::Vector2f & MOUSE_POS_V);
        ViewType EstablishViewToUse() const;

    public:
        static const float VIEW_CHANGE_SLIDER_SPEED_;
        static const float VIEW_CHANGE_BETWEEN_TIME_SEC_;
        static const float CREATURE_IMAGE_RIGHT_PAD_;
        static const float DETAILVIEW_SLIDER_SPEED_;
        static const float DETAILVIEW_TIMER_DURATION_SEC_;
        static const float DETAILVIEW_COLOR_ALPHA_START_;
        static const float DETAILVIEW_COLOR_ALPHA_END_;
        static const float DETAILVIEW_INNER_PAD_;
        static const std::string POPUP_NAME_GIVE_;
        static const std::string POPUP_NAME_CHAR_SELECT_;
        static const std::string POPUP_NAME_NUMBER_SELECT_;
        static const std::string POPUP_NAME_CONTENTSELECTION_;
        static const std::string POPUP_NAME_DROPCONFIRM_;

    private:
        const float                 SCREEN_WIDTH_;
        const float                 SCREEN_HEIGHT_;
        const float                 INNER_PAD_;
        const sf::FloatRect         INNER_RECT_;
        const float                 CREATURE_IMAGE_POS_LEFT_;
        const float                 CREATURE_IMAGE_SCALE_;
        const float                 CREATURE_IMAGE_HEIGHT_MAX_;
        const float                 LISTBOX_HEIGHT_REDUCTION_;
        const float                 LISTBOX_SCREEN_EDGE_MARGIN_;
        const float                 LISTBOX_BETWEEN_SPACER_;
        const float                 LISTBOX_WIDTH_;
        const float                 OUT_OF_SIGHT_POS_;
        const float                 FIRST_LISTBOX_POS_LEFT_;
        const float                 SECOND_LISTBOX_POS_LEFT_;
        const float                 STATS_POS_LEFT_;
        const sf::Color             LISTBOX_COLOR_IMAGE_;
        const sf::Color             LISTBOX_COLOR_LINE_;
        const sf::Color             LISTBOX_COLOR_FG_;
        const sf::Color             LISTBOX_COLOR_BG_;
        const sf::Color             LISTBOX_COLOR_TITLE_;
        const sf::Color             DESCBOX_TEXT_COLOR_;
        const unsigned int          DESCBOX_TEXT_SIZE_;
        const sfml_util::gui::ColorSet LISTBOX_COLORSET_;
        const sfml_util::gui::BackgroundInfo LISTBOX_BG_INFO_;
        MainMenuTitle               mainMenuTitle_;
        const float                 CREATURE_IMAGE_POS_TOP_;
        const float                 LISTBOX_POS_TOP_;
        const float                 LISTBOX_HEIGHT_;
        const sf::FloatRect         LISTBOX_REGION_;
        const sf::FloatRect         DESCBOX_REGION_;
        const float                 DESCBOX_MARGIN_;
        const sfml_util::Margins    DESCBOX_MARGINS_;
        const float                 DETAILVIEW_WIDTH_;
        const float                 DETAILVIEW_HEIGHT_;
        const float                 DETAILVIEW_POS_LEFT_;
        const float                 DETAILVIEW_POS_TOP_;

        sfml_util::gui::TextInfo    listBoxItemTextInfo_;
        creature::CreaturePtr_t     creaturePtr_;
        BottomSymbol                bottomSymbol_;
        sf::Texture                 paperBgTexture_;
        sf::Sprite                  paperBgSprite_;
        OuroborosUPtr_t             ouroborosUPtr_;
        sf::Texture                 creatureTexture_;
        sf::Sprite                  creatureSprite_;
        ViewType                    view_;
        CharViewMap_t               characterViewMap_;

        //members responsible for animating view changes
        bool                        isSliderAnimating_;
        bool                        isSlidingLeft_;
        bool                        isViewForcedToItems_;
        ViewType                    viewToChangeTo_;
        float                       sliderAnimTimerSec_;
        float                       detailsPosLeft_;
        float                       centerPosLeft_;
        bool                        isImageSliding_;
        bool                        isDetailsSliding_;
        bool                        isCenterSliding_;
        bool                        isStatsSliding_;
        bool                        isListBoxSliding_;
        bool                        isDescBoxSliding_;
        bool                        isImageSlidingDone_;
        bool                        isDetailsSlidingDone_;
        bool                        isCenterSlidingDone_;
        bool                        isStatsSlidingDone_;
        bool                        isListBoxSlidingDone_;
        bool                        isDescBoxSlidingDone_;
        bool                        hasImageChanged_;
        bool                        hasDetailsChanged_;
        bool                        hasCenterChanged_;
        bool                        hasStatsChanged_;
        bool                        hasListBoxChanged_;
        bool                        hasDescBoxChanged_;
        sfml_util::sliders::ZeroSliderOnce<float> imageSlider_;
        sfml_util::sliders::ZeroSliderOnce<float> detailsSlider_;
        sfml_util::sliders::ZeroSliderOnce<float> centerSlider_;
        sfml_util::sliders::ZeroSliderOnce<float> statsSlider_;
        sfml_util::sliders::ZeroSliderOnce<float> listBoxSlider_;
        sfml_util::sliders::ZeroSliderOnce<float> descBoxSlider_;

        sfml_util::gui::TextRegionUPtr_t detailsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t statsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t eqTitleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t unEqTitleTextRegionUPtr_;
        sfml_util::gui::ListBoxSPtr_t    equippedListBoxSPtr_;
        sfml_util::gui::ListBoxSPtr_t    unEquipListBoxSPtr_;
        sfml_util::gui::TextRegionUPtr_t insTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
        sfml_util::gui::box::BoxUPtr_t   descBoxUPtr_;
        sfml_util::gui::TextRegionUPtr_t centerTextRegionUPtr_;

        sfml_util::gui::FourStateButtonSPtr_t backButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t itemsButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t titlesButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t condsButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t spellsButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t giveButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t shareButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t gatherButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t equipButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t unequipButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t dropButtonSPtr_;

        //members that manage the give/drop/share/gather actions
        ActionType                        actionType_;
        ContentType                       contentType_;
        sfml_util::gui::ListBoxItemSPtr_t listBoxItemToGiveSPtr_;
        creature::CreaturePtr_t           creatureToGiveToPtr_;
        item::ItemPtr_t                   iItemToDropPtr_;

        //members that manage the item detail view
        bool            isDetailViewFadingIn_;
        bool            isDetailViewFadingOut_;
        bool            isDetailViewDoneFading_;
        bool            isAchievementDisplaying_;
        bool            hasMouseMoved_;
        bool            isWaitingOnPopup_;
        float           detailViewTimerSec_;
        float           detailViewSliderRatio_;
        sf::Vector2f    mousePosV_;
        sf::FloatRect   detailViewSourceRect_;
        sf::VertexArray detailViewQuads_;
        sf::Sprite      detailViewSprite_;
        sf::Texture     detailViewTexture_;
        sfml_util::gui::TextRegionUPtr_t detailViewTextUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> detailViewSlider_;
    };

}
}

#endif //GAME_STAGE_INVENTORY_STAGE_HPP_INCLUDED
