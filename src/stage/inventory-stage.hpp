// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_INVENTORY_STAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_INVENTORY_STAGE_HPP_INCLUDED
//
// inventory-stage.hpp
//
#include "combat/creature-interaction.hpp"
#include "combat/turn-action-info.hpp"
#include "creature/achievement-enum.hpp"
#include "game/phase-enum.hpp"
#include "gui/box-entity-info.hpp"
#include "gui/cached-texture.hpp"
#include "gui/callback.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/list-box.hpp"
#include "gui/margins.hpp"
#include "gui/sliders.hpp"
#include "gui/stage-title.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    namespace animation
    {
        class SparkleAnimation;
        using SparkleAnimationUPtr_t = std::unique_ptr<SparkleAnimation>;

        class SongAnimation;
        using SongAnimationUPtr_t = std::unique_ptr<SongAnimation>;
    } // namespace animation

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

} // namespace gui

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;

    class Condition;
    using ConditionPtr_t = misc::NotNull<Condition *>;

    class Title;
    using TitlePtr_t = misc::NotNull<Title *>;

} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
    using ItemPtrOpt_t = boost::optional<ItemPtr_t>;
} // namespace item
namespace combat
{
    class CombatSoundEffects;
    using CombatSoundEffectsUPtr_t = std::unique_ptr<CombatSoundEffects>;
} // namespace combat

namespace stage
{

    // displays all the information about a player character including the inventory
    class InventoryStage
        : public stage::StageBase

        , public gui::PopupCallback_t::IHandler_t
        , public gui::ListBox<InventoryStage, item::ItemPtr_t>::Callback_t::IHandler_t

        , public gui::ListBox<InventoryStage, creature::ConditionPtr_t>::Callback_t::IHandler_t

