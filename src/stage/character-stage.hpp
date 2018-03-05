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
#ifndef HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
#define HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
//
// character-stage.hpp
//  A Stage class that displays the character creation screen.
//
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage.hpp"

#include "creature/race-stats.hpp"
#include "creature/role-stats.hpp"
#include "player/character.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "stage/character-stage-anim-num.hpp"
#include "stats/stat-set.hpp"

#include "popup/i-popup-callback.hpp"

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

    namespace gui
    {
        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box
    } // namespace gui
} // namespace sfml_util

namespace stage
{

    // wrapper class for the lower meny buttons
    class CharacterStage;
    class MenuButton : public sfml_util::gui::FourStateButton
    {
    public:
        MenuButton(const MenuButton &) = delete;
        MenuButton(const MenuButton &&) = delete;
        MenuButton & operator=(const MenuButton &) = delete;
        MenuButton & operator=(const MenuButton &&) = delete;

    public:
        MenuButton(
            const std::string & NAME,
            const std::string & IMAGE_FILENAME_UP,
            const std::string & IMAGE_FILENAME_OVER);

        virtual ~MenuButton() = default;
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        virtual void SetOwningCharacterStage(CharacterStage * const stagePtr)
        {
            stagePtr_ = stagePtr;
        }

    protected:
        virtual void OnClick(const sf::Vector2f &);
        CharacterStage * stagePtr_;
    };
    using MenuButtonUPtr_t = std::unique_ptr<MenuButton>;

    // encapsulates a stat modifier that is drawn to the screen
    struct StatModText : public sf::Drawable
    {
        StatModText(
            const stats::Traits::Enum STAT,
            const std::string & NAME, // name of race or role
            const stats::Trait_t VAL,
            const float POS_LEFT,
            const float POS_TOP)
            : value(VAL)
            , stat(STAT)
            , text_region_sptr(std::make_shared<sfml_util::gui::TextRegion>(
                  std::string(NAME).append("StatModText")))
        {
            const sf::Int8 COLOR_BASE(100);
            sf::Color color(COLOR_BASE, 255, COLOR_BASE);

            if (VAL < 0)
            {
                color = sf::Color(255, COLOR_BASE, COLOR_BASE);
            }

            std::ostringstream ss;
            ss << NAME << ((VAL > 0) ? "+" : "") << VAL;

            const sfml_util::gui::TextInfo TEXT_INFO(
                ss.str(),
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Small(),
                color,
                sfml_util::Justified::Left);

            const sf::FloatRect RECT(POS_LEFT, POS_TOP, 0.0f, 0.0f);
            text_region_sptr->Setup(TEXT_INFO, RECT);
        }

        virtual ~StatModText() = default;

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw(*text_region_sptr, states);
        }

