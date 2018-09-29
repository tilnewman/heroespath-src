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
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/callback.hpp"
#include "sfml-util/gui/image-text-entity.hpp"
#include "sfml-util/gui/main-menu-background.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/gui/radio-or-check-set.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage-title.hpp"
#include "sfml-util/stage.hpp"
#include "stage/character-stage-anim-num.hpp"
#include "stage/character-stage-stat-box.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

} // namespace sfml_util

namespace stage
{

    // manages the CharacterCreation process
    class CharacterStage
        : public sfml_util::Stage
        //, public sfml_util::RadioButtonSet::Callback_t::IHandler_t
        , public sfml_util::PopupCallback_t::IHandler_t
        , public sfml_util::SliderBar::Callback_t::IHandler_t
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
        , public sfml_util::TextEntryBox::Callback_t::IHandler_t
    {
    public:
        CharacterStage(const CharacterStage &) = delete;
        CharacterStage(CharacterStage &&) = delete;
        CharacterStage & operator=(const CharacterStage &) = delete;
        CharacterStage & operator=(CharacterStage &&) = delete;

        CharacterStage();
        virtual ~CharacterStage();

        // required by callback handler

        // bool HandleCallback(const sfml_util::RadioButton::Callback_t::PacketPtr_t &)
        // override;

        bool HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::SliderBar::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::TextEntryBox::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyPress(const sf::Event::KeyEvent & KE) override;
        bool KeyRelease(const sf::Event::KeyEvent & KE) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        const sfml_util::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

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
            sfml_util::TextInfo & descTextInfo,
            sfml_util::TextRegionUVec_t & textRegionUVec);

        void Setup_AttributeHelpText(
            const creature::Traits::Enum TRAIT_ENUM,
            const sf::FloatRect & REGION,
            sfml_util::TextInfo & helpTextInfo,
            sfml_util::TextRegionUVec_t & textRegionUVec);

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
            const creature::Traits::Enum WHICH_STAT, sfml_util::TextInfo & textInfo) const;

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
        static const double SMOKE_ANIM_SPEED_MIN_;
        static const double SMOKE_ANIM_SPEED_MAX_;
        //
        const unsigned int DESC_TEXT_FONT_SIZE_;
        const unsigned int RADIO_BUTTON_TEXT_SIZE_;
        //
        StatBox statBox_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::StageTitle stageTitle_;
        //
        sfml_util::sliders::Drifter<float> smokeAnimDrifterX_;
        sfml_util::sliders::Drifter<float> smokeAnimDrifterY_;
        sfml_util::MainMenuBackground background_;
        sfml_util::AnimationUPtr_t smokeAnimUPtr_;
        //
        sfml_util::MainMenuButtonUPtr_t backButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t saveButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t helpButtonUPtr_;
        sfml_util::MainMenuButtonUPtr_t nextButtonUPtr_;
        //
        creature::StatSet statSetBase_;
        //
        // sfml_util::RadioButtonSetUPtr_t raceRadioButtonUPtr_;
        sfml_util::TextRegionUPtr_t racetDescTextRegionUPtr_;
        // sfml_util::RadioButtonSetUPtr_t roleRadioButtonUPtr_;
        sfml_util::TextRegionUPtr_t roletDescTextRegionUPtr_;
        // sfml_util::RadioButtonSetUPtr_t sexRadioButtonUPtr_;
        sfml_util::TextEntryBoxUPtr_t nameTextEntryBoxUPtr_;
        //
        sfml_util::TextRegionUPtr_t attrDescTextRegionUPtr_;
        //
        sfml_util::TextRegionUPtr_t sbInsTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> sbInsTextSlider_;
        sfml_util::TextRegionUPtr_t nInsTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> nInsTextSlider_;
        //
        sfml_util::BottomSymbol bottomSymbol_;
        std::vector<std::string> characterImageFilenamesVec_;
        sfml_util::CachedTexture woodCachedTexture_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
