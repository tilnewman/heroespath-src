#ifndef GAME_OUROBOROS_INCLUDED
#define GAME_OUROBOROS_INCLUDED
//
// ouroboros.hpp
//  An animated snake eating its own tail for the background of main menu screens.
//
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/sliders.hpp"

#include <memory>
#include <string>


namespace game
{

    //manages an animated snake image that spins, grows/shrinks, and fades in/out
    class Ouroboros : public sfml_util::gui::GuiEntity
    {
        //prevent copy construction
        Ouroboros(const Ouroboros &);

        //prevent copy assignment
        Ouroboros & operator=(const Ouroboros &);

    public:
        Ouroboros(const std::string & NAME, const bool WILL_INVERT = false);
        virtual ~Ouroboros();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;
        virtual bool UpdateTime(const float ELAPSED_TIME_SEC);
        virtual void OnClick(const sf::Vector2f &) {}
    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        float rotation_;
        sf::Sprite  sprite_;
        sf::Texture texture_;
        sfml_util::sliders::Drifter<float> sizeDrifter_;
        sfml_util::sliders::Drifter<float> shadeDrifter_;
        sfml_util::sliders::Drifter<float> rotSpeedDrifter_;
    };

    using OuroborosSPtr_t = std::shared_ptr<Ouroboros>;

}
#endif //GAME_OUROBOROS_INCLUDED
