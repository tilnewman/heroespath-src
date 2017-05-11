#ifndef GAME_SETTINGSSTAGE_INCLUDED
#define GAME_SETTINGSSTAGE_INCLUDED
//
// settings-stage.hpp
//  A Stage class that allows changing system and game settings.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sliderbar-music.hpp"
#include "sfml-util/sliderbar-effects.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/radio-button.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/horiz-symbol.hpp"
#include "game/i-popup-callback.hpp"
#include "game/main-menu-title.hpp"

#include <memory>


namespace game
{
namespace stage
{

    //A simple class that displays the system and game settings
    class SettingsStage
    :
        public sfml_util::Stage,
        public game::callback::IPopupHandler_t,
        public sfml_util::callback::IRadioButtonSetCallbackHandler_t
    {
        //prevent copy construction
        SettingsStage(const SettingsStage &) =delete;

        //prevent copy assignment
        SettingsStage & operator=(const SettingsStage &) =delete;

    public:
        SettingsStage();
        virtual ~SettingsStage();

        //required by callback handler
        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
        virtual void HandleResolutionChange();
        virtual bool KeyRelease(const sf::Event::KeyEvent &);
        float GetBackButtonPosLeft() const;

    private:
        void ResetPositions();
        //
        static const float BOX_WIDTH_;
        static const float BOX_HEIGHT_;
        static const float BOX_INNER_PAD_;
        static const float MUSIC_VOLUME_HORIZ_POS_;
        static const float SOUNDEFFECTS_VOLUME_HORIZ_POS_;
        static const float ANTIALIAS_HORIZ_POS_;
        //
        float                                     boxTop_;
        sfml_util::gui::BackgroundImage           backgroundImage_;
        MainMenuTitle                             mainMenuTitle_;
        sfml_util::gui::box::Box                  box_;
        sfml_util::gui::TextRegionSVec_t          textRegionSVec_;
        sfml_util::gui::RadioButtonSetSPtr_t      resRadioButtonSetSPtr_;
        sfml_util::gui::TextRegionSPtr_t          resolutionTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t          musicVolumeTextRegionSPtr_;
        sfml_util::SliderBarLabeled_MusicSPtr_t   musicVolumeSliderBarSPtr_;
        sfml_util::gui::TextRegionSPtr_t          effectsVolumeTextRegionSPtr_;
        sfml_util::SliderBarLabeled_EffectsSPtr_t effectsVolumeSliderBarSPtr_;
        main_menu_buttons::BackButtonSPtr_t       backButtonSPtr_;
        sfml_util::gui::TextRegionSPtr_t          revisionTextRegionSPtr_;
        sfml_util::gui::RadioButtonSetSPtr_t      aaRadioButtonSetSPtr_;
        sfml_util::gui::TextRegionSPtr_t          aaTextRegionSPtr_;
        unsigned                                  prevAALevel_;
        BottomSymbol bottomSymbol_;
        sfml_util::gui::TextRegionSPtr_t musicInfoLabelTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t musicInfoTextRegionSPtr_;
    };

}
}
#endif //GAME_SETTINGSSTAGE_INCLUDED
