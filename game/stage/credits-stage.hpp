#ifndef GAME_CREDITSTAGE_INCLUDED
#define GAME_CREDITSTAGE_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/music-set.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "game/stage/credit.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace stage
{

    //A simple class that displays the credits
    class CreditsStage : public sfml_util::Stage
    {
        //prevent copy construction
        CreditsStage(const CreditsStage &) =delete;

        //prevent copy assignment
        CreditsStage & operator=(const CreditsStage &) =delete;

    public:
        CreditsStage();
        virtual ~CreditsStage();

        virtual void Setup();
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

    private:
        static const float DEFAULT_SCROLL_SPEED_;
        static const float SCROLL_SPEED_MULT_;
        static const float SCROLL_SPEED_MAX_;
        static const float CREDIT_BOX_INNER_PAD_;
        //
        const float CREDIT_BOX_WIDTH_;
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const float CREDITBOX_POS_LEFT_;
        //
        const sfml_util::MusicSetSPtr_t BG_MUSIC_SET_SPTR_;
        //
        float           creditBoxPosTop_;
        float           creditBoxHeight_;
        float           totalHeight_;
        float           heightTracker_;
        sf::Texture     hpTitleTexture_;
        sf::Sprite      bpTitleSprite_;
        sfml_util::gui::box::Box box_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        CreditSVec_t    creditSVec_;
        float           scrollSpeed_;
        bool            isKeyHeldDown_;
    };

}
}
#endif //GAME_CREDITSTAGE_INCLUDED
