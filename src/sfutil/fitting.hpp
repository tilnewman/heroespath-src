// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_FITTING_HPP_INCLUDED
#define HEROESPATH_SFUTIL_FITTING_HPP_INCLUDED
//
// fitting.hpp
//
#include "misc/real.hpp"
#include "sfutil/center.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfutil
{

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either ORIG or LIMIT is <= zero then that dimension is ignored during fitting, if both
    // LIMITs are <= zero then fitting is skipped but SCALE is still applied
    template <typename T, typename = std::enable_if_t<std::is_same<T, float>::value>>
    constexpr const sf::Vector2<T>
        FitCopy(const sf::Vector2<T> & ORIG, const sf::Vector2<T> & LIMIT, const T SCALE = 1.0f)
    {
        // can assume T = float

        float scaleToUse { 1.0f };

        auto scaleBasedOnWidth = [&]() constexpr->float { return (LIMIT.x / ORIG.x); };
        auto scaleBasedOnHeight = [&]() constexpr->float { return (LIMIT.y / ORIG.y); };

        const auto IS_HORIZ_LIMIT_ZERO_OR_LESS { misc::IsRealZeroOrLess(LIMIT.x) };
        const auto IS_VERT_LIMIT_ZERO_OR_LESS { misc::IsRealZeroOrLess(LIMIT.y) };

        if (IS_HORIZ_LIMIT_ZERO_OR_LESS && IS_VERT_LIMIT_ZERO_OR_LESS)
        {
            scaleToUse = 1.0f;
        }
        else if (IS_HORIZ_LIMIT_ZERO_OR_LESS)
        {
            if (misc::IsRealZeroOrLess(ORIG.y) == false)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }
        else if (IS_VERT_LIMIT_ZERO_OR_LESS)
        {
            if (misc::IsRealZeroOrLess(ORIG.x) == false)
            {
                scaleToUse = scaleBasedOnWidth();
            }
        }
        else if (misc::IsRealZeroOrLess(ORIG.x))
        {
            if (misc::IsRealZeroOrLess(ORIG.y) == false)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }
        else if (misc::IsRealZeroOrLess(ORIG.y))
        {
            if (misc::IsRealZeroOrLess(ORIG.x) == false)
            {
                scaleToUse = scaleBasedOnWidth();
            }
        }
        else
        {
            // at this point we know none of the ORIG or LIMIT values are zero or less

            scaleToUse = scaleBasedOnWidth();

            if ((scaleToUse * ORIG.y) > LIMIT.y)
            {
                scaleToUse = scaleBasedOnHeight();
            }
        }

        return (ORIG * (scaleToUse * SCALE));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT or ORIG is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // if either LIMIT or ORIG is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // if either LIMIT or ORIG is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // rescaled to SCALE, if either LIMIT is <= zero then that dimension is ignored, if both LIMITs
    // are <= zero then fitting is skipped
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
    // if either LIMIT or orig is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // applies SCALE, if either LIMIT or orig is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
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

    // returns a scale that would make ORIG the maximum size that fits within LIMIT, if either LIMIT
    // or orig is <= zero then that dimension is ignored, if both LIMITs are <= zero
    // then 1.0f is returned
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT)
    {
        if (misc::IsRealZeroOrLess(ORIG.x) == false)
        {
            return (FitCopy(sf::Vector2f(ORIG), LIMIT).x / static_cast<float>(ORIG.x));
        }
        else if (misc::IsRealZeroOrLess(ORIG.y) == false)
        {
            return (FitCopy(sf::Vector2f(ORIG), LIMIT).y / static_cast<float>(ORIG.y));
        }
        else
        {
            return 1.0f;
        }
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE,
    // if either LIMIT or ORIG is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // if either LIMIT or ORIG is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // rescaled to SCALE, if either LIMIT or ORIG is <= zero then that dimension is ignored, if
    // both LIMITs are <= zero then fitting is skipped
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

    // returns a scale that would make ORIG the maximum size that fits within LIMIT, if both LIMITs
    // are <= zero then fitting is skipped
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

    // returns a scale that would make ORIG the maximum size that fits within the size of LIMIT, if
    // both LIMITs are <= zero then fitting is skipped
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT)
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT, if both LIMITs
    // are <= zero then fitting is skipped
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT)
    {
        return ScaleThatFits(Size(ORIG), LIMIT);
    }

    // returns a scale that would make ORIG the maximum size that fits within LIMIT, if both LIMITs
    // are <= zero then fitting is skipped
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

    // returns a scale that would make ORIG the maximum size that fits within the size of LIMIT, if
    // both LIMITs are <= zero then fitting is skipped
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT)
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // rescales orig to the maximum size that fits within LIMIT and then applies SCALE,
    // if either LIMIT or orig is <= zero then that dimension is ignored, if both LIMITs are <=
    // zero or less then fitting is skipped
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
    // applies SCALE, if either LIMIT or orig is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void Fit(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        Fit(orig, Size(LIMIT), SCALE);
    }

    // rescales s (local) to the maximum size that fits within LIMIT, if either LIMIT or Size(s) is
    // <= zero then that dimension is ignored, if both LIMITs are <= zero then fitting is skipped
    void Fit(sf::Sprite & s, const sf::Vector2f & LIMIT);

    // rescales s (local) to the maximum size that fits within LIMITs, if either LIMIT or Size(s) is
    // zero then that dimension is ignore, if both LIMITs are <= zero then fitting is skipped
    void Fit(sf::Sprite & s, const float WIDTH_LIMIT, const float HEIGHT_LIMIT);

    // returns a scale that would make ORIG the maximum size that fits within LIMIT, if both LIMITs
    // are <= zero then fitting is skipped
    template <typename T1, typename T2>
    constexpr float ScaleThatFits(
        const T1 WIDTH_ORIG, const T1 HEIGHT_ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT)
    {
        return ScaleThatFits(
            sf::Vector2<T1>(WIDTH_ORIG, HEIGHT_ORIG), sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered to ORIG, if either LIMIT or ORIG is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        /*
         * This version of the code cannot be used since this function is constexpr.
         * Keeping this here for reference.
         *
        const sf::Vector2f CENTER_ORIG_V{ CenterOf(ORIG) };
        const sf::Vector2f NEW_SIZE_V{ FitCopy(Size(ORIG), LIMIT, SCALE) };
        const sf::Vector2f NEW_POS_V{ CENTER_ORIG_V - (NEW_SIZE_V * 0.5f) };
        return { NEW_POS_V, NEW_SIZE_V };
        */

        return sf::Rect<T1>(sf::FloatRect(
            (sf::Vector2f(CenterOf(ORIG))
             - (FitCopy(Size(sf::FloatRect(ORIG)), LIMIT, SCALE) * 0.5f)),
            FitCopy(Size(sf::FloatRect(ORIG)), LIMIT, SCALE)));
    }

    // returns ORIG scaled to the maximum size that fits within LIMIT and is then rescaled to SCALE
    // and then recentered to ORIG, if either LIMIT or ORIG is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
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
    // and then recentered to ORIG, if either LIMIT or orig is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
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
    // and then recentered to orig, if either LIMIT or orig is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
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
    // rescaled to SCALE and then recentered to ORIG, if either LIMIT or ORIG is <= zero then that
    // dimension is ignored, if both LIMITs are <= zero then fitting is skipped
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        /*
         * This version of the code cannot be used since this function is constexpr.
         * Keeping this here for reference.
         *
        const sf::Vector2f CENTER_ORIG_V{ CenterOf(ORIG) };
        const sf::Vector2f NEW_SIZE_V{ FitCopy(Size(ORIG), Size(LIMIT), SCALE) };
        const sf::Vector2f NEW_POS_V{ CENTER_ORIG_V - (NEW_SIZE_V * 0.5f) };
        return { NEW_POS_V, NEW_SIZE_V };
        */

        return sf::Rect<T1>(sf::FloatRect(
            (sf::Vector2f(CenterOf(ORIG))
             - (FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE) * 0.5f)),
            FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE)));
    }

    // returns ORIG scaled to the maximum size that fits within the size of LIMIT and is then
    // rescaled to SCALE and then centered to LIMIT, if either LIMIT or ORIG is <= zero then that
    // dimension is ignored, if both LIMITs are <= zero then fitting is skipped
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr const sf::Rect<T1> FitAndCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        /*
         * This version of the code cannot be used since this function is constexpr.
         * Keeping this here for reference.
         *
        const sf::Vector2f CENTER_LIMIT_V{ CenterOf(LIMIT) };
        const sf::Vector2f NEW_SIZE_V{ FitCopy(Size(ORIG), Size(LIMIT), SCALE) };
        const sf::Vector2f NEW_POS_V{ CENTER_LIMIT_V - (NEW_SIZE_V * 0.5f) };
        return { NEW_POS_V, NEW_SIZE_V };
        */

        return sf::Rect<T1>(sf::FloatRect(
            (sf::Vector2f(CenterOf(LIMIT))
             - (FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE) * 0.5f)),
            FitCopy(Size(sf::FloatRect(ORIG)), Size(LIMIT), SCALE)));
    }

    // rescales orig to the maximum size that fits within the size of LIMIT and is then rescaled to
    // SCALE and then recentered to ORIG, if either LIMIT or orig is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
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

    // rescales orig to the maximum size that fits within the size of LIMIT and is then rescaled to
    // SCALE and then centered to LIMIT, if either LIMIT or orig is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
    template <
        typename T1,
        typename T2,
        typename Scale_t = float,
        typename = std::enable_if_t<std::is_floating_point<Scale_t>::value>>
    constexpr void
        FitAndCenterTo(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE = 1.0f)
    {
        orig = FitAndCenterCopy(orig, LIMIT, SCALE);
    }

    // rescales s (local) to the maximum size that fits within LIMIT and then recenters to s's
    // original center, if either LIMIT or s is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    void FitAndReCenter(sf::Sprite & s, const sf::Vector2f & LIMIT);

    // rescales s (local) to the maximum size that fits within LIMIT and then recentered to s's
    // original center, if either LIMIT or s is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    void FitAndReCenter(sf::Sprite & s, const float WIDTH_LIMIT, const float HEIGHT_LIMIT);

    // rescales s (local) to the maximum size that fits within the size of LIMIT and then recenters
    // to s's original center, if either LIMIT or s is <= zero then that dimension is ignored, if
    // both LIMITs are <= zero then fitting is skipped
    void FitAndReCenter(sf::Sprite & s, const sf::FloatRect & LIMIT);

    // rescales s (local) to the maximum size that fits within the size (global) of LIMIT and then
    // recenters to s's original center, if either dimension of LIMIT is <= zero then that
    // dimension is ignored, if both LIMITs are <= zero then fitting is skipped
    void FitAndReCenter(sf::Sprite & s, const sf::Sprite & LIMIT);

    // rescales s (local) to the maximum size that fits within the size of LIMIT and then centers s
    // in LIMIT, if either LIMIT or s is <= zero then that dimension is ignored, if both LIMITs are
    // <= zero then fitting is skipped
    void FitAndCenterTo(sf::Sprite & s, const sf::FloatRect & LIMIT);

    // rescales s (local) to the maximum size that fits within the size of LIMIT (global) and then
    // centers s in LIMIT, if either LIMIT or s is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    void FitAndCenterTo(sf::Sprite & s, const sf::Sprite & LIMIT);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_FITTING_HPP_INCLUDED
