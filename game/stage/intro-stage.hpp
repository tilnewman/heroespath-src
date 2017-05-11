#ifndef GAME_INTROSTAGE_INCLUDED
#define GAME_INTROSTAGE_INCLUDED
//
// intro-stage.hpp
//  A simple Stage class to display the intro title and start playing the theme music.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"


namespace game
{
namespace stage
{

    //A simple Stage class that displays the intro title
    class IntroStage : public sfml_util::Stage
    {
        //prevent copy construction
        IntroStage(const IntroStage &) =delete;

        //prevent copy assignment
        IntroStage & operator=(const IntroStage &) =delete;

    public:
        IntroStage();
        virtual ~IntroStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        sf::Texture titleTexture_;
        sf::Sprite  titleSprite_;
        std::size_t initialDrawHoldCounter_;
    };

}
}
#endif //GAME_INTROSTAGE_INCLUDED
