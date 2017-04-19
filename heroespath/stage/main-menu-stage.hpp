#ifndef APPBASE_HEROESPATH_MAINMENUSTAGE_INCLUDED
#define APPBASE_HEROESPATH_MAINMENUSTAGE_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gradient.hpp"

#include "heroespath/horiz-symbol.hpp"
#include "heroespath/ouroboros.hpp"
#include "heroespath/main-menu-buttons.hpp"
#include "heroespath/i-popup-callback.hpp"
#include "sfml-util/gui/background-image.hpp"

#include <memory>


namespace heroespath
{
namespace stage
{

    //A simple class that displays the credits
    class MainMenuStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        //prevent copy construction
        MainMenuStage(const MainMenuStage &);

        //prevent copy assignment
        MainMenuStage & operator=(const MainMenuStage &);

    public:
        MainMenuStage();
        virtual ~MainMenuStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const sf::FloatRect SCREEN_RECT_;
        const float BUTTON_SCALE_;
        //
        sf::Texture                                     titleTexture_;
        sf::Sprite                                      titleSprite_;
        sfml_util::GradientRect                         gradient_;
        main_menu_buttons::ResumeButtonSPtr_t           resumeButtonSPtr_;
        main_menu_buttons::CreateCharactersButtonSPtr_t createButtonSPtr_;
        main_menu_buttons::SettingsButtonSPtr_t         settingsButtonSPtr_;
        main_menu_buttons::CreditsButtonSPtr_t          creditsButtonSPtr_;
        main_menu_buttons::ExitButtonSPtr_t             exitButtonSPtr_;
        OuroborosSPtr_t                                 ouroborosSPtr_;
        BottomSymbol                                    bottomSymbol_;
        sfml_util::gui::BackgroundImage                 backgroundImage_;
    };

    using MainMenuStageSPtr_t = std::shared_ptr<MainMenuStage>;

}
}
#endif //APPBASE_HEROESPATH_MAINMENUSTAGE_INCLUDED
