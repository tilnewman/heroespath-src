// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_COMMON_HPP_INCLUDED
#define HEROESPATH_SFUTIL_COMMON_HPP_INCLUDED
//
// common.hpp
//
#include "gui/orientation-enum.hpp"
#include "misc/real.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
class Image;
class RectangleShape;
class Texture;
class Sprite;
} // namespace sf

namespace heroespath
{
namespace gui
{
    class Text;
}
namespace sfutil
{

    // The sfutil functions do math as/in float but returns the type of the first param unless
    // the function is returning a scale which is always a float.

    // get/setScale
    template <typename T>
    struct is_sf_transformable : std::bool_constant<std::is_convertible_v<T *, sf::Transformable *>>
    {};

    template <typename... T>
    constexpr bool is_sf_transformable_v = std::conjunction_v<is_sf_transformable<T>...>;

    template <typename T>
    struct has_getpos
        : std::bool_constant<(is_sf_transformable_v<T> || misc::are_same_v<T, gui::Text>)>
    {};

    template <typename... T>
    constexpr bool has_getpos_v = std::conjunction_v<has_getpos<T>...>;

    template <typename T>
    struct has_getsize
        : std::bool_constant<misc::are_any_v<T, sf::RectangleShape, sf::Image, sf::Texture>>
    {};

    template <typename... T>
    constexpr bool has_getsize_v = std::conjunction_v<has_getsize<T>...>;

    template <typename T>
    struct has_getglobalbounds
        : std::bool_constant<(
              std::is_convertible_v<T *, sf::Shape *> || misc::are_any_v<T, sf::Sprite, gui::Text>)>
    {};

    template <typename... T>
    constexpr bool has_getglobalbounds_v = std::conjunction_v<has_getglobalbounds<T>...>;

    template <typename T>
    using vector2_type_picker_t
        = std::conditional_t<(has_getsize_v<T> || has_getpos_v<T>), sf::Vector2f, T>;

    template <typename T>
    using pos_type_picker_t = vector2_type_picker_t<T>;

    template <typename T>
    using size_type_picker_t = vector2_type_picker_t<T>;

    template <typename T>
    using rect_type_picker_t = std::conditional_t<has_getglobalbounds_v<T>, sf::FloatRect, T>;

    template <typename T>
    const typename std::enable_if_t<has_getglobalbounds_v<T>, sf::FloatRect>
        rect_extractor(const T & THING_WITH_GLOBAL_BOUNDS) noexcept
    {
        return THING_WITH_GLOBAL_BOUNDS.getGlobalBounds();
    }

    template <typename T>
    const typename std::enable_if_t<!has_getglobalbounds_v<T>, T>
        rect_extractor(const T & RECT) noexcept
    {
        return RECT;
    }

    template <typename T>
    const typename std::enable_if_t<has_getpos_v<T>, sf::Vector2f>
        pos_extractor(const T & THING_WITH_POS) noexcept
    {
        return THING_WITH_POS.getPosition();
    }

    template <typename T>
    const typename std::enable_if_t<!has_getpos_v<T>, T> pos_extractor(const T & POS_V) noexcept
    {
        return POS_V;
    }

    template <typename T>
    const typename std::enable_if_t<has_getsize_v<T>, sf::Vector2f>
        size_extractor(const T & THING_WITH_SIZE) noexcept
    {
        return THING_WITH_SIZE.getSize();
    }

    template <typename T>
    const typename std::enable_if_t<!has_getsize_v<T>, sf::Vector2f>
        size_extractor(const T & SIZE_V) noexcept
    {
        return SIZE_V;
    }

    // same as (A.x * B.x), (A.y * B.y)
    template <typename T1, typename T2>
    const sf::Vector2<T1> VectorMult(const sf::Vector2<T1> & A, const sf::Vector2<T2> & B) noexcept
    {
        return sf::Vector2<T1>(
            static_cast<T1>(static_cast<float>(A.x) * static_cast<float>(B.x)),
            static_cast<T1>(static_cast<float>(A.y) * static_cast<float>(B.y)));
    }

