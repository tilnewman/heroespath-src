//
// color-shaker.cpp
//
#include "color-shaker.hpp"


namespace sfml_util
{

    ColorShaker::ColorShaker(const sf::Color & COLOR_ORIG,
                             const sf::Color & COLOR_ALT,
                             const float       SPEED,
                             const bool        WILL_START)
    :
        shaker_   (0.0f, 1.0f, SPEED),
        colorOrig_(COLOR_ORIG),
        colorAlt_ (COLOR_ALT),
        colorCurr_(COLOR_ORIG),
        isShaking_(WILL_START),
        redDiff_  (static_cast<float>(COLOR_ALT.r - COLOR_ORIG.r)),
        greenDiff_(static_cast<float>(COLOR_ALT.g - COLOR_ORIG.g)),
        blueDiff_ (static_cast<float>(COLOR_ALT.b - COLOR_ORIG.b)),
        alphaDiff_(static_cast<float>(COLOR_ALT.a - COLOR_ORIG.a))
    {}


    const sf::Color ColorShaker::Update(const float ELAPSED_TIME_SECONDS)
    {
        if (isShaking_)
        {
            const float NEW_RATIO(shaker_.Update(ELAPSED_TIME_SECONDS));

            colorCurr_ = colorOrig_;
            colorCurr_.r += static_cast<sf::Uint8>(NEW_RATIO * redDiff_);
            colorCurr_.g += static_cast<sf::Uint8>(NEW_RATIO * greenDiff_);
            colorCurr_.b += static_cast<sf::Uint8>(NEW_RATIO * blueDiff_);
            colorCurr_.a += static_cast<sf::Uint8>(NEW_RATIO * alphaDiff_);
        }

        return colorCurr_;
    }

}
