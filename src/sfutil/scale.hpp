// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
//
// scale.hpp
//
#include "misc/real.hpp"
#include "sfutil/center.hpp"
#include "sfutil/common.hpp"

namespace heroespath
{
namespace sfutil
{

    // same as VectorMult(V, SCALE_V), or put another way (A.x * B.x), (A.y * B.y)
    template <typename T1, typename Scale_t>
    const sf::Vector2<T1>
        ScaleCopy(const sf::Vector2<T1> & V, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return VectorMult(V, SCALE_V);
    }

    template <typename T1, typename Scale_t>
    const sf::Rect<T1>
        ScaleCopy(const sf::Rect<T1> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return sf::Rect<T1>(sf::FloatRect(
            static_cast<float>(R.left),
            static_cast<float>(R.top),
            (static_cast<float>(R.width) * static_cast<float>(SCALE_V.x)),
            (static_cast<float>(R.height) * static_cast<float>(SCALE_V.y))));
    }

    template <typename T1, typename Scale_t>
    const sf::Rect<T1> ScaleCopy(const sf::Rect<T1> & R, const Scale_t & SCALE) noexcept
    {
        return sf::Rect<T1>(sf::FloatRect(
            static_cast<float>(R.left),
            static_cast<float>(R.top),
            (static_cast<float>(R.width) * static_cast<float>(SCALE)),
            (static_cast<float>(R.height) * static_cast<float>(SCALE))));
    }

    template <typename T, typename Scale_t>
    void Scale(sf::Vector2<T> & v, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        v = VectorMult(v, SCALE_V);
    }

    template <typename T, typename Scale_t>
    void Scale(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        r.width = static_cast<T>(static_cast<float>(r.width) * static_cast<float>(SCALE_V.x));
        r.height = static_cast<T>(static_cast<float>(r.height) * static_cast<float>(SCALE_V.y));
    }

    template <typename T, typename Scale_t>
    void Scale(sf::Rect<T> & r, const Scale_t & SCALE) noexcept
    {
        r.width = static_cast<T>(static_cast<float>(r.width) * static_cast<float>(SCALE));
        r.height = static_cast<T>(static_cast<float>(r.height) * static_cast<float>(SCALE));
    }

    // returns a copy of R with size scaled by SCALE and then re-centered to the orig center of R
    template <typename T, typename Scale_t>
    const sf::Rect<T>
        ScaleAndReCenterCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return CenterToCopy(R, R, SCALE_V);
    }

    // returns a copy of R with size scaled by SCALE and then re-centered to the orig center of R
    template <typename T, typename Scale_t>
    const sf::Rect<T> ScaleAndReCenterCopy(const sf::Rect<T> & R, const Scale_t & SCALE) noexcept
    {
        return ScaleAndReCenterCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // scales the size of r by SCALE and then re-centers to the orig center of R
    template <typename T, typename Scale_t>
    void ScaleAndReCenter(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        r = ScaleAndReCenterCopy(r, SCALE_V);
    }

    // scales the size of r by SCALE and then re-centers to the orig center of R
    template <typename T, typename Scale_t>
    void ScaleAndReCenter(sf::Rect<T> & r, const Scale_t & SCALE) noexcept
    {
        r = ScaleAndReCenterCopy(r, SCALE);
    }

    // sets s's scale to SCALE_V and then re-centers
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void SetScaleAndReCenter(T & thing, const sf::Vector2f & SCALE_V) noexcept
    {
        const auto ORIG_CENTER { CenterOf(thing.getGlobalBounds()) };
        thing.setScale(SCALE_V);
        thing.setPosition((ORIG_CENTER - Half(Size(thing.getGlobalBounds()))) + thing.getOrigin());
    }

    // sets s's scale to SCALE and then re-centers
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void SetScaleAndReCenter(T & thing, const float SCALE) noexcept
    {
        SetScaleAndReCenter(thing, sf::Vector2f(SCALE, SCALE));
    }

    // multiplies s's scale by SCALE_V and then re-centers
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void ScaleAndReCenter(T & thing, const sf::Vector2f & SCALE_V) noexcept
    {
        const auto ORIG_CENTER { CenterOf(thing.getGlobalBounds()) };
        thing.setScale(VectorMult(thing.getScale(), SCALE_V));
        thing.setPosition((ORIG_CENTER - Half(Size(thing.getGlobalBounds()))) + thing.getOrigin());
    }

    // multiplies s's scale by SCALE and then re-centers
    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void ScaleAndReCenter(T & thing, const float SCALE) noexcept
    {
        ScaleAndReCenter(thing, SCALE);
    }

    // returns a copy of R with the min dimension set equal to the max
    template <typename T>
    const sf::Rect<T> GrowToSquareCopy(const sf::Rect<T> & R) noexcept
    {
        const auto MAX_DIMM = misc::Max(R.width, R.height);
        return sf::Rect<T>(R.left, R.top, MAX_DIMM, MAX_DIMM);
    }

    // returns a copy of R with the min dimension set equal to the max and then scaled to SCALE
    template <typename T, typename Scale_t>
    const sf::Rect<T>
        GrowToSquareCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return ScaleCopy(GrowToSquareCopy(R), SCALE_V);
    }

