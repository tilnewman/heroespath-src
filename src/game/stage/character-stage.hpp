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
#ifndef GAME_CHARACTERSTAGE_HPP_INCLUDED
#define GAME_CHARACTERSTAGE_HPP_INCLUDED
//
// character-stage.hpp
//  A Stage class that displays the character creation screen.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/animation.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/sliderbar.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/stats/stat-set.hpp"
#include "game/stage/character-stage-anim-num.hpp"
#include "game/creature/race-stats.hpp"
#include "game/creature/role-stats.hpp"
#include "game/i-popup-callback.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"

#include <memory>
#include <string>
#include <vector>


//forward declarations
namespace sfml_util
{
namespace gui
{
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

namespace stage
{

    //wrapper class for the lower meny buttons
    class CharacterStage;
    class MenuButton : public sfml_util::gui::FourStateButton
    {
        MenuButton(const MenuButton &) =delete;
        MenuButton & operator=(const MenuButton &) =delete;

    public:
        MenuButton(const std::string & NAME,
                   const std::string & IMAGE_FILENAME_UP,
                   const std::string & IMAGE_FILENAME_OVER);

        virtual ~MenuButton() {}
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual void SetOwningCharacterStage(CharacterStage * const stagePtr) { stagePtr_ = stagePtr; }
    protected:
        virtual void OnClick(const sf::Vector2f &);
        CharacterStage * stagePtr_;
    };
    using MenuButtonSPtr_t = std::shared_ptr<MenuButton>;


    //encapsulates a stat modifier that is drawn to the screen
    struct StatModText : public sf::Drawable
    {
        StatModText(const stats::stat::Enum STAT,
                    const std::string &     NAME, //name of race or role
                    const stats::Stat_t     VAL,
                    const float             POS_LEFT,
                    const float             POS_TOP)
        :
            value(VAL),
            stat(STAT),
            text_region_sptr( new sfml_util::gui::TextRegion(std::string(NAME).append("StatModText")) )
        {
            const sf::Int8 COLOR_BASE(100);
            sf::Color color(COLOR_BASE, 255, COLOR_BASE);

            if (VAL < 0)
                color = sf::Color(255, COLOR_BASE, COLOR_BASE);

            std::ostringstream ss;
            ss << NAME << ((VAL > 0) ? "+" : "") << VAL;

            const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                     sfml_util::FontManager::Instance()->Font_Default2(),
                                                     sfml_util::FontManager::Instance()->Size_Small(),
                                                     color,
                                                     sfml_util::Justified::Left);

            const sf::FloatRect RECT(POS_LEFT, POS_TOP, 0.0f, 0.0f);
            text_region_sptr->Setup(TEXT_INFO, RECT);
        }

        virtual ~StatModText()
        {}

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            target.draw( * text_region_sptr, states);
        }

        stats::Stat_t value;
        stats::stat::Enum stat;
        sfml_util::gui::TextRegionSPtr_t text_region_sptr;
    };

    using StatModTextVec_t = std::vector<StatModText>;



    //manages the CharacterCreation process
    class CharacterStage
    :
        public sfml_util::Stage,
        public sfml_util::callback::IRadioButtonSetCallbackHandler_t,
        public game::callback::IPopupHandler_t,
        public sfml_util::gui::callback::ISliderBarCallbackHandler_t,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        //prevent copy construction
        CharacterStage(const CharacterStage &) =delete;

        //prevent copy assignment
        CharacterStage & operator=(const CharacterStage &) =delete;

    public:
        CharacterStage();
        virtual ~CharacterStage();