        stats::Trait_t value;
        stats::Traits::Enum stat;
        sfml_util::gui::TextRegionSPtr_t text_region_sptr;
    };

    using StatModTextVec_t = std::vector<StatModText>;

    // manages the CharacterCreation process
    class CharacterStage
        : public sfml_util::Stage
        , public sfml_util::callback::IRadioButtonSetCallbackHandler_t
        , public popup::IPopupHandler_t
        , public sfml_util::gui::callback::ISliderBarCallbackHandler_t
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        CharacterStage(const CharacterStage &) = delete;
        CharacterStage(const CharacterStage &&) = delete;
        CharacterStage & operator=(const CharacterStage &) = delete;
        CharacterStage & operator=(const CharacterStage &&) = delete;

    public:
        CharacterStage();
        virtual ~CharacterStage();

        // required by callback handler
        inline virtual const std::string HandlerName() const override { return GetStageName(); }

        virtual bool
            HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t &) override;

        virtual bool HandleCallback(const popup::PopupResponse &) override;

        virtual bool
            HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &) override;

        virtual bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        virtual void Setup() override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        virtual bool KeyPress(const sf::Event::KeyEvent & KE) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE) override;
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;
        virtual sfml_util::gui::IGuiEntityPtr_t
            UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;
        virtual void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) override;
        virtual bool AreAnyAnimNumStillMoving() const;

    private:
        void Setup_Button(MenuButtonUPtr_t & buttonUPtr, const float VERT_POS);
        void Setup_RaceRadioButtons();
        void Setup_RoleRadioButtons();
        void Setup_RaceDescriptionBox();
        void Setup_RoleDescriptionBox();
        void Setup_NameLabel();
        void Setup_NameTextEntryBox();
        void Setup_SexRadioButtons();
        void Setup_SpacebarInstructionText();
        void Setup_StatBackgroundBox(const float STATBOX_POS_TOP);

        void Setup_StatLabels(
            const float STATBOX_POS_TOP, const sfml_util::gui::TextInfo & STAT_TEXT_INFO);

        void Setup_StatNumberPositions();
        void Setup_FixedStats(const sfml_util::gui::TextInfo &);
        void Setup_SmokeAnimation(const float ATTRIB_BOX_TOP);

        // returns the vertical position on screen
        float Setup_AttributeDescriptionBox();

        void Setup_Attribute(
            const stats::Traits::Enum TRAIT_ENUM,
            const std::string & DESC_KEY,
            const sf::FloatRect & REGION,
            sfml_util::gui::TextInfo & descTextInfo,
            sfml_util::gui::TextRegionUVec_t & textRegionUVec);

        void Setup_AttributeHelpText(
            const stats::Traits::Enum TRAIT_ENUM,
            const sf::FloatRect & REGION,
            sfml_util::gui::TextInfo & helpTextInfo,
            sfml_util::gui::TextRegionUVec_t & textRegionUVec);

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
        float GetAttributeNumPosTop(const stats::Traits::Enum STAT);

        // returns true if any text was set that needs to be displayed
        bool GetStatHelpText(
            const stats::Traits::Enum WHICH_STAT, sfml_util::gui::TextInfo & textInfo) const;

        void UndoAndClearStatModifierChanges();
        void SetVisibleStatsToStatSetBase();
        void HandleChangedStatModifiers();
        void CreateStatModifers();
        void ApplyStatModifiersToStatSetBase();

        // these functions return stats::Traits::Count on error or none result
        stats::Traits::Enum GetHeldDownStat() const;
        stats::Traits::Enum GetStatAbove(const stats::Traits::Enum STAT) const;
        stats::Traits::Enum GetStatBelow(const stats::Traits::Enum STAT) const;

        float GetStatPosTop(const stats::Traits::Enum STAT) const; // returns <0.0f on error
        bool AreAnyStatsIgnored() const;
        void HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V);
        void ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS);
        void SwapAttributes(const stats::Traits::Enum A, const stats::Traits::Enum B);
        void HandleStuckAnims(const float ELAPSED_TIME_SEC);
        bool HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent &);
        void ResetForNewCharacterCreation();
        bool RaceChange(const creature::race::Enum);
        bool RoleChange();
        bool CreateCharacter();

    private:
        static const stats::Trait_t STAT_INVALID_;
        static const stats::Trait_t STAT_INITIAL_MAX_;
        //
        static const sfml_util::gui::ColorSet GUI_DEFAULT_COLORSET_;
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
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const unsigned int SMALL_FONT_SIZE_;
        const unsigned int RADIO_BOX_TEXT_SIZE_;
        const float STATBOX_WIDTH_;
        const float STATBOX_HEIGHT_;
        const float STATBOX_POS_LEFT_;
        const float STATS_POS_LEFT_;
        //
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        //
        sfml_util::MainMenuTitle mainMenuTitle_;
        //
        float attribVertOffset1_;
        float attribVertOffset2_;
        //
        sfml_util::sliders::Drifter<float> smokeAnimDrifterX_;
        sfml_util::sliders::Drifter<float> smokeAnimDrifterY_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::AnimationUPtr_t smokeAnimUPtr_;
        //
        MenuButtonUPtr_t backButtonUPtr_;
        MenuButtonUPtr_t saveButtonUPtr_;
        MenuButtonUPtr_t helpButtonUPtr_;
        MenuButtonUPtr_t nextButtonUPtr_;
        //
        stats::StatSet statSetBase_;
        stats::StatSet statSetRace_;
        stats::StatSet statSetRole_;
        stats::StatSet statSetFixedAnim_;
        StatModTextVec_t statModifierTextVec_;
        bool willDrawStatModText_;
        //
        sfml_util::gui::TextRegion strLabelTextRegion_;
        sfml_util::gui::TextRegion accLabelTextRegion_;
        sfml_util::gui::TextRegion chaLabelTextRegion_;
        sfml_util::gui::TextRegion lckLabelTextRegion_;
        sfml_util::gui::TextRegion spdLabelTextRegion_;
        sfml_util::gui::TextRegion intLabelTextRegion_;
        //
        float statsLineLength_;
        float statsLine1PosTop_;
        float statsLine2PosTop_;
        float statsLine3PosTop_;
        float statsLine4PosTop_;
        float statsLine5PosTop_;
        float statsLine6PosTop_;
        float statsLineVertPosDiff_;
        //
        float statsFirstNumPosLeft_;
        float statsStrPosTop_;
        float statsAccPosTop_;
        float statsChaPosTop_;
        float statsLckPosTop_;
        float statsSpdPosTop_;
        float statsIntPosTop_;
        //
        sfml_util::gui::box::BoxUPtr_t statsBoxUPtr_;
        //
        bool isAnimStats_;
        bool isWaitingForStats_;
        float animStatsDelayPerSec_;
        float animStatsTimeCounterSec_;
        AnimNumSVec_t animStatsSVec_;
        AnimNumSVec_t fixedStatsSVec_;
        int initialRollCounter_;
        //
        float dragStartY_;
        stats::Traits::Enum closestDragStat_;
        //
        sfml_util::gui::RadioButtonSetUPtr_t raceRadioButtonUPtr_;
        sfml_util::gui::TextRegionUPtr_t racetDescTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetUPtr_t roleRadioButtonSPtr_;
        sfml_util::gui::TextRegionUPtr_t roletDescTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetUPtr_t sexRadioButtonUPtr_;
        sfml_util::gui::TextEntryBoxUPtr_t nameTextEntryBoxUPtr_;
        //
        sfml_util::gui::TextRegionUPtr_t attrDescTextRegionUPtr_;
        //
        sfml_util::gui::TextRegionUPtr_t sbInsTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> sbInsTextSlider_;
        sfml_util::gui::TextRegionUPtr_t nInsTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> nInsTextSlider_;
        //
        sfml_util::BottomSymbol bottomSymbol_;
        //
        std::size_t selectedImageIndex_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_HPP_INCLUDED
