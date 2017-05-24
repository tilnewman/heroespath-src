#ifndef GAME_STAGE_LOADGAMEMENU_INCLUDED
#define GAME_STAGE_LOADGAMEMENU_INCLUDED
//
// load-game-menu-stage.hpp
//  A Stage class that allows players to load previous games
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/list-box.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/state/game-state.hpp"
#include "game/i-popup-callback.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"

#include <memory>
#include <vector>
#include <string>


namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
        using TextRegionSVec_t = std::vector<TextRegionSPtr_t>;
    }
}
namespace game
{
    class Ouroboros;
    using OuroborosSPtr_t = std::shared_ptr<Ouroboros>;

namespace stage
{

    //A Stage class that allows players to load saved games
    class LoadGameStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        //prevent copy construction
        LoadGameStage(const LoadGameStage &) =delete;

        //prevent copy assignment
        LoadGameStage & operator=(const LoadGameStage &) =delete;

    public:
        LoadGameStage();
        virtual ~LoadGameStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        void SetupGameInfoDisplay();

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        MainMenuTitle mainMenuTitle_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::gui::FourStateButtonSPtr_t backButtonSPtr_;
        sfml_util::gui::ListBoxSPtr_t gsListBoxSPtr_;
        //
        sfml_util::gui::TextRegionSPtr_t locTextRegionSPtr_;
        sfml_util::gui::TextRegionSVec_t charTextRegionSVec_;
        sfml_util::gui::TextRegionSPtr_t charLabelTextRegionSPtr_;
        //
        float gsListBoxPosLeft_;
        float gsListBoxPosTop_;
        float gsListBoxPosWidth_;
        float gsListBoxPosHeight_;
        //
        OuroborosSPtr_t ouroborosSPtr_;
        BottomSymbol bottomSymbol_;
    };

}
}
#endif //GAME_STAGE_LOADGAMEMENU_INCLUDED