        //required by callback handler
        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);
        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);

        bool HandleCallback_SaveButton();
        bool HandleCallback_BackButton();
        bool HandleCallback_HelpButton();
        bool HandleCallback_NextButton();

        void AdjustRoleRadioButtonsForRace(const game::creature::race::Enum WHICH_RACE);

        virtual void Setup();

        //on error the pos for Intelligence is returned
        //depends on members attribVertOffset1_ and attribVertOffset2_ being set before running
        float GetAttributeNumPosTop(const stats::stat::Enum STAT);

        void SetupRaceDescriptionBox();
        void SetupRoleDescriptionBox();
        void SetupAttrDescriptionBox();

        //returns true if any text was set that needs to be displayed
        bool GetStatHelpText(const stats::stat::Enum WHICH_STAT,
                             sfml_util::gui::TextInfo & textInfo) const;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        void UndoAndClearStatModifierChanges();
        void SetVisibleStatsToStatSetBase();
        void HandleChangedStatModifiers(const std::string & WHEN_STR);
        void CreateStatModifers();
        void ApplyStatModifiersToStatSetBase();

        //these functions return stats::stat::Count on error or none result
        stats::stat::Enum GetHeldDownStat() const;
        stats::stat::Enum GetStatAbove(const stats::stat::Enum STAT) const;
        stats::stat::Enum GetStatBelow(const stats::stat::Enum STAT) const;

        //returns a value less than zero on error
        float GetStatPosTop(const stats::stat::Enum STAT) const;

        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual sfml_util::gui::IGuiEntityPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        bool AreAnyAnimNumStillMoving() const;
        bool AreAnyStatsIgnored() const;

        void HandleAttributeDragging(const sf::Vector2f & MOUSE_POS_V);

        void ProduceAnimatingDigits(const float ELAPSED_TIME_SECONDS);

        void SwapAttributes(const stats::stat::Enum A, const stats::stat::Enum B);

        void HandleStuckAnims(const float ELAPSED_TIME_SEC);

        bool HandleMenuNavigationKeyRelease(const sf::Event::KeyEvent &);

        void ResetForNewCharacterCreation();

    private:
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
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const unsigned int SMALL_FONT_SIZE_;
        const unsigned int RADIO_BOX_TEXT_SIZE_;
        const float STATBOX_WIDTH_;
        const float STATBOX_HEIGHT_;
        const float STATBOX_POS_LEFT_;
        const float STATS_POS_LEFT_;
        //
        OuroborosUPtr_t ouroborosUPtr_;
        //
        MainMenuTitle mainMenuTitle_;
        //
        float attribVertOffset1_;
        float attribVertOffset2_;
        //
        float                                  smokeAnimmaxX_;
        sfml_util::sliders::Drifter<float>     smokeAnimDrifterX_;
        sfml_util::sliders::Drifter<float>     smokeAnimDrifterY_;
        sfml_util::gui::BackgroundImage        backgroundImage_;
        sfml_util::MultiTextureAnimationSPtr_t smokeAnimSPtr_;
        //
        MenuButtonSPtr_t backButtonSPtr_;
        MenuButtonSPtr_t saveButtonSPtr_;
        MenuButtonSPtr_t helpButtonSPtr_;
        MenuButtonSPtr_t nextButtonSPtr_;
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
        bool          isAnimStats_;
        bool          isWaitingForStats_;
        float         animStatsDelayPerSec_;
        float         animStatsTimeCounterSec_;
        AnimNumSVec_t animStatsSVec_;
        AnimNumSVec_t fixedStatsSVec_;
        int           initialRollCounter_;
        //
        float             dragStartY_;
        stats::stat::Enum closestDragStat_;
        //
        sfml_util::gui::RadioButtonSetSPtr_t raceRadioButtonSPtr_;
        sfml_util::gui::TextRegionUPtr_t     racetDescTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetSPtr_t roleRadioButtonSPtr_;
        sfml_util::gui::TextRegionUPtr_t     roletDescTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetSPtr_t sexRadioButtonSPtr_;
        sfml_util::gui::TextEntryBoxSPtr_t   nameTextEntryBoxSPtr_;
        //
        sfml_util::gui::TextRegionUPtr_t attrDescTextRegionUPtr_;
        //
        sfml_util::gui::TextRegionUPtr_t              sbInsTextRegionUPtr_;
        sfml_util::sliders::Slider2<sf::Uint8, float> sbInsTextSlider_;
        sfml_util::gui::TextRegionUPtr_t              nInsTextRegionUPtr_;
        sfml_util::sliders::Slider2<sf::Uint8, float> nInsTextSlider_;
        //
        BottomSymbol bottomSymbol_;
        //
        std::size_t selectedImageIndex_;
    };

}
}

#endif //GAME_CHARACTERSTAGE_HPP_INCLUDED
