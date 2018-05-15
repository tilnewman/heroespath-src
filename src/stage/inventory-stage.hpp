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
#include "combat/combat-text.hpp"
#include "combat/creature-interaction.hpp"
#include "combat/fight-results.hpp"
#include "creature/achievement-enum.hpp"
#include "game/phase-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
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

    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box
    } // namespace gui
} // namespace sfml_util

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
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
        : public sfml_util::Stage
        , public popup::IPopupHandler_t
        , public sfml_util::gui::callback::IListBoxCallbackHandler
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
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

        // used to remember which view each character was last using
        using CharViewMap_t = misc::VectorMap<std::size_t, ViewType>;

    public:
        InventoryStage(const InventoryStage &) = delete;
        InventoryStage(InventoryStage &&) = delete;
        InventoryStage & operator=(const InventoryStage &) = delete;
        InventoryStage & operator=(InventoryStage &&) = delete;

        explicit InventoryStage(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const game::Phase::Enum CURRENT_PHASE);

        virtual ~InventoryStage();

        const std::string HandlerName() const override { return GetStageName(); }

        bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &) override;

        bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        bool HandleCallback(const popup::PopupResponse &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
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
            sfml_util::gui::FourStateButtonUPtr_t & sortButtonUPtr,
            sf::Sprite & sprite);

        void Setup_MenuButton(
            sfml_util::gui::FourStateButtonUPtr_t & buttonUPtr,
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
        void SetDescBoxTextFromListBoxItem(const sfml_util::gui::ListBoxItemSPtr_t &);
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
        float UpdateImageDetailsPosition(); // returns the sprite width

        bool IsDetailViewFading() const
        {
            return (isDetailViewFadingIn_ || isDetailViewFadingOut_);
        }

        bool IsDetailViewFadingOrVisible() const
        {
            return (IsDetailViewFading() || isDetailViewDoneFading_);
        }

        const item::ItemPtrOpt_t GetItemMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
        const sf::FloatRect GetItemRectMouseIsOver(const sf::Vector2f & MOUSE_POS_V);
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
        void ForceSelectionAndDrawOfListBox();
        bool HandleSpellsOrSongs();
        bool HandleSong_Step1_Play(const song::SongPtr_t);
        bool HandleSong_Step2_DisplayResults();

        void SystemErrorPopup(
            const std::string & GENERAL_ERROR_MSG,
            const std::string & TECH_ERROR_MSG,
            const std::string & TITLE_MSG = "");

        void SetDetailViewQuads();

        void SortByName(sfml_util::gui::ListBox & listbox, bool & isSortReversed);
        void SortByPrice(sfml_util::gui::ListBox & listbox, bool & isSortReversed);
        void SortByWeight(sfml_util::gui::ListBox & listbox, bool & isSortReversed);

        bool IfMouseDownIsOnDisabledButtonPopupRejection(
            const sfml_util::gui::FourStateButtonUPtr_t & BUTTON_UPTR,
            const sf::Vector2f & MOUSE_POS_V);

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
        const sf::Color LISTBOX_COLOR_IMAGE_;
        const sf::Color LISTBOX_COLOR_LINE_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const sf::Color LISTBOX_COLOR_TITLE_;
        const sf::Color DESCBOX_TEXT_COLOR_;
        const unsigned int DESCBOX_TEXT_SIZE_;
        const sfml_util::gui::ColorSet LISTBOX_COLORSET_;
        const sfml_util::gui::BackgroundInfo LISTBOX_BG_INFO_;
        sfml_util::MainMenuTitle mainMenuTitle_;
        const float CREATURE_IMAGE_POS_TOP_;
        const float LISTBOX_POS_TOP_;
        const float LISTBOX_HEIGHT_;
        const sf::FloatRect LISTBOX_REGION_;
        const sf::FloatRect DESCBOX_REGION_;
        const float DESCBOX_MARGIN_;
        const sfml_util::Margins DESCBOX_MARGINS_;
        const float DETAILVIEW_WIDTH_;
        const float DETAILVIEW_HEIGHT_;
        const float DETAILVIEW_POS_LEFT_;
        const float DETAILVIEW_POS_TOP_;
        const float SORT_ICON_SCALE_;
        const float SORT_ICON_SPACER_;
        const float SORT_ICON_POS_TOP_;
        const sf::Color SORT_ICON_COLOR_;

        sfml_util::gui::TextInfo listBoxItemTextInfo_;
        creature::CreaturePtr_t creaturePtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        sf::Texture paperBgTexture_;
        sf::Sprite paperBgSprite_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sf::Texture creatureTexture_;
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
        sfml_util::gui::ListBoxUPtr_t equippedListBoxUPtr_;
        sfml_util::gui::ListBoxUPtr_t unEquipListBoxUPtr_;
        sfml_util::gui::TextRegionUPtr_t insTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
        sfml_util::gui::box::BoxUPtr_t descBoxUPtr_;
        sfml_util::gui::TextRegionUPtr_t centerTextRegionUPtr_;

        sfml_util::gui::FourStateButtonUPtr_t backButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t itemsButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t titlesButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t condsButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t spellsButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t giveButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t shareButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t gatherButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t equipButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t unequipButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t dropButtonUPtr_;

        sf::Texture sortButtonNameTexture_;
        sf::Texture sortButtonPriceTexture_;
        sf::Texture sortButtonWeightTexture_;
        sfml_util::gui::FourStateButtonUPtr_t eqSortButtonNameUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t eqSortButtonPriceUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t eqSortButtonWeightUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t unEqSortButtonNameUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t unEqSortButtonPriceUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t unEqSortButtonWeightUPtr_;
        bool isSortReversedEqName_;
        bool isSortReversedEqPrice_;
        bool isSortReversedEqWeight_;
        bool isSortReversedUneqName_;
        bool isSortReversedUneqPrice_;
        bool isSortReversedUneqWeight_;

        // members that manage the give/drop/share/gather actions
        ActionType actionType_;
        ContentType contentType_;
        sfml_util::gui::ListBoxItemSPtr_t listBoxItemToGiveSPtr_;
        creature::CreaturePtrOpt_t creatureToGiveToPtrOpt_;
        sfml_util::gui::ListBoxItemSPtr_t itemToDropSPtr_;

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
        sf::Texture detailViewTexture_;
        sfml_util::gui::TextRegionUPtr_t detailViewTextUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> detailViewSlider_;

        // members that support spell casting (and song playing)
        spell::SpellPtrOpt_t spellBeingCastPtrOpt_;
        song::SongPtrOpt_t songBeingPlayedPtrOpt_;
        combat::TurnActionInfo turnActionInfo_;
        combat::FightResult fightResult_;
        std::size_t creatureEffectIndex_;
        std::size_t hitInfoIndex_;
        combat::CombatSoundEffectsUPtr_t combatSoundEffectsUPtr_;
        sfml_util::animation::SparkleAnimationUPtr_t sparkleAnimUPtr_;
        sfml_util::animation::SongAnimationUPtr_t songAnimUPtr_;

        // members that control combat action restrictions
        creature::CreaturePtr_t turnCreaturePtr_;
        game::Phase::Enum currentPhase_;
        bool hasTakenActionSpellOrSong_;

        combat::CreatureInteraction creatureInteraction_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_INVENTORY_STAGE_HPP_INCLUDED
