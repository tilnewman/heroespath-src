// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-size-and-scale.cpp
//
#include "sfml-util-size-and-scale.hpp"

#include "sfml-util/sfml-util-center-to.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f Size(const sf::Sprite & S, const sf::Vector2f & SCALE_V)
    {
        return Size(S.getGlobalBounds(), SCALE_V);
    }

    const sf::Vector2f Size(const sf::Sprite & S, const float SCALE)
    {
        return Size(S, sf::Vector2f(SCALE, SCALE));
    }

    void SetScaleAndReCenter(sf::Sprite & s, const sf::Vector2f & SCALE_V)
    {
        const auto ORIG_CENTER { CenterOf(s) };
        s.setScale(SCALE_V);
        s.setPosition((ORIG_CENTER - (Size(s) * 0.5f)) + s.getOrigin());
    }

    void SetScaleAndReCenter(sf::Sprite & s, const float SCALE)
    {
        SetScaleAndReCenter(s, sf::Vector2f(SCALE, SCALE));
    }

    void ScaleAndReCenter(sf::Sprite & s, const sf::Vector2f & SCALE_V)
    {
        const auto ORIG_CENTER { CenterOf(s) };
        s.setScale(s.getScale().x * SCALE_V.x, s.getScale().y * SCALE_V.y);
        s.setPosition((ORIG_CENTER - (Size(s) * 0.5f)) + s.getOrigin());
    }

    void ScaleAndReCenter(sf::Sprite & s, const float SCALE)
    {
        ScaleAndReCenter(s, sf::Vector2f(SCALE, SCALE));
    }

    void SetSize(sf::Sprite & s, const sf::Vector2f & V_ORIG)
    {
        const sf::Vector2f V { V_ORIG };
        auto newScaleV { s.getScale() };

        if (!misc::IsRealZeroOrLess(V.x) && (s.getLocalBounds().width > 0.0f))
        {
            newScaleV.x = V.x / s.getLocalBounds().width;
        }

        if (!misc::IsRealZeroOrLess(V.y) && (s.getLocalBounds().height > 0.0f))
        {
            newScaleV.y = V.y / s.getLocalBounds().height;
        }

        s.setScale(newScaleV);
    }

    void SetSizeAndPos(sf::Sprite & s, const sf::FloatRect & R)
    {
        SetSize(s, Size(R));
        s.setPosition(Position(R) + s.getOrigin());
    }

    bool IsSizeZeroOrLessEither(const sf::Sprite & S)
    {
        return IsSizeZeroOrLessEither(S.getGlobalBounds());
    }

    bool IsSizeZeroOrLessBoth(const sf::Sprite & S)
    {
        return IsSizeZeroOrLessBoth(S.getGlobalBounds());
    }

    bool IsSizeLessThanOneEither(const sf::Sprite & S)
    {
        return IsSizeLessThanOneEither(S.getGlobalBounds());
    }

    bool IsSizeLessThanOneBoth(const sf::Sprite & S)
    {
        return IsSizeLessThanOneBoth(S.getGlobalBounds());
    }

    bool IsSizeZeroOrLessEither(const sf::Text & T)
    {
        return IsSizeZeroOrLessEither(T.getGlobalBounds());
    }

    bool IsSizeZeroOrLessBoth(const sf::Text & T)
    {
        return IsSizeZeroOrLessBoth(T.getGlobalBounds());
    }

    bool IsSizeLessThanOneEither(const sf::Text & T)
    {
        return IsSizeLessThanOneEither(T.getGlobalBounds());
    }

    bool IsSizeLessThanOneBoth(const sf::Text & T)
    {
        return IsSizeLessThanOneBoth(T.getGlobalBounds());
    }

    const sf::FloatRect MinimallyEnclosing(
        const sf::Sprite & A,
        const sf::Sprite & B,
        const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS)
    {
        return MinimallyEnclosing(
            A.getGlobalBounds(), B.getGlobalBounds(), WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS);
    }

    const sf::FloatRect MinimallyEnclosing(
        const std::vector<sf::Sprite> & VEC, const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS)
    {
        std::vector<sf::FloatRect> frv;
        for (const auto & S : VEC)
        {
            frv.emplace_back(S.getGlobalBounds());
        }

        return MinimallyEnclosing(frv, WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS);
    }

    const sf::FloatRect MinimallyEnclosing(
        const sf::Text & A, const sf::Text & B, const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS)
    {
        return MinimallyEnclosing(
            A.getGlobalBounds(), B.getGlobalBounds(), WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS);
    }

    const sf::FloatRect MinimallyEnclosing(
        const std::vector<sf::Text> & VEC, const bool WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS)
    {
        std::vector<sf::FloatRect> frv;
        for (const auto & T : VEC)
        {
            frv.emplace_back(T.getGlobalBounds());
        }

        return MinimallyEnclosing(frv, WILL_EXCLUDE_IF_EITHER_SIZE_ZERO_OR_LESS);
    }

} // namespace sfml_util
} // namespace heroespath