        , public gui::ListBox<InventoryStage, spell::SpellPtr_t>::Callback_t::IHandler_t
        , public gui::ListBox<InventoryStage, creature::TitlePtr_t>::Callback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
    {
        using ItemListBox_t = gui::ListBox<InventoryStage, item::ItemPtr_t>;
        using ItemListElementPtr_t = gui::ListElementPtr_t<item::ItemPtr_t>;
        using ItemListBoxUPtr_t = std::unique_ptr<ItemListBox_t>;
        using ItemListElementPtrOpt_t = gui::ListElementPtrOpt_t<item::ItemPtr_t>;

        using CondListBox_t = gui::ListBox<InventoryStage, creature::ConditionPtr_t>;
        using CondListBoxUPtr_t = std::unique_ptr<CondListBox_t>;

        using CondListElementPtrOpt_t = gui::ListElementPtrOpt_t<creature::ConditionPtr_t>;

        using SpellListBox_t = gui::ListBox<InventoryStage, spell::SpellPtr_t>;
        using SpellListBoxUPtr_t = std::unique_ptr<SpellListBox_t>;
        using SpellListElementPtrOpt_t = gui::ListElementPtrOpt_t<spell::SpellPtr_t>;

        using TitleListBox_t = gui::ListBox<InventoryStage, creature::TitlePtr_t>;
        using TitleListBoxUPtr_t = std::unique_ptr<TitleListBox_t>;
        using TitleListElementPtrOpt_t = gui::ListElementPtrOpt_t<creature::TitlePtr_t>;

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

        // used to remember which view each character was last using
        using CharViewMap_t = misc::VectorMap<std::size_t, ViewType>;

    public:
        InventoryStage(const InventoryStage &) = delete;
        InventoryStage(InventoryStage &&) = delete;
        InventoryStage & operator=(const InventoryStage &) = delete;
        InventoryStage & operator=(InventoryStage &&) = delete;

        InventoryStage(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const game::Phase::Enum PREVIOUS_PHASE);

        virtual ~InventoryStage();

        bool HandleCallback(const ItemListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const CondListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const SpellListBox_t::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        bool HandleCallback(const TitleListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::PopupCallback_t::PacketPtr_t &) override;

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

    private:
        void Setup_PaperBackground();
        void Setup_InstructionText();
        void Setup_CreatureImage();
        void Setup_CreatureDetails(const bool WILL_UPDATE_POSITION);
        void Setup_CreatureStats();
        void Setup_CenterText();
        void Setup_ListBox();
        void Setup_DescBox(const bool WILL_MOVE_OFFSCREEN);
        void Setup_FirstListBoxTitle();
        void Setup_DescBoxTitle();
        void Setup_MenuButtons();
        void Setup_SortButtons();

        void Setup_SortButton(
            const std::string & NAME,
            const std::string & IMAGE_PATH_KEY,
            gui::ImageTextEntityUPtr_t & sortButtonUPtr);

        void Setup_MenuButton(
            gui::ImageTextEntityUPtr_t & buttonUPtr,
            const std::string & TEXT,
            const float HORIZ_OFFSET_MULT);

        void Setup_ButtonMouseoverText();

        void UpdateTime_ViewChangeNone(const float CUT_OFF_TIME_SEC);
        void UpdateTime_ViewChangeNormal(const float CUT_OFF_TIME_SEC);
        void UpdateTime_SlideCharacterImage(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SlideDetailText(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SlideCenterText(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SlideStatusText(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SlideListBox(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SlideDescBox(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_DetailView(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_DetailViewFade(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SparkleAnimation(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_SongAnimation(const float ELAPSED_TIME_SECONDS);

    protected:
        void AfterChangingViewTasks();
        bool HandleAchievementDisplay();
        bool HandleViewChange(const ViewType);
        bool HandleBack();
        bool HandleEquipRequest();
        bool HandleEquipActual();
        bool HandleUnequipRequest();
        bool HandleUnequipActual();
        bool HandleGiveRequestInitial();
        bool HandleGiveRequestItems();
        bool HandleGiveRequestCoins();
        bool HandleGiveRequestGems();
        bool HandleGiveRequestMeteorShards();
        bool HandleGiveActualItems(const creature::CreaturePtr_t);
        bool HandleGiveActualCoins(const creature::CreaturePtr_t);
        bool HandleGiveActualGems(const creature::CreaturePtr_t);
        bool HandleGiveActualMeteorShards(const creature::CreaturePtr_t);
        bool HandleShare();
        bool HandleGather();
        bool HandleDropRequest();
        bool HandleDropActual();
        bool HandlePlayerChangeBeforeOrAfter(const bool IS_NEXT_CREATURE_AFTER);
        bool HandlePlayerChangeIndex(const std::size_t CHARACTER_NUM); // zero indexed
        bool HandlePlayerChangeTo(const creature::CreaturePtr_t, const bool IS_SLIDING_LEFT);
        void StartSlidingAnimation(const bool IS_SLIDING_LEFT);

        void SetDescBoxText(const std::string &);

        void PopupCharacterSelectWindow(
            const std::string & PROMPT_TEXT,
            const bool CAN_SELECT_SELF = false,
            const bool CAN_SELECT_BEASTS = false);

        void PopupRejectionWindow(const std::string & MESSAGE);

        void PopupNumberSelectWindow(const std::string & PROMPT_TEXT, const std::size_t NUMBER_MAX);

        void PopupDoneWindow(const std::string &, const bool);
        void PopupContentSelectionWindow(const std::string &);

        void HandleCoinsGive(
            const std::size_t COUNT, const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR);

        void HandleGemsGive(
            const std::size_t COUNT, const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR);

        void HandleMeteorShardsGive(
            const std::size_t COUNT, const creature::CreaturePtr_t CREATURE_TO_GIVE_TO_PTR);

        void HandleCoinsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS);
        void HandleGemsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS);
        void HandleMeteorShardsGather(const bool WILL_TRIGGER_SECONDARY_ACTIONS);
        void HandleCoinsShare();
        void HandleGemsShare();
        void HandleMeteorShardsShare();
        void EndOfGiveShareGatherTasks();
        void UpdateImageDetailsPosition(); // returns the sprite width

        bool IsDetailViewFading() const
        {
            return (isDetailViewFadingIn_ || isDetailViewFadingOut_);
        }

        bool IsDetailViewFadingOrVisible() const
        {
            return (IsDetailViewFading() || isDetailViewDoneFading_);
        }

        const item::ItemPtrOpt_t GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
        const FloatRectOpt_t GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
        void SetupDetailViewItem(const item::ItemPtrOpt_t);
        void SetupDetailViewCreature(const creature::CreaturePtrOpt_t CREATURE_PTR_OPT);

        const std::string MakeTitleString(
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::AchievementType::Enum WHICH_ACHV) const;

        const std::string MakeTitleCountNeededString(
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::AchievementType::Enum WHICH_ACHV) const;

        const std::string
            MakeTitleSeparatorString(const creature::AchievementType::Enum WHICH_ACHV) const;

        void StartDetailViewFadeOutTasks();
        void HandleDetailViewMouseInterrupt(const sf::Vector2f & MOUSE_POS_V);
        bool HandleCast_Step1_TargetSelection(const spell::SpellPtr_t);

        void HandleCast_Step2_PerformSpell(
            const spell::SpellPtr_t, const creature::CreaturePVec_t &);

        bool HandleCast_Step3_DisplayResults();
        void ForceSelectionAndDrawOfLeftListBox();
        bool HandleSpellsOrSongs();
        bool HandleSong_Step1_Play(const song::SongPtr_t);
        bool HandleSong_Step2_DisplayResults();

        void SystemErrorPopup(
            const std::string & GENERAL_ERROR_MSG,
            const std::string & TECH_ERROR_MSG,
            const std::string & TITLE_MSG = "");

        void SetDetailViewQuads();

        bool IfMouseDownIsOnDisabledButtonPopupRejection(
            const gui::ImageTextEntityUPtr_t & BUTTON_UPTR, const sf::Vector2f & MOUSE_POS_V);

        template <typename Element_t>
        bool HandleConditionOrTitleCallback(
            const typename gui::ListBox<InventoryStage, Element_t>::Callback_t::PacketPtr_t &
                PACKET_PTR)
        {
            if ((PACKET_PTR->gui_event == gui::GuiEvent::Click)
                || (PACKET_PTR->gui_event == gui::GuiEvent::SelectionChange)
                || (PACKET_PTR->keypress_event.code == sf::Keyboard::Up)
                || (PACKET_PTR->keypress_event.code == sf::Keyboard::Down))
            {
                SetDescBoxTextToSelectedForConditionOrTitle(*PACKET_PTR->listbox_ptr);
                return true;
            }

            return false;
        }

        template <typename Element_t>
        bool SetDescBoxTextToSelectedForConditionOrTitle(
            const gui::ListBox<InventoryStage, Element_t> & LISTBOX)
        {
            if (LISTBOX.Empty())
            {
                return false;
            }
            else
            {
                const auto COND_OR_TITLE_PTR { LISTBOX.Selection()->Element() };
                SetDescBoxText(COND_OR_TITLE_PTR->Name() + "\n\n" + COND_OR_TITLE_PTR->LongDesc());
                return true;
            }
        }

        void FreeAllLeftListBoxes();

        void LeftListBoxSetHorizPosition(const float NEW_POS_HORIZ);

        void UpdateCreatureImage();

        void SetButtonDisabledIf(gui::ImageTextEntityUPtr_t & buttonUPtr, const bool WILL_DISABLE);

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
        static const std::string POPUP_NAME_SPELLBOOK_;
        static const std::string POPUP_NAME_SPELL_RESULT_;
        static const std::string POPUP_NAME_MUSICSHEET_;
        static const std::string POPUP_NAME_SONG_RESULT_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float INNER_PAD_;
        const sf::FloatRect INNER_RECT_;
        const float CREATURE_IMAGE_POS_LEFT_;
        const float CREATURE_IMAGE_SCALE_;
        const float CREATURE_IMAGE_HEIGHT_MAX_;
        const float LISTBOX_HEIGHT_REDUCTION_;
        const float LISTBOX_SCREEN_EDGE_MARGIN_;
        const float LISTBOX_BETWEEN_SPACER_;
        const float LISTBOX_WIDTH_;
        const float OUT_OF_SIGHT_POS_;
        const float FIRST_LISTBOX_POS_LEFT_;
        const float SECOND_LISTBOX_POS_LEFT_;
        const float STATS_POS_LEFT_;
        const sf::Color CHARATER_IMAGE_COLOR_;
        const sf::Color LISTBOX_COLOR_IMAGE_;
        const sf::Color LISTBOX_COLOR_LINE_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const sf::Color LISTBOX_COLOR_TITLE_;
        const sf::Color DESCBOX_TEXT_COLOR_;
        const unsigned int DESCBOX_TEXT_SIZE_;
        const gui::FocusColors LISTBOX_COLORSET_;
        gui::StageTitle stageTitle_;
        const float CREATURE_IMAGE_POS_TOP_;
        const float LISTBOX_POS_TOP_;
        const float LISTBOX_HEIGHT_;
        const sf::FloatRect LISTBOX_REGION_LEFT_;
        const sf::FloatRect LISTBOX_REGION_RIGHT_;
        const float DESCBOX_MARGIN_;
        const gui::Margins DESCBOX_MARGINS_;
        gui::BoxEntityInfo listBoxInfoLeft_;
        gui::BoxEntityInfo listBoxInfoRight_;
        // listBoxPacketLeft_;
        const float DETAILVIEW_WIDTH_;
        const float DETAILVIEW_HEIGHT_;
        const float DETAILVIEW_POS_LEFT_;
        const float DETAILVIEW_POS_TOP_;
        const float SORT_ICON_SCALE_;
        const float SORT_ICON_SPACER_;
        const float SORT_ICON_POS_TOP_;
        const sf::Color SORT_ICON_COLOR_;

        const gui::TextInfo LIST_ELEMENT_TEXT_INFO_DEFAULT_;
        creature::CreaturePtr_t creaturePtr_;
        gui::BottomSymbol bottomSymbol_;
        gui::CachedTexture paperBgCachedTexture_;
        sf::Sprite paperBgSprite_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        sf::Sprite creatureSprite_;
        ViewType view_;
        CharViewMap_t characterViewMap_;

        // members responsible for animating view changes
        bool isSliderAnimating_;
        bool isSlidingLeft_;
        bool isViewForcedToItems_;
        ViewType viewToChangeTo_;
        float sliderAnimTimerSec_;
        float detailsPosLeft_;
        float centerPosLeft_;
        bool isImageSliding_;
        bool isDetailsSliding_;
        bool isCenterSliding_;
        bool isStatsSliding_;
        bool isListBoxSliding_;
        bool isDescBoxSliding_;
        bool isImageSlidingDone_;
        bool isDetailsSlidingDone_;
        bool isCenterSlidingDone_;
        bool isStatsSlidingDone_;
        bool isListBoxSlidingDone_;
        bool isDescBoxSlidingDone_;
        bool hasImageChanged_;
        bool hasDetailsChanged_;
        bool hasCenterChanged_;
        bool hasStatsChanged_;
        bool hasListBoxChanged_;
        bool hasDescBoxChanged_;
        gui::SliderZeroToOne imageSlider_;
        gui::SliderZeroToOne detailsSlider_;
        gui::SliderZeroToOne centerSlider_;
        gui::SliderZeroToOne statsSlider_;
        gui::SliderZeroToOne listBoxSlider_;
        gui::SliderZeroToOne descBoxSlider_;

        gui::TextRegionUPtr_t detailsTextRegionUPtr_;
        gui::TextRegionUPtr_t statsTextRegionUPtr_;
        gui::TextRegionUPtr_t eqTitleTextRegionUPtr_;
        gui::TextRegionUPtr_t unEqTitleTextRegionUPtr_;

        ItemListBoxUPtr_t itemLeftListBoxUPtr_; // equipped items
        CondListBoxUPtr_t condLeftListBoxUPtr_;
        SpellListBoxUPtr_t spellLeftListBoxUPtr_;
        TitleListBoxUPtr_t titleLeftListBoxUPtr_;

        ItemListBoxUPtr_t itemRightListBoxUPtr_; // unequipped items

        gui::TextRegionUPtr_t insTextRegionUPtr_;
        gui::TextRegionUPtr_t descTextRegionUPtr_;
        gui::BoxEntityUPtr_t descBoxUPtr_;
        gui::TextRegionUPtr_t centerTextRegionUPtr_;

        gui::ImageTextEntityUPtr_t backButtonUPtr_;
        gui::ImageTextEntityUPtr_t itemsButtonUPtr_;
        gui::ImageTextEntityUPtr_t titlesButtonUPtr_;
        gui::ImageTextEntityUPtr_t condsButtonUPtr_;
        gui::ImageTextEntityUPtr_t spellsButtonUPtr_;
        gui::ImageTextEntityUPtr_t giveButtonUPtr_;
        gui::ImageTextEntityUPtr_t shareButtonUPtr_;
        gui::ImageTextEntityUPtr_t gatherButtonUPtr_;
        gui::ImageTextEntityUPtr_t equipButtonUPtr_;
        gui::ImageTextEntityUPtr_t unequipButtonUPtr_;
        gui::ImageTextEntityUPtr_t dropButtonUPtr_;

        gui::ImageTextEntityUPtr_t eqSortButtonNameUPtr_;
        gui::ImageTextEntityUPtr_t eqSortButtonPriceUPtr_;
        gui::ImageTextEntityUPtr_t eqSortButtonWeightUPtr_;
        gui::ImageTextEntityUPtr_t unEqSortButtonNameUPtr_;
        gui::ImageTextEntityUPtr_t unEqSortButtonPriceUPtr_;
        gui::ImageTextEntityUPtr_t unEqSortButtonWeightUPtr_;

        bool isSortReversedEqName_;
        bool isSortReversedEqPrice_;
        bool isSortReversedEqWeight_;
        bool isSortReversedUneqName_;
        bool isSortReversedUneqPrice_;
        bool isSortReversedUneqWeight_;

        // members that manage the give/drop/share/gather actions
        ActionType actionType_;
        ContentType contentType_;
        ItemListElementPtrOpt_t listElementToGivePtrOpt_;
        creature::CreaturePtrOpt_t creatureToGiveToPtrOpt_;
        ItemListElementPtrOpt_t listElementToDropPtrOpt_;

        // members that manage the item detail view
        bool isDetailViewFadingIn_;
        bool isDetailViewFadingOut_;
        bool isDetailViewDoneFading_;
        bool isAchievementDisplaying_;
        bool hasMouseMoved_;
        bool isWaitingOnPopup_;
        float detailViewTimerSec_;
        float detailViewSliderRatio_;
        sf::Vector2f mousePosV_;
        sf::FloatRect detailViewSourceRect_;
        sf::VertexArray detailViewQuads_;
        sf::Sprite detailViewSprite_;
        gui::CachedTextureOpt_t detailViewCachedTextureOpt_;
        gui::TextRegionUPtr_t detailViewTextUPtr_;
        gui::SliderZeroToOne detailViewSlider_;

        // members that support spell casting (and song playing)
        spell::SpellPtrOpt_t spellBeingCastPtrOpt_;
        song::SongPtrOpt_t songBeingPlayedPtrOpt_;
        combat::TurnActionInfo turnActionInfo_;
        combat::FightResult fightResult_;
        std::size_t creatureEffectIndex_;
        std::size_t hitInfoIndex_;
        combat::CombatSoundEffectsUPtr_t combatSoundEffectsUPtr_;
        gui::animation::SparkleAnimationUPtr_t sparkleAnimUPtr_;
        gui::animation::SongAnimationUPtr_t songAnimUPtr_;

        // members that control combat action restrictions
        creature::CreaturePtr_t turnCreaturePtr_;
        game::Phase::Enum previousPhase_;
        bool hasTakenActionSpellOrSong_;

        combat::CreatureInteraction creatureInteraction_;

        // a mapping between creatures and their images
        misc::VectorMap<creature::CreaturePtr_t, gui::CachedTexture> creatureToImageMap_;

        float creatureImageWidthScaled_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_INVENTORY_STAGE_HPP_INCLUDED