    template <typename T>
    const sf::Vector2<T> Half(const sf::Vector2<T> & V) noexcept
    {
        return sf::Vector2<T>((V.x / T(2)), (V.y / T(2)));
    }

    template <typename T>
    const T Left(const sf::Rect<T> & RECT) noexcept
    {
        return RECT.left;
    }

    // template <typename T>
    // std::enable_if_t<has_getpos_v<T>, float> Left(const T & X) noexcept
    //{
    //    return X.getPosition().x;
    //}

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Left(const T & X) noexcept
    {
        return X.getGlobalBounds().left;
    }

    template <typename T>
    const T Right(const sf::Rect<T> & RECT) noexcept
    {
        return (RECT.left + RECT.width);
    }

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Right(const T & X) noexcept
    {
        return Right(X.getGlobalBounds());
    }

    template <typename T>
    const T Top(const sf::Rect<T> & RECT) noexcept
    {
        return RECT.top;
    }

    // template <typename T>
    // std::enable_if_t<has_getpos_v<T>, float> Top(const T & X) noexcept
    //{
    //    return X.getPosition().y;
    //}

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Top(const T & X) noexcept
    {
        return X.getGlobalBounds().top;
    }

    template <typename T>
    const T Bottom(const sf::Rect<T> & RECT) noexcept
    {
        return (RECT.top + RECT.height);
    }

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Bottom(const T & X) noexcept
    {
        return Bottom(X.getGlobalBounds());
    }

