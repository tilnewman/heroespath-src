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
#include "sfutil/common.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace heroespath
{
namespace sfutil
{

    // ScaleThatFits()
    //  Returns a float that when multiplied to ORIG will make it fit LIMIT, or put another way,
    //  that will make the largest dimmension of ORIG match that dimmension of LIMIT.

    // returns a scale that would make ORIG fit within LIMIT, if any ORIG OR LIMIT are <= zero then
    // that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT) noexcept
    {
        float scaleThatFits { 1.0f };

        auto scaleBasedOnWidth
            = [&]() { return (static_cast<float>(LIMIT.x) / static_cast<float>(ORIG.x)); };

        auto scaleBasedOnHeight
            = [&]() { return (static_cast<float>(LIMIT.y) / static_cast<float>(ORIG.y)); };

        const bool ORIG_X_INVALID = misc::IsRealZeroOrLess(ORIG.x);
        const bool ORIG_Y_INVALID = misc::IsRealZeroOrLess(ORIG.y);
        const bool LIMIT_X_INVALID = misc::IsRealZeroOrLess(LIMIT.x);
        const bool LIMIT_Y_INVALID = misc::IsRealZeroOrLess(LIMIT.y);

        if ((ORIG_X_INVALID && ORIG_Y_INVALID) || (LIMIT_X_INVALID && LIMIT_Y_INVALID)
            || (LIMIT_X_INVALID && ORIG_Y_INVALID) || (LIMIT_Y_INVALID && ORIG_X_INVALID))
        {
            scaleThatFits = 1.0f;
        }
        else if (ORIG_Y_INVALID || LIMIT_Y_INVALID)
        {
            scaleThatFits = scaleBasedOnWidth();
        }
        else if (ORIG_X_INVALID || LIMIT_X_INVALID)
        {
            scaleThatFits = scaleBasedOnHeight();
        }
        else
        {
            // at this point we know all ORIG and LIMIT are > 0
            scaleThatFits = scaleBasedOnWidth();

            if ((scaleThatFits * static_cast<float>(ORIG.y)) > static_cast<float>(LIMIT.y))
            {
                scaleThatFits = scaleBasedOnHeight();
            }
        }

        return scaleThatFits;
    }

    // returns a scale that would make ORIG fit within LIMIT, if any ORIG OR LIMIT are <= zero
    // then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(
        const sf::Vector2<T1> & ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT) noexcept
    {
        return ScaleThatFits(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns a scale that would make ORIG fit within LIMIT, if any ORIG OR LIMIT are <= zero
    // then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(
        const T1 WIDTH_ORIG,
        const T1 HEIGHT_ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT) noexcept
    {
        return ScaleThatFits(
            sf::Vector2<T1>(WIDTH_ORIG, HEIGHT_ORIG), sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns a scale that would make ORIG fit within the size of LIMIT, if any ORIG OR LIMIT
    // are
    // <= zero then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT) noexcept
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // returns a scale that would make the size of ORIG fit within LIMIT, if any ORIG OR
    // LIMIT are <= zero then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT) noexcept
    {
        return ScaleThatFits(Size(ORIG), LIMIT);
    }

    // returns a scale that would make the size of ORIG fit within LIMIT, if any ORIG OR LIMIT
    // are
    // <= zero then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(
        const sf::Rect<T1> & ORIG, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT) noexcept
    {
        return ScaleThatFits(Size(ORIG), sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT));
    }

    // returns a scale that would make the size of ORIG fit within the size of LIMIT, if any
    // ORIG OR LIMIT are <= zero then that dimension is ignored
    template <typename T1, typename T2>
    float ScaleThatFits(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT) noexcept
    {
        return ScaleThatFits(ORIG, Size(LIMIT));
    }

    // Fit()
    // Rescale the size of ORIG so that its largest dimmension matches that same dimmension of
    // LIMIT

    // returns ORIG scaled to fit LIMIT, if any ORIG or LIMIT is <= 0 then that dimension is not
    // fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Vector2<T1>
        FitCopy(const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT) noexcept
    {
        return sf::Vector2<T1>(sf::Vector2f(ORIG) * ScaleThatFits(ORIG, LIMIT));
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE, if any ORIG or LIMIT is <= 0
    // then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        return sf::Vector2<T1>(
            FitCopy(sf::Vector2f(ORIG), sf::Vector2f(LIMIT)) * static_cast<float>(SCALE));
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE, if any ORIG or LIMIT is <= 0
    // then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t = float>
    const sf::Vector2<T1> FitCopy(
        const T1 WIDTH_ORIG,
        const T1 HEIGHT_ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = Scale_t(1)) noexcept
    {
        return FitCopy(
            sf::Vector2<T1>(WIDTH_ORIG, HEIGHT_ORIG),
            sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT),
            SCALE);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE, if any ORIG or LIMIT is <= 0
    // then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t = float>
    const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = Scale_t(1)) noexcept
    {
        return FitCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Vector2<T1> FitCopy(const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT) noexcept
    {
        return FitCopy(ORIG, Size(LIMIT));
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Vector2<T1> FitCopy(
        const sf::Vector2<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        return FitCopy(ORIG, Size(LIMIT), SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Rect<T1> FitCopy(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT) noexcept
    {
        return sf::Rect<T1>(Position(ORIG), FitCopy(Size(ORIG), LIMIT));
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1>
        FitCopy(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE)
    {
        return sf::Rect<T1>(Position(ORIG), FitCopy(Size(ORIG), LIMIT, SCALE));
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE, if any ORIG or LIMIT is <= 0
    // then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t = float>
    const sf::Rect<T1> FitCopy(
        const sf::Rect<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = Scale_t(1)) noexcept
    {
        return FitCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Rect<T1> FitCopy(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT) noexcept
    {
        return FitCopy(ORIG, Size(LIMIT));
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1>
        FitCopy(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        return FitCopy(ORIG, Size(LIMIT), SCALE);
    }

    // returns ORIG scaled to fit LIMIT, if any ORIG or LIMIT is <= 0 then that dimension is not
    // fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void Fit(sf::Vector2<T1> & orig, const sf::Vector2<T2> & LIMIT) noexcept
    {
        orig = FitCopy(orig, LIMIT);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE, if any ORIG or LIMIT is <= 0
    // then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void Fit(sf::Vector2<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        orig = FitCopy(orig, LIMIT, SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void Fit(sf::Vector2<T1> & orig, const sf::Rect<T2> & LIMIT) noexcept
    {
        Fit(orig, Size(LIMIT));
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void Fit(sf::Vector2<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        Fit(orig, Size(LIMIT), SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void Fit(sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT) noexcept
    {
        orig = FitCopy(orig, LIMIT);
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void Fit(sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        orig = FitCopy(orig, LIMIT, SCALE);
    }

    // returns ORIG scaled to fit the size of LIMIT, if any ORIG or LIMIT is <= 0 then that
    // dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void Fit(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT) noexcept
    {
        Fit(orig, Size(LIMIT));
    }

    // returns ORIG scaled to fit the size of LIMIT and then rescaled to SCALE, if any ORIG or
    // LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void Fit(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        Fit(orig, Size(LIMIT), SCALE);
    }

    // rescales s (local) to fit LIMIT, if either size of s or LIMIT is <= 0 it is ignored
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void Fit(T & thing, const sf::Vector2f & LIMIT) noexcept
    {
        thing.setScale(thing.getScale() * ScaleThatFits(Size(thing.getGlobalBounds()), LIMIT));
    }

    // rescales s (local) to fit LIMIT, if either size of s or LIMIT is <= 0 it is ignored
    template <typename T1, typename T2, typename = std::enable_if_t<has_getglobalbounds_v<T1>>>
    void Fit(T1 & thing, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT)
    {
        Fit(thing, sf::Vector2f(sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT)));
    }

    // FitAndRecenter()
    // Rescale the size of ORIG so that its largest dimmension matches that same dimmension of
    // LIMIT and then re-center to the original center of ORIG.

    // returns ORIG scaled to fit LIMIT and then re-centered to the original center of ORIG, if
    // any ORIG or LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Rect<T1>
        FitAndReCenterCopy(const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT) noexcept
    {
        return CenterToCopy(FitCopy(ORIG, LIMIT), ORIG);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        return CenterToCopy(FitCopy(ORIG, LIMIT, SCALE), ORIG);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t = float>
    const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = Scale_t(1)) noexcept
    {
        return FitAndReCenterCopy(ORIG, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to fit LIMIT and then re-centered to the original center of ORIG, if
    // any ORIG or LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    const sf::Rect<T1>
        FitAndReCenterCopy(const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT) noexcept
    {
        return FitAndReCenterCopy(ORIG, Size(LIMIT));
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    const sf::Rect<T1> FitAndReCenterCopy(
        const sf::Rect<T1> & ORIG, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        return FitAndReCenterCopy(ORIG, Size(LIMIT), SCALE);
    }

    // returns ORIG scaled to fit LIMIT and then re-centered to the original center of ORIG, if
    // any ORIG or LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void FitAndReCenter(sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT) noexcept
    {
        orig = FitAndReCenterCopy(orig, LIMIT);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void FitAndReCenter(
        sf::Rect<T1> & orig, const sf::Vector2<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        orig = FitAndReCenterCopy(orig, LIMIT, SCALE);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t = float>
    void FitAndReCenter(
        sf::Rect<T1> & orig,
        const T2 WIDTH_LIMIT,
        const T2 HEIGHT_LIMIT,
        const Scale_t SCALE = Scale_t(1)) noexcept
    {
        FitAndReCenter(orig, sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT), SCALE);
    }

    // returns ORIG scaled to fit LIMIT and then re-centered to the original center of ORIG, if
    // any ORIG or LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void FitAndReCenter(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT) noexcept
    {
        FitAndReCenter(orig, Size(LIMIT));
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // original center of ORIG, if any ORIG or LIMIT is <= 0 then that dimension is not fit but
    // SCALE is ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void FitAndReCenter(
        sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        FitAndReCenter(orig, Size(LIMIT), SCALE);
    }

    // rescales s (local) to the maximum size that fits within LIMIT and then recenters to s's
    // original center, if either LIMIT or s is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void FitAndReCenter(T & thing, const sf::Vector2f & LIMIT_V) noexcept
    {
        const auto ORIG_CENTER_V { CenterOf(thing.getGlobalBounds()) };
        Fit(thing, LIMIT_V);

        thing.setPosition(
            (ORIG_CENTER_V - Half(Size(thing.getGlobalBounds()))) + thing.getOrigin());
    }

    // rescales s (local) to the maximum size that fits within LIMIT and then recenters to s's
    // original center, if either LIMIT or s is <= zero then that dimension is ignored, if both
    // LIMITs are <= zero then fitting is skipped
    template <typename T1, typename T2, typename = std::enable_if_t<has_getglobalbounds_v<T1>>>
    void FitAndReCenter(T1 & thing, const T2 WIDTH_LIMIT, const T2 HEIGHT_LIMIT) noexcept
    {
        FitAndReCenter(thing, sf::Vector2f(sf::Vector2<T2>(WIDTH_LIMIT, HEIGHT_LIMIT)));
    }

    // rescales s (local) to the maximum size that fits within the size of LIMIT and then
    // recenters to s's original center, if either LIMIT or s is <= zero then that dimension is
    // ignored, if both LIMITs are <= zero then fitting is skipped
    template <typename T1, typename T2, typename = std::enable_if_t<has_getglobalbounds_v<T1>>>
    void FitAndReCenter(T1 & thing, const sf::Rect<T2> & LIMIT)
    {
        FitAndReCenter(thing, sf::Vector2f(Size(LIMIT)));
    }

    // rescales s (local) to the maximum size that fits within the size (global) of LIMIT and
    // then recenters to s's original center, if either dimension of LIMIT is <= zero then that
    // dimension is ignored, if both LIMITs are <= zero then fitting is skipped
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void FitAndReCenter(T & thing, const T & LIMIT) noexcept
    {
        FitAndReCenter(thing, Size(LIMIT.getGlobalBounds()));
    }

    // FitAndReCenterTo()

    // returns ORIG scaled to fit LIMIT and then re-centered to the center of LIMIT, if any ORIG
    // or LIMIT is <= 0 then that dimension is not fit but SCALE is ALWAYS applied
    template <typename T1, typename T2>
    void FitAndCenterTo(sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT) noexcept
    {
        Fit(orig, LIMIT);
        CenterTo(orig, LIMIT);
    }

    // returns ORIG scaled to fit LIMIT and then rescaled to SCALE and then re-centered to the
    // center of LIMIT, if any ORIG or LIMIT is <= 0 then that dimension is not fit but SCALE is
    // ALWAYS applied
    template <typename T1, typename T2, typename Scale_t>
    void FitAndCenterTo(
        sf::Rect<T1> & orig, const sf::Rect<T2> & LIMIT, const Scale_t SCALE) noexcept
    {
        Fit(orig, LIMIT, SCALE);
        CenterTo(orig, LIMIT);
    }

    // rescales s (local) to the maximum size that fits within the size of LIMIT and then
    // centers s in LIMIT, if either LIMIT or s is <= zero then that dimension is ignored, if
    // both LIMITs are
    // <= zero then fitting is skipped
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void FitAndCenterTo(T & thing, const sf::FloatRect & LIMIT_RECT) noexcept
    {
        Fit(thing, Size(LIMIT_RECT));

        thing.setPosition(
            (CenterOf(LIMIT_RECT) - Half(Size(thing.getGlobalBounds()))) + thing.getOrigin());
    }

    // rescales s (local) to the maximum size that fits within the size of LIMIT (global) and
    // then centers s in LIMIT, if either LIMIT or s is <= zero then that dimension is ignored,
    // if both LIMITs are <= zero then fitting is skipped
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void FitAndCenterTo(T & thing, const T & LIMIT) noexcept
    {
        FitAndCenterTo(thing, LIMIT.getGlobalBounds());
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_FITTING_HPP_INCLUDED
