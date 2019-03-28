// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
#define HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
//
// character-stage.hpp
//  A Stage class that displays the character creation screen.
//
#include "creature/creature.hpp"
#include "creature/race-stats.hpp"
#include "creature/role-stats.hpp"
#include "creature/stat-set.hpp"
#include "gui/box-entity.hpp"
#include "gui/cached-texture.hpp"
#include "gui/font-manager.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/main-menu-background.hpp"
#include "gui/main-menu-buttons.hpp"
#include "gui/radio-or-check-set.hpp"
#include "gui/sliderbar.hpp"
#include "gui/sliders.hpp"
#include "gui/stage-title.hpp"
#include "gui/text-entry-box.hpp"
#include "gui/text-region.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"
#include "stage/character-stage-stat-anim.hpp"
#include "stage/character-stage-stat-box.hpp"
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

    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

} // namespace gui

namespace stage
{

    // manages the CharacterCreation process
    class CharacterStage
        : public stage::StageBase
        , public gui::RadioOrCheckSet::Callback_t::IHandler_t
        , public misc::PopupCallback_t::IHandler_t
        , public gui::SliderBar::Callback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
        , public gui::TextEntryBox::Callback_t::IHandler_t
    {
    public:
        CharacterStage();
        virtual ~CharacterStage();

        CharacterStage(const CharacterStage &) = delete;
        CharacterStage(CharacterStage &&) = delete;
        CharacterStage & operator=(const CharacterStage &) = delete;
        CharacterStage & operator=(CharacterStage &&) = delete;

        const std::string HandleCallback(
            const gui::RadioOrCheckSet::Callback_t::Packet_t &, const std::string &) override;

        const std::string HandleCallback(
            const misc::PopupCallback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION) override;

        const std::string HandleCallback(
            const gui::ImageTextEntity::Callback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION) override;

        const std::string HandleCallback(
            const gui::SliderBar::Callback_t::Packet_t &, const std::string &) override;

        const std::string HandleCallback(
            const gui::TextEntryBox::Callback_t::Packet_t &, const std::string &) override;

        void Setup() override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        bool KeyPress(const sf::Event::KeyEvent & KE) override;
        bool KeyRelease(const sf::Event::KeyEvent & KE) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) override;
        virtual bool AreAnyAnimNumStillMoving() const;

    private:
        void Setup_MenuButtons();
        void Setup_RaceRadioButtons();
        void Setup_RoleRadioButtons();
        void Setup_RaceDescriptionBox();
        void Setup_RoleDescriptionBox();
        void Setup_NameLabel();
        void Setup_NameTextEntryBox();
        void Setup_SexRadioButtons();
        void Setup_SpacebarInstructionText();
        void Setup_StatsAnim();

        // returns the vertical position on screen
        float Setup_AttributeDescriptionBox();

        void Setup_Attribute(
            const creature::Traits::Enum TRAIT_ENUM,
            const std::string & DESC_KEY,
            const sf::FloatRect & REGION,
            gui::TextInfo & descTextInfo,
            gui::TextRegionUVec_t & textRegionUVec);

        void Setup_AttributeHelpText(
            const creature::Traits::Enum TRAIT_ENUM,
            const sf::FloatRect & REGION,
            gui::TextInfo & helpTextInfo,
            gui::TextRegionUVec_t & textRegionUVec);

        const std::string OnSaveButton();
        bool OnBackButton();
        const std::string OnNextButton();

        void MissingAttributesPopup();
        void CharacterNameMissingPopup();
        void CharacterImageSelectionPopup(const std::string & CHARACTER_NAME);
        void LeaveStageConfirmPopup(const std::string & CHARACTER_NAME);
        void Help1Popup();
        void Help2Popup();
        void Help3Popup();
        void CharacterCreationConfirmPopup(const std::size_t IMAGE_INDEX);
        void AdjustRoleRadioButtonsForRace(const creature::race::Enum WHICH_RACE);

        // returns true if any text was set that needs to be displayed
        bool GetStatHelpText(
            const creature::Traits::Enum WHICH_STAT, gui::TextInfo & textInfo) const;

        void UndoAndClearStatModifierChanges();
        void SetVisibleStatsToStatSetBase();
        void HandleChangedStatModifiers();
        void CreateStatModifers();
        void ApplyStatModifiersToStatSetBase();

        // float GetStatPosTop(const creature::Traits::Enum STAT) const; // returns <0.0f on error
        bool AreAnyStatsIgnored() const;
        void HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V);
        void ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS);
        void SwapAttributes(const creature::Traits::Enum A, const creature::Traits::Enum B);
        void HandleStuckAnims(const float ELAPSED_TIME_SEC);
        bool HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent &);
        void ResetForNewCharacterCreation();
        bool RaceChange(const creature::race::Enum);
        bool RoleChange();
        void CreateCharacter();
        creature::sex::Enum GetCurrentSelectedSex() const;

        void SetMenuButtonsDisabledWhileStatsAreAnimating(const bool WILL_DISABLE);

        const gui::BoxEntityInfo MakeGuiBoxEntityInfo() const;

    private:
        // const creature::Trait_t STAT_INVALID_;
        // const creature::Trait_t STAT_INITIAL_MAX_;

        const sf::Color LIGHT_TEXT_COLOR_;
        const sf::Color DESC_TEXT_COLOR_;

        const std::string POPUP_NAME_NONAMEERROR_;
        const std::string POPUP_NAME_MISSINGATTRIBS_;
        const std::string POPUP_NAME_CREATECONFIRM_;
        const std::string POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_;
        const std::string POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_;
        const std::string POPUP_NAME_HELP_1_;
        const std::string POPUP_NAME_HELP_2_;
        const std::string POPUP_NAME_HELP_3_;
        const std::string POPUP_NAME_IMAGE_SELECTION_;

        const unsigned int DESC_TEXT_FONT_SIZE_;
        const unsigned int RADIO_BUTTON_TEXT_SIZE_;

        gui::MainMenuBackground background_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        gui::StageTitle stageTitle_;
        StatBox statBox_;
        StatAnim statAnim_;

        gui::MainMenuButtonUPtr_t backButtonUPtr_;
        gui::MainMenuButtonUPtr_t saveButtonUPtr_;
        gui::MainMenuButtonUPtr_t helpButtonUPtr_;
        gui::MainMenuButtonUPtr_t nextButtonUPtr_;

        creature::StatSet statSetBase_;

        gui::RadioOrCheckSetUPtr_t raceRadioButtonUPtr_;
        gui::TextRegionUPtr_t racetDescTextRegionUPtr_;
        gui::RadioOrCheckSetUPtr_t roleRadioButtonUPtr_;
        gui::TextRegionUPtr_t roletDescTextRegionUPtr_;
        gui::RadioOrCheckSetUPtr_t sexRadioButtonUPtr_;
        gui::TextEntryBoxUPtr_t nameTextEntryBoxUPtr_;

        gui::TextRegionUPtr_t attrDescTextRegionUPtr_;

        gui::TextRegionUPtr_t sbInsTextRegionUPtr_;
        gui::SliderOscillator<sf::Uint8, float> sbInsTextSlider_;
        gui::TextRegionUPtr_t nInsTextRegionUPtr_;
        gui::SliderOscillator<sf::Uint8, float> nInsTextSlider_;

        gui::BottomSymbol bottomSymbol_;
        std::vector<std::string> characterImageFilenamesVec_;
        gui::CachedTexture woodCachedTexture_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
