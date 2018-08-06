// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
//
// fade.hpp
//  Helper classes and functions for fading the screen in and out.
//
#include <SFML/Graphics/VertexArray.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // A class allowing simple fading in and out of a color to any part of the screen.
    class Fade : public sf::Drawable
    {
    public:
        Fade(const Fade &) = delete;
        Fade(Fade &&) = delete;
        Fade & operator=(const Fade &) = delete;
        Fade & operator=(Fade &&) = delete;

    public:
        Fade(); // must call Setup() before any other functinos if using this constructor

        Fade(
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT);

        explicit Fade(const sf::FloatRect & SCREEN_REGION);

        virtual ~Fade();

        void
            FadeTo(const sf::Color & NEW_COLOR = sf::Color::Black, const float SPEED_MULT = 200.0f);

        void Reset(const sf::Color & NEW_COLOR);

        const sf::Color GetCurrentColor() const;
        const sf::Color GetTargetColor() const;

        // returns true when the fade in or out is complete
        bool Update(const float SECONDS);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        void UpdateRegion(const float WIDTH, const float HEIGHT);

    private:
        void Setup(const sf::FloatRect & SCREEN_REGION);

        void Setup(
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT);

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

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_FADE_HPP_INCLUDED
