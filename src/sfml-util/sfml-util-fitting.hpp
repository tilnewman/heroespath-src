// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_FITTING_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_FITTING_HPP_INCLUDED
//
// sfml-util-fitting.hpp
//
#include "misc/real.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfml_util
{

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <typename T, typename = std::enable_if_t<std::is_same<T, float>::value>>
    constexpr const sf::Vector2<T>
        FitCopy(const sf::Vector2<T> & ORIG, const sf::Vector2<T> & LIMIT, const T SCALE = 1.0f)
    {
        // can assume T = float

        float scaleToUse { 0.0f };

        auto scaleBasedOnWidth = [&]() constexpr->float { return (LIMIT.x / ORIG.x); };
        auto scaleBasedOnHeight = [&]() constexpr->float { return (LIMIT.y / ORIG.y); };

        if (misc::IsRealZero(LIMIT.x))
        {
            if (misc::IsRealZero(ORIG.y) == false)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }
        else if (misc::IsRealZero(LIMIT.y))
        {
            if (misc::IsRealZero(ORIG.x) == false)
            {
                scaleToUse = scaleBasedOnWidth();
            }
        }
        else if (misc::IsRealZero(ORIG.x))
        {
            if (misc::IsRealZero(ORIG.y) == false)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }
        else if (misc::IsRealZero(ORIG.y))
        {
            if (misc::IsRealZero(ORIG.x) == false)
            {
                scaleToUse = scaleBasedOnWidth();
            }
        }
        else
        {
            scaleToUse = scaleBasedOnWidth();

            if ((scaleToUse * ORIG.y) > LIMIT.y)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }

        return (ORIG * (scaleToUse * SCALE));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename std::
            enable_if<std::is_floating_point<Scale_t> {} && (!std::is_same<T1, T2> {}), int>::type
        = 0>
    constexpr const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        return sf::Vector2<T1>(
            FitCopy(sf::Vector2f(ORIG), sf::Vector2f(LIMIT), static_cast<float>(SCALE)));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T1> FitCopy(
        const T1 WIDTH_ORIG,
        const T1 HEIGHT_ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = 1.0f)
    {
        return FitCopy(
            sf::Vector2<T1>(WIDTH_ORIG, HEIGHT_ORIG),
            sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT),
            SCALE);
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {}) && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    constexpr const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = 1.0f)
    {
        return FitCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to the maximum size that fits within the size of LIMIT and is then
    // rescaled to SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        return FitCopy(ORIG, Size(LIMIT), SCALE);
    }

    // rescales orig to the maximum size that fits within LIMIT and then applies SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        Fit(sf::Vector2<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        orig = FitCopy(orig, LIMIT, SCALE);
    }

    // rescales orig to the maximum size that fits within the size of LIMIT and then
    // applies SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        Fit(sf::Vector2<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        Fit(orig, Size(LIMIT), SCALE);
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT)
    {
        if (misc::IsRealZero(ORIG.x) == false)
        {
            return (FitCopy(sf::Vector2f(ORIG), LIMIT).x / static_cast<float>(ORIG.x));
        }
        else if (misc::IsRealZero(ORIG.y) == false)
        {
            return (FitCopy(sf::Vector2f(ORIG), LIMIT).y / static_cast<float>(ORIG.y));
        }
        else
        {
            return 0.0f;
        }
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitCopy(
        const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        return { Position(ORIG),
                 sf::Vector2<T1>(
                     Size(sf::FloatRect(ORIG))
                     * (ScaleThatFits(Size(ORIG), LIMIT) * static_cast<float>(SCALE))) };
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {}) && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    constexpr const sf::Rect<T1> FitCopy(
        const sf::Rect<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = 1.0f)
    {
        return FitCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to the maximum size that fits within the size of LIMIT and is then
    // rescaled to SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1>
        FitCopy(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        return FitCopy(ORIG, Size(LIMIT), SCALE);
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT
    template <
        typename T1,
        typename T2,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {})),
            int>::type
        = 0>
    constexpr float
        ScaleThatFits(const sf::Vector2<T1> & ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT)
    {
        return ScaleThatFits(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns a scale that would make ORIG the maximum size that fits within the size of LIMIT
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT)
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT)
    {
        return ScaleThatFits(Size(ORIG), LIMIT);
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT
    template <
        typename T1,
        typename T2,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {})),
            int>::type
        = 0>
    constexpr float
        ScaleThatFits(const sf::Rect<T1> & ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT)
    {
        return ScaleThatFits(Size(ORIG), sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns a scale that would make ORIG the maximum size that fits within the size of LIMIT
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT)
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // rescales orig to the maximum size that fits within LIMIT and then applies SCALE,
    // if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        Fit(sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        orig = FitCopy(orig, LIMIT, SCALE);
    }

    // rescales orig to the maximum size that fits within the size of LIMIT and then
    // applies SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void Fit(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        Fit(orig, Size(LIMIT), SCALE);
    }

    // rescales s (global) to the maximum size that fits within LIMIT and then
    // applies SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void Fit(sf::Sprite & s, const sf::Vector2<T> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        const float NEW_SCALE { ScaleThatFits(Size(s), FitCopy(Size(s), LIMIT))
                                * static_cast<float>(SCALE) };

        s.setScale(NEW_SCALE, NEW_SCALE);
    }

    // rescales s (global) to the maximum size that fits within LIMIT and then
    // applies SCALE, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T> {} || std::is_floating_point<T> {}) && (!std::is_same<T, bool> {})
             && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    void Fit(sf::Sprite & s, const T WIDTH_LIMIT, const T HEIGHT_LIMIT, const Scale_t SCALE = 1.0f)
    {
        Fit(s, sf::Vector2<T>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(
        const T1 WIDTH_ORIG, const T1 HEIGHT_ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT)
    {
        return ScaleThatFits(
            sf::Vector2<T1>(WIDTH_ORIG, HEIGHT_ORIG), sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        /*
        const sf::Vector2f CENTER_V{ CenterOf(ORIG) };
        const sf::Vector2f NEW_SIZE_V{ FitCopy(Size(ORIG), LIMIT, SCALE)
        }; const sf::Vector2f NEW_POS_V{ CENTER_V - (NEW_SIZE_V * 0.5f) };
        return { NEW_POS_V, NEW_SIZE_V };
        */

        return sf::Rect<T1>(sf::FloatRect(
            (sf::Vector2f(CenterOf(ORIG))
             - (FitCopy(Size(sf::FloatRect(ORIG)), LIMIT, SCALE) * 0.5f)),
            FitCopy(Size(sf::FloatRect(ORIG)), LIMIT, SCALE)));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {}) && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    constexpr const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = 1.0f)
    {
        FitAndReCenterCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // rescales orig to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void FitAndReCenter(
        sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        orig = FitAndReCenterCopy(orig, LIMIT, SCALE);
    }

    // rescales orig to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T2> {} || std::is_floating_point<T2> {})
             && (!std::is_same<T2, bool> {}) && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    constexpr void FitAndReCenter(
        sf::Rect<T1> & orig,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = 1.0f)
    {
        FitAndReCenter(orig, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to the maximum size that fits within the size of LIMIT and is then
    // rescaled to SCALE and then recentered, if either LIMIT is zero then that dimmension is
    // ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        /*
        const sf::Vector2f ORIG_CENTER_V{ CenterOf(ORIG) };
        const sf::Vector2f NEW_SIZE_V{ FitCopy(Size(ORIG), Size(LIMIT), SCALE) };
        const sf::Vector2f NEW_POS_V{ ORIG_CENTER_V - (NEW_SIZE_V * 0.5f) };
        return { NEW_POS_V, NEW_SIZE_V };
        */

        return sf::Rect<T1>(sf::FloatRect(
            (sf::Vector2f(CenterOf(ORIG))
             - (FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE) * 0.5f)),
            FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE)));
    }

    // rescales orig to the maximum size that fits within the size of LIMIT and is then rescaled to
    // SCALE and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        FitAndReCenter(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        orig = FitAndReCenterCopy(orig, LIMIT, SCALE);
    }

    // rescales s (global) to the maximum size that fits within LIMIT and is then rescaled to
    // SCALE and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void FitAndReCenter(sf::Sprite & s, const sf::Vector2<T> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        auto const ORIG_CENTER_LEFT { CenterOf(s).x };
        auto const ORIG_CENTER_TOP { CenterOf(s).y };

        auto const NEW_SCALE { ScaleThatFits(Size(s), LIMIT) * SCALE };
        s.setScale(NEW_SCALE, NEW_SCALE);
        s.setPosition(sf::Vector2f(ORIG_CENTER_LEFT, ORIG_CENTER_TOP) - (Size(s, 0.5f)));
    }

    // rescales s (global) to the maximum size that fits within LIMIT and is then rescaled to
    // SCALE and then recentered, if either LIMIT is zero then that dimmension is ignored
    template <
        typename T,
        typename Scale_t = float,
        typename std::enable_if<
            ((std::is_integral<T> {} || std::is_floating_point<T> {}) && (!std::is_same<T, bool> {})
             && std::is_floating_point<Scale_t> {}),
            int>::type
        = 0>
    void FitAndReCenter(
        sf::Sprite & s, const T WIDTH_LIMIT, const T HEIGHT_LIMIT, const Scale_t SCALE = 1.0f)
    {
        FitAndReCenter(s, sf::Vector2<T>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // rescales s (global) to the maximum size that fits within the size of LIMIT and is then
    // rescaled to SCALE and then recentered, if either LIMIT is zero then that dimmension is
    // ignored
    template <
        typename T,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void FitAndReCenter(sf::Sprite & s, const sf::Rect<T> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        const float ORIG_CENTER_LEFT { CenterOf(s).x };
        const float ORIG_CENTER_TOP { CenterOf(s).y };

        const float NEW_SCALE { ScaleThatFits(Size(s), Size(sf::FloatRect(LIMIT)))
                                * static_cast<float>(SCALE) };

        s.setScale(NEW_SCALE, NEW_SCALE);
        s.setPosition(sf::Vector2f(ORIG_CENTER_LEFT, ORIG_CENTER_TOP) - (Size(s, 0.5f)));
    }

    // rescales s (global) to the maximum size that fits within the size of LIMIT (global) and is
    // then rescaled to SCALE and then recentered, if either LIMIT is zero then that dimmension is
    // ignored
    template <
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    void FitAndReCenter(sf::Sprite & s, const sf::Sprite & LIMIT, const Scale_t SCALE = 1.0f)
    {
        FitAndReCenter(s, LIMIT.getGlobalBounds(), SCALE);
    }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_FITTING_HPP_INCLUDED
