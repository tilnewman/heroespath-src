#ifndef APPBASE_SFMLUTIL_FADE_INCLUDED
#define APPBASE_SFMLUTIL_FADE_INCLUDED
//
// fade.hpp
//  Helper classes and functions for fading the screen in and out.
//
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <vector>


namespace sfml_util
{

    //A class allowing simple fading in and out of a color to any part of the screen.
    class Fade : public sf::Drawable
    {
        //prevent copy construction
        Fade(const Fade &);

        //prevent copy assignment
        Fade & operator=(const Fade &);

    public:
        Fade();//must call Setup() before any other functinos if using this constructor

        Fade(const float REGION_LEFT,
             const float REGION_TOP,
             const float REGION_WIDTH,
             const float REGION_HEIGHT);

        Fade(const sf::FloatRect & SCREEN_REGION);

        virtual ~Fade();

        void FadeTo(const sf::Color & NEW_COLOR  = sf::Color::Black,
                    const float       SPEED_MULT = 200.0f);

        void Reset(const sf::Color & NEW_COLOR);

        const sf::Color GetCurrentColor() const;
        const sf::Color GetTargetColor() const;

        //returns true when the fade in or out is complete
        bool Update(const float SECONDS);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        void UpdateRegion(const float WIDTH, const float HEIGHT);

    private:
        void Setup(const sf::FloatRect & SCREEN_REGION);

        void Setup( const float REGION_LEFT,
                    const float REGION_TOP,
                    const float REGION_WIDTH,
                    const float REGION_HEIGHT );

        float targetColorRed_;
        float targetColorGreen_;
        float targetColorBlue_;
        float targetColorAlpha_;
        float currentColorRed_;
        float currentColorGreen_;
        float currentColorBlue_;
        float currentColorAlpha_;
        float speedMultiplier_;
        sf::VertexArray quad_;
        bool isRedIncr_;
        bool isBlueIncr_;
        bool isGreenIncr_;
        bool isAlphaIncr_;
    };


    using FadeSPtr_t = std::shared_ptr<Fade>;
    using FadeSVec_t = std::vector<FadeSPtr_t>;

}
#endif //APPBASE_SFMLUTIL_FADE_INCLUDED