    // returns a copy of R with the min dimension set equal to the max and then scaled to SCALE
    template <typename T, typename Scale_t>
    const sf::Rect<T> GrowToSquareCopy(const sf::Rect<T> & R, const Scale_t SCALE) noexcept
    {
        return ScaleCopy(GrowToSquareCopy(R), SCALE);
    }

    // changes R so the min dimension equals the max
    template <typename T>
    void GrowToSquare(sf::Rect<T> & r) noexcept
    {
        const auto MAX_DIMM = misc::Max(r.width, r.height);
        r.width = MAX_DIMM;
        r.height = MAX_DIMM;
    }

    // changes R so the min dimension equals the max and then scales to SCALE
    template <typename T, typename Scale_t>
    void GrowToSquare(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        r = GrowToSquareCopy(r, SCALE_V);
    }

    // changes R so the min dimension equals the max
    template <typename T, typename Scale_t>
    void GrowToSquare(sf::Rect<T> & r, const Scale_t SCALE) noexcept
    {
        r = GrowToSquareCopy(r, SCALE);
    }

    // returns a copy of R with the max dimension set equal to the min
    template <typename T>
    const sf::Rect<T> ShrinkToSquareCopy(const sf::Rect<T> & R) noexcept
    {
        const auto MIN_DIMM = misc::Min(R.width, R.height);
        return sf::Rect<T>(R.left, R.top, MIN_DIMM, MIN_DIMM);
    }

    // returns a copy of R with the max dimension set equal to the min and then scaled to SCALE_V
    template <typename T, typename Scale_t>
    const sf::Rect<T>
        ShrinkToSquareCopy(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return ScaleCopy(ShrinkToSquareCopy(R), SCALE_V);
    }

    // returns a copy of R with the max dimension set equal to the min and then scaled to SCALE
    template <typename T, typename Scale_t>
    const sf::Rect<T> ShrinkToSquareCopy(const sf::Rect<T> & R, const Scale_t SCALE) noexcept
    {
        return ShrinkToSquareCopy(R, sf::Vector2<Scale_t>(SCALE, SCALE));
    }

    // changes r so that the max dimension set equal to the min
    template <typename T>
    void ShrinkToSquare(sf::Rect<T> & r) noexcept
    {
        const auto MIN_DIMM = misc::Min(r.width, r.height);
        r.width = MIN_DIMM;
        r.height = MIN_DIMM;
    }

    // changes r so that the max dimension set equal to the min and then scales to SCALE_V
    template <typename T, typename Scale_t>
    void ShrinkToSquare(sf::Rect<T> & r, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        r = ShrinkToSquareCopy(r, SCALE_V);
    }

    // changes r so that the max dimension set equal to the min and then scales to SCALE_V
    template <typename T, typename Scale_t>
    void ShrinkToSquare(sf::Rect<T> & r, const Scale_t SCALE) noexcept
    {
        r = ShrinkToSquareCopy(r, SCALE);
    }

    // returns a copy of R with the min dimension set equal to the max and then re-centered
    template <typename T>
    const sf::Rect<T> GrowToSquareAndReCenterCopy(const sf::Rect<T> & R) noexcept
    {
        return CenterTo(GrowToSquareCopy(R), R);
    }

    // returns a copy of R with the min dimension set equal to the maxand then scaled to SCALE_V and
    // then re-centered
    template <typename T, typename Scale_t>
    const sf::Rect<T> GrowToSquareAndReCenterCopy(
        const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return CenterTo(GrowToSquareCopy(R, SCALE_V), R);
    }

    // returns a copy of R with the min dimension set equal to the max and then scaled to SCALE_V
    // and then re-centered
    template <typename T, typename Scale_t>
    const sf::Rect<T>
        GrowToSquareAndReCenterCopy(const sf::Rect<T> & R, const Scale_t SCALE) noexcept
    {
        return CenterTo(GrowToSquareCopy(R, SCALE), R);
    }

    // returns a copy of R with the max dimension set equal to the min and then re-centered
    template <typename T>
    const sf::Rect<T> ShrinkToSquareAndReCenterCopy(const sf::Rect<T> & R) noexcept
    {
        return CenterToCopy(ShrinkToSquareCopy(R), R);
    }

    // returns a copy of R with the max dimension set equal to the min and then scaled to SCALE_V
    // and then re-centered
    template <typename T, typename Scale_t>
    const sf::Rect<T> ShrinkToSquareAndReCenterCopy(
        const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return CenterToCopy(ShrinkToSquareCopy(R, SCALE_V), R);
    }

    // returns a copy of R with the max dimension set equal to the min and then scaled to SCALE_V
    // and then re-centered
    template <typename T, typename Scale_t>
    const sf::Rect<T>
        ShrinkToSquareAndReCenterCopy(const sf::Rect<T> & R, const Scale_t SCALE) noexcept
    {
        return CenterToCopy(ShrinkToSquareCopy(R, SCALE), R);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_SIZE_AND_SCALE_HPP_INCLUDED
