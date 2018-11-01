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
#include "misc/not-null.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/image-text-entity.hpp"
#include "sfml-util/main-menu-background.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/radio-or-check-set.hpp"
#include "sfml-util/sliderbar.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage-title.hpp"
#include "sfml-util/text-entry-box.hpp"
#include "sfml-util/text-region.hpp"
#include "stage/character-stage-anim-num.hpp"
#include "stage/character-stage-stat-box.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

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
        //, public gui::RadioButtonSet::Callback_t::IHandler_t
        , public gui::PopupCallback_t::IHandler_t
        , public gui::SliderBar::Callback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
        , public gui::TextEntryBox::Callback_t::IHandler_t
    {
    public:
        CharacterStage(const CharacterStage &) = delete;
        CharacterStage(CharacterStage &&) = delete;
        CharacterStage & operator=(const CharacterStage &) = delete;
        CharacterStage & operator=(CharacterStage &&) = delete;

        CharacterStage();
        virtual ~CharacterStage();

        // required by callback handler

        // bool HandleCallback(const gui::RadioButton::Callback_t::PacketPtr_t &)
        // override;

        bool HandleCallback(const gui::PopupCallback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::SliderBar::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::TextEntryBox::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
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
        void Setup_SmokeAnimation(const float ATTRIB_BOX_TOP);

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

        bool OnSaveButton();
        bool OnBackButton();
        bool OnHelpButton();
        bool OnNextButton();

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

        float GetStatPosTop(const creature::Traits::Enum STAT) const; // returns <0.0f on error
        bool AreAnyStatsIgnored() const;
        void HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V);
        void ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS);
        void SwapAttributes(const creature::Traits::Enum A, const creature::Traits::Enum B);
        void HandleStuckAnims(const float ELAPSED_TIME_SEC);
        bool HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent &);
        void ResetForNewCharacterCreation();
        bool RaceChange(const creature::race::Enum);
        bool RoleChange();
        bool CreateCharacter();
        creature::sex::Enum GetCurrentSelectedSex() const;

        void SetMenuButtonsDisabledWhileStatsAreAnimating(const bool WILL_DISABLE);

    private:
        static const creature::Trait_t STAT_INVALID_;
        static const creature::Trait_t STAT_INITIAL_MAX_;
        //
        static const sf::Color LIGHT_TEXT_COLOR_;
        static const sf::Color DESC_TEXT_COLOR_;
        //
        static const std::string POPUP_NAME_NONAMEERROR_;
        static const std::string POPUP_NAME_MISSINGATTRIBS_;
        static const std::string POPUP_NAME_CREATECONFIRM_;
        static const std::string POPUP_NAME_BACKBUTTON_LEAVESCREENCONFIRM_;
        static const std::string POPUP_NAME_NEXTBUTTON_LEAVESCREENCONFIRM_;
        static const std::string POPUP_NAME_HELP_1_;
        static const std::string POPUP_NAME_HELP_2_;
        static const std::string POPUP_NAME_HELP_3_;
        static const std::string POPUP_NAME_IMAGE_SELECTION_;
        //
        static const float SMOKE_ANIM_SPEED_MIN_;
        static const float SMOKE_ANIM_SPEED_MAX_;
        //
        const unsigned int DESC_TEXT_FONT_SIZE_;
        const unsigned int RADIO_BUTTON_TEXT_SIZE_;
        //
        StatBox statBox_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        gui::StageTitle stageTitle_;
        //
        gui::SliderDrift<float> smokeAnimSliderDriftX_;
        gui::SliderDrift<float> smokeAnimSliderDriftY_;
        gui::MainMenuBackground background_;
        gui::AnimationUPtr_t smokeAnimUPtr_;
        //
        gui::MainMenuButtonUPtr_t backButtonUPtr_;
        gui::MainMenuButtonUPtr_t saveButtonUPtr_;
        gui::MainMenuButtonUPtr_t helpButtonUPtr_;
        gui::MainMenuButtonUPtr_t nextButtonUPtr_;
        //
        creature::StatSet statSetBase_;
        //
        // gui::RadioButtonSetUPtr_t raceRadioButtonUPtr_;
        gui::TextRegionUPtr_t racetDescTextRegionUPtr_;
        // gui::RadioButtonSetUPtr_t roleRadioButtonUPtr_;
        gui::TextRegionUPtr_t roletDescTextRegionUPtr_;
        // gui::RadioButtonSetUPtr_t sexRadioButtonUPtr_;
        gui::TextEntryBoxUPtr_t nameTextEntryBoxUPtr_;
        //
        gui::TextRegionUPtr_t attrDescTextRegionUPtr_;
        //
        gui::TextRegionUPtr_t sbInsTextRegionUPtr_;
        gui::SliderOscillator<sf::Uint8, float> sbInsTextSlider_;
        gui::TextRegionUPtr_t nInsTextRegionUPtr_;
        gui::SliderOscillator<sf::Uint8, float> nInsTextSlider_;
        //
        gui::BottomSymbol bottomSymbol_;
        std::vector<std::string> characterImageFilenamesVec_;
        gui::CachedTexture woodCachedTexture_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