    template <typename T>
    const sf::Vector2<T> TopLeft(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>(R.left, R.top);
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f> TopLeft(const T & X) noexcept
    {
        return TopLeft(X.getGlobalBounds());
    }

    template <typename T>
    const sf::Vector2<T> TopRight(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>((R.left + R.width), R.top);
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2<T>> TopRight(const T & X) noexcept
    {
        return TopRight(X.getGlobalBounds());
    }

    template <typename T>
    const sf::Vector2<T> BottomLeft(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>(R.left, (R.top + R.height));
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2<T>>
        BottomLeft(const T & X) noexcept
    {
        return BottomLeft(X.getGlobalBounds());
    }

    template <typename T>
    const sf::Vector2<T> BottomRight(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>((R.left + R.width), (R.top + R.height));
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2<T>>
        BottomRight(const T & X) noexcept
    {
        return BottomRight(X.getGlobalBounds());
    }

    template <typename T>
    const T Width(const sf::Rect<T> & RECT) noexcept
    {
        return RECT.width;
    }

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Width(const T & X) noexcept
    {
        return X.getGlobalBounds().width;
    }

    template <typename T>
    const T Height(const sf::Rect<T> & RECT) noexcept
    {
        return RECT.height;
    }

    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Height(const T & X) noexcept
    {
        return X.getGlobalBounds().height;
    }

    template <typename T>
    const sf::Vector2<T> Position(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>(R.left, R.top);
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f> Position(const T & X) noexcept
    {
        return sf::Vector2f(X.getGlobalBounds().left, X.getGlobalBounds().top);
    }

    template <typename T>
    const sf::Vector2<T> Size(const sf::Rect<T> & R) noexcept
    {
        return sf::Vector2<T>(R.width, R.height);
    }

    template <typename T>
    const std::enable_if_t<has_getsize_v<T>, sf::Vector2f> Size(const T & X) noexcept
    {
        return X.getSize();
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f> Size(const T & X) noexcept
    {
        return sf::Vector2f(X.getGlobalBounds().width, X.getGlobalBounds().height);
    }

    template <typename T, typename Scale_t>
    const sf::Vector2<T> Size(const sf::Rect<T> & R, const Scale_t SCALE) noexcept
    {
        return sf::Vector2<T>(
            static_cast<T>(static_cast<float>(R.width) * static_cast<float>(SCALE)),
            static_cast<T>(static_cast<float>(R.height) * static_cast<float>(SCALE)));
    }

    template <typename T, typename Scale_t>
    const std::enable_if_t<has_getsize_v<T>, sf::Vector2f>
        Size(const T & X, const Scale_t SCALE) noexcept
    {
        return (X.getSize() * static_cast<float>(SCALE));
    }

    template <typename T, typename Scale_t>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f>
        Size(const T & X, const Scale_t SCALE) noexcept
    {
        return Size(X.getGlobalBounds(), SCALE);
    }

    // returns the size of R * SCALE_V
    template <typename T, typename Scale_t>
    const sf::Vector2<T> Size(const sf::Rect<T> & R, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return sf::Vector2<T>(
            static_cast<T>(static_cast<float>(R.width) * static_cast<float>(SCALE_V.x)),
            static_cast<T>(static_cast<float>(R.height) * static_cast<float>(SCALE_V.y)));
    }

    template <typename T, typename Scale_t>
    const std::enable_if_t<has_getsize_v<T>, sf::Vector2f>
        Size(const T & X, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return VectorMult(X.getSize(), SCALE_V);
    }

    template <typename T, typename Scale_t>
    const std::enable_if_t<has_getglobalbounds_v<T>, sf::Vector2f>
        Size(const T & X, const sf::Vector2<Scale_t> & SCALE_V) noexcept
    {
        return Size(X.getGlobalBounds(), SCALE_V);
    }

    template <typename T1, typename T2>
    const sf::Rect<T1>
        SetPositionCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & POS_V) noexcept
    {
        return sf::Rect<T1>(sf::Vector2<T1>(POS_V), Size(R));
    }

    template <typename T1, typename T2>
    void SetPosition(sf::Rect<T1> & r, const sf::Vector2<T2> & POS_V) noexcept
    {
        r.left = static_cast<T1>(POS_V.x);
        r.top = static_cast<T1>(POS_V.y);
    }

    template <typename T1, typename T2>
    const sf::Rect<T1> SetSizeCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & SIZE_V) noexcept
    {
        return sf::Rect<T1>(Position(R), sf::Vector2<T1>(SIZE_V));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<misc::are_arithmetic_nobool_v<T2>, sf::Rect<T1>>
        SetSizeCopy(const sf::Rect<T1> & R, const T2 SIZE) noexcept
    {
        return sf::Rect<T1>(R.left, R.top, static_cast<T1>(SIZE), static_cast<T1>(SIZE));
    }

    template <typename T1, typename T2>
    void SetSize(sf::Rect<T1> & r, const sf::Vector2<T2> & SIZE_V) noexcept
    {
        r.width = static_cast<T1>(SIZE_V.x);
        r.height = static_cast<T1>(SIZE_V.y);
    }

    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void SetSize(T & thing, const sf::Vector2f & NEW_SIZE_V) noexcept
    {
        auto newScaleV = thing.getScale();
        const sf::Vector2f ORIG_SIZE_V = Size(thing.getGlobalBounds());

        if (!(misc::IsRealZeroOrLess(NEW_SIZE_V.x) || misc::IsRealZeroOrLess(ORIG_SIZE_V.x)))
        {
            newScaleV.x = (NEW_SIZE_V.x / ORIG_SIZE_V.x);
        }

        if (!(misc::IsRealZeroOrLess(NEW_SIZE_V.y) || misc::IsRealZeroOrLess(ORIG_SIZE_V.y)))
        {
            newScaleV.y = (NEW_SIZE_V.y / ORIG_SIZE_V.y);
        }

        thing.setScale(newScaleV);
    }

    template <typename T, typename = std::enable_if_t<has_getglobalbounds_v<T>>>
    void SetSizeAndPos(T & thing, const sf::FloatRect & R) noexcept
    {
        SetSize(thing, Size(R));
        thing.setPosition(Position(R) + thing.getOrigin());
    }

    template <typename T1, typename T2>
    const sf::Rect<T1>
        AdjustPositionCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & ADJ_V) noexcept
    {
        return SetPositionCopy(R, (Position(R) + sf::Vector2<T1>(ADJ_V)));
    }

    template <typename T1, typename T2>
    void AdjustPosition(sf::Rect<T1> & r, const sf::Vector2<T2> & ADJ_V) noexcept
    {
        r.left += static_cast<T1>(ADJ_V.x);
        r.top += static_cast<T1>(ADJ_V.y);
    }

    template <typename T1, typename T2>
    const sf::Rect<T1>
        AdjustSizeCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & ADJ_V) noexcept
    {
        return SetSizeCopy(R, (Size(R) + sf::Vector2<T1>(ADJ_V)));
    }

    template <typename T1, typename T2>
    void AdjustSize(sf::Rect<T1> & r, const sf::Vector2<T2> & ADJ_V) noexcept
    {
        r.width += static_cast<T1>(ADJ_V.x);
        r.height += static_cast<T1>(ADJ_V.y);
    }

    template <typename T1, typename T2>
    void Move(sf::Rect<T1> & r, const sf::Vector2<T2> & MOVE_V) noexcept
    {
        r.left += static_cast<T1>(MOVE_V.x);
        r.top += static_cast<T1>(MOVE_V.y);
    }

    template <typename T1, typename T2>
    const sf::Rect<T1> MoveCopy(const sf::Rect<T1> & R, const sf::Vector2<T2> & MOVE_V) noexcept
    {
        return sf::Rect<T1>(
            (R.left + static_cast<T1>(MOVE_V.x)),
            (R.top + static_cast<T1>(MOVE_V.y)),
            R.width,
            R.height);
    }

    template <typename T1, typename T2>
    const sf::Rect<T1>
        MakeRect(const sf::Vector2<T1> & POS_V, const sf::Vector2<T2> & SIZE_V) noexcept
    {
        return sf::Rect<T1>(POS_V, sf::Vector2<T1>(SIZE_V));
    }

    template <typename T>
    const sf::Rect<T> MakeRectWithPosition(const sf::Vector2<T> & POS_V) noexcept
    {
        return sf::Rect<T>(POS_V, {});
    }

    template <typename T>
    const sf::Rect<T> MakeRectWithSize(const sf::Vector2<T> & SIZE_V) noexcept
    {
        return sf::Rect<T>({}, SIZE_V);
    }

    template <typename T>
    bool IsZeroOrLessAny(const sf::Vector2<T> & V) noexcept
    {
        return (misc::IsRealZeroOrLess(V.x) || misc::IsRealZeroOrLess(V.y));
    }

    template <typename T>
    bool IsZeroOrLessAny(const sf::Rect<T> & R) noexcept
    {
        return (
            misc::IsRealZeroOrLess(R.left) || misc::IsRealZeroOrLess(R.top)
            || misc::IsRealZeroOrLess(R.width) || misc::IsRealZeroOrLess(R.height));
    }

    template <typename T>
    bool IsZeroOrLessAll(const sf::Vector2<T> & V) noexcept
    {
        return (misc::IsRealZeroOrLess(V.x) && misc::IsRealZeroOrLess(V.y));
    }

    template <typename T>
    bool IsZeroOrLessAll(const sf::Rect<T> & R) noexcept
    {
        return (
            misc::IsRealZeroOrLess(R.left) && misc::IsRealZeroOrLess(R.top)
            && misc::IsRealZeroOrLess(R.width) && misc::IsRealZeroOrLess(R.height));
    }

    template <typename T>
    bool IsLessThanOneAny(const sf::Vector2<T> & V) noexcept
    {
        return ((V.x < T(1)) || (V.y < T(1)));
    }

    template <typename T>
    bool IsLessThanOneAny(const sf::Rect<T> & R) noexcept
    {
        return ((R.left < T(1)) || (R.top < T(1)) || (R.width < T(1)) || (R.height < T(1)));
    }

    template <typename T>
    bool IsLessThanOneAll(const sf::Vector2<T> & V) noexcept
    {
        return ((V.x < T(1)) && (V.y < T(1)));
    }

    template <typename T>
    bool IsLessThanOneAll(const sf::Rect<T> & R) noexcept
    {
        return ((R.left < T(1)) && (R.top < T(1)) && (R.width < T(1)) && (R.height < T(1)));
    }

    template <typename T>
    const sf::Vector2<T> CreateVector(
        const gui::Orientation::Enum ORIENTATION,
        const T VALUE_AT_ORIENTATION,
        const T OTHER_VALUE) noexcept
    {
        if (ORIENTATION == gui::Orientation::Horiz)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, OTHER_VALUE);
        }
        else if (ORIENTATION == gui::Orientation::Vert)
        {
            return sf::Vector2<T>(OTHER_VALUE, VALUE_AT_ORIENTATION);
        }
        else if (ORIENTATION == gui::Orientation::Both)
        {
            return sf::Vector2<T>(VALUE_AT_ORIENTATION, VALUE_AT_ORIENTATION);
        }
        else
        {
            return sf::Vector2<T>(OTHER_VALUE, OTHER_VALUE);
        }
    }

    template <typename T>
    void ChangeVector(
        sf::Vector2<T> & orig, const gui::Orientation::Enum ORIENTATION, const T NEW_VALUE) noexcept
    {
        if (ORIENTATION == gui::Orientation::Horiz)
        {
            orig.x = NEW_VALUE;
        }
        else if (ORIENTATION == gui::Orientation::Vert)
        {
            orig.y = NEW_VALUE;
        }
        else if (ORIENTATION == gui::Orientation::Both)
        {
            orig.x = NEW_VALUE;
            orig.y = NEW_VALUE;
        }
    }

    template <typename T>
    const sf::Vector2<T> ChangeVectorCopy(
        const sf::Vector2<T> & ORIG,
        const gui::Orientation::Enum ORIENTATION,
        const T NEW_VALUE) noexcept
    {
        sf::Vector2<T> v(ORIG);
        ChangeVector(v, ORIENTATION, NEW_VALUE);
        return v;
    }

    // DOES NOT WORK IF SIZES <0: DIFFERENT from sf::Rect::intersects()
    // Even when the sizes are are >=0 it sometimes returns different results than sf::Rect's
    // but I don't need to handle those cases in Heroes' Path so who cares -this is fast.
    template <typename T>
    bool Contains(const sf::Rect<T> & R, const sf::Vector2<T> & P) noexcept
    {
        return !(
            (P.x < R.left) || (P.x > (R.left + R.width)) || (P.y < R.top)
            || (P.y > (R.top + R.height)));
    }

    template <typename T>
    bool Contains(const sf::Rect<T> & R, const float LEFT, const float TOP) noexcept
    {
        return Contains(R, sf::Vector2f(LEFT, TOP));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1>, bool>
        Contains(const T1 & THING, const sf::Vector2<T2> & P) noexcept
    {
        return Contains(THING.getGlobalBounds(), P);
    }

    template <typename T>
    const std::enable_if_t<has_getglobalbounds_v<T>, bool>
        Contains(const T & THING, const float LEFT, const float TOP) noexcept
    {
        return Contains(THING.getGlobalBounds(), sf::Vector2f(LEFT, TOP));
    }

    // works and returns true for sizes=0, but will not work for sizes<0
    template <typename T>
    bool Contains(const sf::Rect<T> & A, const sf::Rect<T> & B) noexcept
    {
        return (
            Contains(A, sf::Vector2<T>(B.left, B.top))
            && Contains(A, sf::Vector2<T>((B.left + B.width), (B.top + B.height))));
    }

    // works and returns true for sizes=0, but will not work for sizes<0
    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1>, bool>
        Contains(const T1 & THING, const sf::Rect<T2> & R) noexcept
    {
        return Contains(THING.getGlobalBounds(), sf::FloatRect(R));
    }

    // works and returns true for sizes=0, but will not work for sizes<0
    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1, T2>, bool>
        Contains(const T1 & THING1, const T2 & THING2) noexcept
    {
        return Contains(THING1.getGlobalBounds(), THING2.getGlobalBounds());
    }

    // DOES NOT WORK IF SIZES <=0: DIFFERENT from sf::Rect::intersects()
    // returns true on overlap: SAME as sf::Rect::intersects()
    // returns true on overlap when sizes=0:  DIFFERENT from sf::Rect::intersects()
    template <typename T>
    bool Intersects(const sf::Rect<T> & A, const sf::Rect<T> & B) noexcept
    {
        return !(
            misc::IsRealGreaterOrClose(A.left, (B.left + B.width))
            || misc::IsRealGreaterOrClose(B.left, (A.left + A.width))
            || misc::IsRealGreaterOrClose(A.top, (B.top + B.height))
            || misc::IsRealGreaterOrClose(B.top, (A.top + A.height)));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1>, sf::Rect<float>>
        Intersects(const T1 & THING, const sf::Rect<T2> & R) noexcept
    {
        return Intersects(THING.getGlobalBounds(), sf::FloatRect(R));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T2>, sf::Rect<float>>
        Intersects(const sf::Rect<T1> & R, const T2 & THING) noexcept
    {
        return Intersects(sf::FloatRect(R), THING.getGlobalBounds());
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1, T2>, sf::Rect<float>>
        Intersects(const T1 & THING1, const T2 & THING2) noexcept
    {
        return Intersects(THING1.getGlobalBounds(), THING2.getGlobalBounds());
    }

    // DOES NOT WORK IF SIZES <=0: DIFFERENT from sf::Rect::intersection().
    // Based on my own Intersects() function which is different from sf::Rect's (see above).
    // Never returns negative sizes.
    template <typename T>
    const std::enable_if_t<misc::are_signed_v<T>, sf::Rect<T>>
        Intersection(const sf::Rect<T> & A, const sf::Rect<T> & B) noexcept
    {
        sf::Rect<T> intersection {};

        if (!Intersects(A, B))
        {
            return intersection;
        }

        intersection = A;

        const auto LEFT_DIFF(B.left - intersection.left);
        if (LEFT_DIFF > T(0))
        {
            intersection.left += LEFT_DIFF;
            intersection.width -= LEFT_DIFF;
        }

        const auto RIGHT_DIFF((intersection.left + intersection.width) - (B.left + B.width));
        if (RIGHT_DIFF > T(0))
        {
            if ((intersection.width < T(0)) || (intersection.width < RIGHT_DIFF))
            {
                intersection.width = T(0);
            }
            else
            {
                intersection.width -= RIGHT_DIFF;
            }
        }

        const auto TOP_DIFF(B.top - intersection.top);
        if (TOP_DIFF > T(0))
        {
            intersection.top += TOP_DIFF;
            intersection.height -= TOP_DIFF;
        }

        const auto BOTTOM_DIFF((intersection.top + intersection.height) - (B.top + B.height));
        if (BOTTOM_DIFF > T(0))
        {
            if ((intersection.height < T(0)) || (intersection.height < BOTTOM_DIFF))
            {
                intersection.height = T(0);
            }
            else
            {
                intersection.height -= BOTTOM_DIFF;
            }
        }

        return intersection;
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1>, sf::Rect<float>>
        Intersection(const T1 & THING, const sf::Rect<T2> & R) noexcept
    {
        return Intersection(THING.getGlobalBounds(), sf::FloatRect(R));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T2>, sf::Rect<float>>
        Intersection(const sf::Rect<T1> & R, const T2 & THING) noexcept
    {
        return Intersection(sf::FloatRect(R), THING.getGlobalBounds());
    }

    template <typename T1, typename T2>
    const std::enable_if_t<has_getglobalbounds_v<T1, T2>, sf::Rect<float>>
        Intersection(const T1 & THING1, const T2 & THING2) noexcept
    {
        return Intersection(THING1.getGlobalBounds(), THING2.getGlobalBounds());
    }

    // returns the smallest sf::Rect that fully includes rects A and B
    template <typename T1, typename T2>
    const sf::Rect<T1> MinimallyEnclosing(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const bool WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS = false) noexcept
    {
        if (WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS)
        {
            const bool IS_EITHER_SIZE_ZERO_OR_LESS_A { IsZeroOrLessAny(Size(A)) };
            const bool IS_EITHER_SIZE_ZERO_OR_LESS_B { IsZeroOrLessAny(Size(B)) };

            if (IS_EITHER_SIZE_ZERO_OR_LESS_A && IS_EITHER_SIZE_ZERO_OR_LESS_B)
            {
                return {};
            }
            else if (IS_EITHER_SIZE_ZERO_OR_LESS_A)
            {
                return sf::Rect<T1>(B);
            }
            else if (IS_EITHER_SIZE_ZERO_OR_LESS_B)
            {
                return A;
            }
        }

        const float MIN_LEFT { static_cast<float>(misc::Min(A.left, static_cast<T1>(B.left))) };
        const float MIN_TOP { static_cast<float>(misc::Min(A.top, static_cast<T1>(B.top))) };

        return {
            static_cast<T1>(MIN_LEFT),
            static_cast<T1>(MIN_TOP),
            static_cast<T1>(
                static_cast<float>(misc::Max((A.left + A.width), (B.left + B.width))) - MIN_LEFT),
            static_cast<T1>(
                static_cast<float>(misc::Max((A.top + A.height), (B.top + B.height))) - MIN_TOP)
        };
    }

    template <typename T, typename... Ts>
    const sf::Rect<T> MinimallyEnclosing(const sf::Rect<T> & FIRST, const Ts... OTHERS) noexcept
    {
        return MinimallyEnclosing(FIRST, MinimallyEnclosing(OTHERS...));
    }

    template <typename T1, typename T2>
    const std::enable_if_t<(has_getglobalbounds_v<T1, T2>), sf::Rect<float>> MinimallyEnclosing(
        const T1 & A,
        const T2 & B,
        const bool WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS = false) noexcept
    {
        return MinimallyEnclosing(
            A.getGlobalBounds(), B.getGlobalBounds(), WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS);
    }

    template <typename T, typename... Ts>
    const typename std::enable_if_t<has_getglobalbounds_v<T, Ts...>, sf::Rect<float>>
        MinimallyEnclosing(const T & FIRST, const Ts... OTHERS) noexcept
    {
        return MinimallyEnclosing(FIRST.getGlobalBounds(), MinimallyEnclosing(OTHERS...));
    }

    template <
        typename Iterator_t,
        typename = std::enable_if_t<misc::are_forward_iterator_v<Iterator_t>>>
    auto MinimallyEnclosing(
        Iterator_t first,
        Iterator_t last,
        const bool WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS = false)
    {
        rect_type_picker_t<misc::iterator_value_type_t<Iterator_t>> minimalRect {};

        if (first != last)
        {
            minimalRect = rect_extractor(*first);

            while (++first != last)
            {
                minimalRect = MinimallyEnclosing(
                    minimalRect, rect_extractor(*first), WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS);
            }
        }

        return minimalRect;
    }

    template <
        typename T,
        typename = std::enable_if_t<misc::are_forward_iterator_v<typename T::const_iterator>>>
    auto MinimallyEnclosing(
        const T & CONTAINER, const bool WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS = false)
    {
        return MinimallyEnclosing(
            std::begin(CONTAINER), std::end(CONTAINER), WILL_IGNORE_ANY_WITH_SIZE_ZERO_OR_LESS);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_COMMON_HPP_INCLUDED

/*


[3300x2100 : 32 ? ?]                [3300x2100 : 32 ? ?]
[2880x1800 : 32 16:10 Retina]       [2880x1800 : 32 16:10 Retina]
[2560x1600 : 32 8:5 WQXGA]          [2560x1600 : 32 8:5 WQXGA]
[2304x1440 : 32 8:5 ...]            [2304x1440 : 32 8:5 ...]
[2048x1536 : 32 4:3 2K]             [2048x1536 : 32 4:3 2K]
[1680x1048 : 32 ? ?]                [1680x1048 : 32 ? ?]
[1600x1200 : 32 4:3 UXGA]           [1600x1200 : 32 4:3 UXGA]
[1280x960 : 32 4:3 SXVGA]           [1280x960 : 32 4:3 SXVGA]
                                    [3840x2400 : 32 16:10 WQUXGA]











*/
