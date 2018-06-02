// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
//
// sfml-util.hpp
//
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-window.hpp"

#include "log/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/handy-types.hpp"
#include "misc/real.hpp"

#include <cmath>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace sf
{

template <typename T>
constexpr bool operator<(const sf::Rect<T> & A, const sf::Rect<T> & B)
{
    return std::tie(A.left, A.top, A.width, A.height) < std::tie(B.left, B.top, B.width, B.height);
}

} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    template <typename T>
    const sf::Vector2<T> RectCenter(const sf::Rect<T> & RECT)
    {
        return sf::Vector2<T>((RECT.left + (RECT.width / T(2))), (RECT.top + (RECT.height / T(2))));
    }

    const sf::Vector2f SpriteCenter(const sf::Sprite & SPRITE);

    template <typename T>
    sfml_util::Direction::Enum DirectionFromAToB(const sf::Vector2<T> & A, const sf::Vector2<T> & B)
    {
        auto const DIFF_HORIZ_ABS{ std::abs(B.x - A.x) };
        auto const DIFF_VERT_ABS{ std::abs(B.y - A.y) };

        if (DIFF_HORIZ_ABS > DIFF_VERT_ABS)
        {
            if (B.x < A.x)
            {
                return sfml_util::Direction::Left;
            }
            else
            {
                return sfml_util::Direction::Right;
            }
        }
        else if (DIFF_VERT_ABS > DIFF_HORIZ_ABS)
        {
            if (B.y < A.y)
            {
                return sfml_util::Direction::Up;
            }
            else
            {
                return sfml_util::Direction::Down;
            }
        }
        else
        {
            if (B.x < A.x)
            {
                return sfml_util::Direction::Left;
            }
            else if (A.x < B.x)
            {
                return sfml_util::Direction::Right;
            }
            else
            {
                return sfml_util::Direction::Count;
            }
        }
    }

    template <typename T>
    sfml_util::Direction::Enum DirectionFromAToB(const sf::Rect<T> & A, const sf::Rect<T> & B)
    {
        return DirectionFromAToB(RectCenter(A), RectCenter(B));
    }

    sfml_util::Direction::Enum DirectionFromAToB(const sf::Sprite & A, const sf::Sprite & B);

    template <typename T>
    constexpr bool DoRectsOverlap(const sf::Rect<T> & A, const sf::Rect<T> & B)
    {
        return (
            (A.left < (B.left + B.width)) && ((A.left + A.width) > B.left)
            && (A.top < (B.top + B.height)) && ((A.top + A.height) > B.top));
    }

    template <typename T>
    constexpr T MagnitudeSFML(const sf::Vector2<T> & V)
    {
        return std::sqrt(V.x * V.x + V.y * V.y);
    }

    template <typename T>
    constexpr void NormalizeSFML(sf::Vector2<T> & v)
    {
        const T M(MagnitudeSFML<T>(v));
        if (M > static_cast<T>(0))
            v /= M;
    }

    template <typename T>
    constexpr T Distance(const sf::Vector2<T> & A, const sf::Vector2<T> & B)
    {
        auto const DIST_HORIZ{ static_cast<float>(B.x) - static_cast<float>(A.x) };
        auto const DIST_VERT{ static_cast<float>(B.y) - static_cast<float>(A.y) };
        return static_cast<T>(std::sqrt((DIST_HORIZ * DIST_HORIZ) + (DIST_VERT * DIST_VERT)));
    }

    const sf::FloatRect MakeMinimalSquareAndCenter(const sf::FloatRect & REGION);

    inline void CenterSprite(sf::Sprite & s, const sf::FloatRect & R)
    {
        s.setPosition(
            (R.left + (R.width * 0.5f)) - (s.getGlobalBounds().width * 0.5f),
            (R.top + (R.height * 0.5f)) - (s.getGlobalBounds().height * 0.5f));
    }

    void ScaleSpriteToFit(
        sf::Sprite & s, const float WIDTH, const float HEIGHT, const float RESIZE_RATIO = 1.0f);

    inline void
        ScaleSpriteToFit(sf::Sprite & s, const sf::FloatRect & R, const float RESIZE_RATIO = 1.0f)
    {
        ScaleSpriteToFit(s, R.width, R.height, RESIZE_RATIO);
    }

    inline void CenterAndScaleSpriteToFit(
        sf::Sprite & s, const sf::FloatRect & R, const float RESIZE_RATIO = 1.0f)
    {
        ScaleSpriteToFit(s, R, RESIZE_RATIO);
        CenterSprite(s, R);
    }

    inline const sf::Vector2f SpriteSize(const sf::Sprite & SPRITE)
    {
        return sf::Vector2f(SPRITE.getGlobalBounds().width, SPRITE.getGlobalBounds().height);
    }

    // assumes 'facing right'
    float GetAngleInDegrees(const sf::Vector2f & BEGIN_POS_V, const sf::Vector2f & END_POS_V);

    const sf::Vector2f ProjectToScreenEdge(
        const sf::Vector2f & FIRST_POS_V,
        const sf::Vector2f & SECOND_POS_V,
        const sf::Vector2f & PROJECTED_IMAGE_SIZE);

    template <typename Before_t, typename After_t>
    constexpr const sf::Vector2<After_t> ConvertVector(const sf::Vector2<Before_t> & V)
    {
        return sf::Vector2<After_t>(static_cast<After_t>(V.x), static_cast<After_t>(V.y));
    }

    template <typename Before_t, typename After_t>
    constexpr const sf::Rect<After_t> ConvertRect(const sf::Rect<Before_t> & R)
    {
        sf::Rect<After_t> tempRect;
        tempRect.left = static_cast<After_t>(R.left);
        tempRect.top = static_cast<After_t>(R.top);
        tempRect.width = static_cast<After_t>(R.width);
        tempRect.height = static_cast<After_t>(R.height);
        return tempRect;
    }

    template <typename T>
    inline const std::string VectorToString(const sf::Vector2<T> & V, const bool WILL_WRAP = true)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "(" : "") << V.x << "," << V.y << ((WILL_WRAP) ? ")" : "");
        return ss.str();
    }

    template <typename T>
    inline void VectorToStream(const sf::Vector2<T> & V, std::ostream & stream)
    {
        stream << Vector2String(V);
    }

    template <typename T>
    inline const std::string RectToString(const sf::Rect<T> & R, const bool WILL_WRAP = true)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "[" : "") << R.left << "," << R.top << " " << R.width << "x"
           << R.height << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }

    template <typename T>
    void RectToStream(const sf::Rect<T> & R, std::ostream & stream)
    {
        stream << Rect2String(R);
    }

    // Drawing rectangles easily is often handy when testing graphics.
    template <typename T = float>
    inline void DrawRectangle(
        sf::RenderTarget & target,
        const sf::RenderStates & STATES,
        const T LEFT,
        const T TOP,
        const T WIDTH,
        const T HEIGHT,
        const sf::Color & LINE_COLOR = sf::Color::White,
        const T LINE_THICKNESS = static_cast<T>(1),
        const sf::Color & FILL_COLOR = sf::Color::Transparent)
    {
        sf::RectangleShape r;
        r.setPosition(static_cast<float>(LEFT), static_cast<float>(TOP));
        r.setSize(sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT)));
        r.setFillColor(FILL_COLOR);
        r.setOutlineColor(LINE_COLOR);
        r.setOutlineThickness(static_cast<float>(LINE_THICKNESS));
        target.draw(r, STATES);
    }

    template <typename PosV_t, typename SizeV_t, typename Thickness_t>
    inline void DrawRectangle(
        sf::RenderTarget & target,
        const sf::RenderStates & STATES,
        const sf::Vector2<PosV_t> & POS_V,
        const sf::Vector2<SizeV_t> & SIZE_V,
        const sf::Color & LINE_COLOR = sf::Color::White,
        const Thickness_t LINE_THICKNESS = static_cast<Thickness_t>(1),
        const sf::Color & FILL_COLOR = sf::Color::Transparent)
    {
        DrawRectangle(
            target,
            STATES,
            static_cast<float>(POS_V.x),
            static_cast<float>(POS_V.y),
            static_cast<float>(SIZE_V.x),
            static_cast<float>(SIZE_V.y),
            LINE_COLOR,
            static_cast<float>(LINE_THICKNESS),
            FILL_COLOR);
    }

    template <typename T>
    inline void DrawRectangle(
        sf::RenderTarget & target,
        const sf::RenderStates & STATES,
        const sf::Rect<T> & REGION_RECT,
        const sf::Color & LINE_COLOR = sf::Color::White,
        const T LINE_THICKNESS = static_cast<T>(1),
        const sf::Color & FILL_COLOR = sf::Color::Transparent)
    {
        DrawRectangle(
            target,
            STATES,
            static_cast<float>(REGION_RECT.left),
            static_cast<float>(REGION_RECT.top),
            static_cast<float>(REGION_RECT.width),
            static_cast<float>(REGION_RECT.height),
            LINE_COLOR,
            static_cast<float>(LINE_THICKNESS),
            FILL_COLOR);
    }

    void DrawRectangleWithLineVerts(
        const sf::FloatRect & RECT, const sf::Color & COLOR, std::vector<sf::Vertex> & verts);

    void DrawQuad(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_LEFT,
        const sf::Color & COLOR_RIGHT,
        std::vector<sf::Vertex> & verts);

    template <typename Given_t, typename Return_t>
    inline Return_t FindPowerOf2GreaterThan(const Given_t A)
    {
        auto x{ static_cast<int>(A) };
        auto y{ x };

        while (x &= (x ^ (~x + 1)))
        {
            y = x << 1;
        }

        return static_cast<Return_t>(y);
    }

    const std::string KeyCodeToString(const sf::Keyboard::Key KEY);

    const std::string VideoModeToString(const sf::VideoMode & VM, const bool WILL_WRAP = true);

    // map a VAL within [IN_MIN, IN_MAX] to the range [OUT_MIN, OUT_MAX]
    template <typename T>
    constexpr T Map(const T VAL, const T IN_MIN, const T IN_MAX, const T OUT_MIN, const T OUT_MAX)
    {
        return OUT_MIN + (((VAL - IN_MIN) * (OUT_MAX - OUT_MIN)) / (IN_MAX - IN_MIN));
    }

    // maps[(1280*900), (7680*4800)/2] at the current resolution width*height to [THE_MIN, THE_MAX],
    // which with THE_MIN=0.0f and THE_MAX=(below) and a current res of 2880x1880 yeilds:
    //  1000 = 233
    //  500  = 117
    //  100  = 23
    //  50   = 11.65
    //  20   = 4.65
    //  10   = 2.33
    float MapByRes(const float THE_MIN, const float THE_MAX);

    inline unsigned MapByRes(const unsigned THE_MIN, const unsigned THE_MAX)
    {
        return static_cast<unsigned>(
            MapByRes(static_cast<float>(THE_MIN), static_cast<float>(THE_MAX)));
    }

    inline float SpacerOld(const float AMOUNT) { return MapByRes(0.0f, AMOUNT); }

    float ScreenRatioToPixelsHoriz(const float RATIO);
    float ScreenRatioToPixelsVert(const float RATIO);

    // linux SFML lib does not seem to support outline fonts...
    void SetTextColor(sf::Text & text, const sf::Color & COLOR);

    template <typename T>
    constexpr void ScaleRect(sf::Rect<T> & rect, const float SCALE)
    {
        rect.left = static_cast<T>(static_cast<float>(rect.left) * SCALE);
        rect.top = static_cast<T>(static_cast<float>(rect.top) * SCALE);
        rect.width = static_cast<T>(static_cast<float>(rect.width) * SCALE);
        rect.height = static_cast<T>(static_cast<float>(rect.height) * SCALE);
    }

    template <typename T>
    constexpr const sf::Rect<T> ScaleRectCopy(const sf::Rect<T> & RECT, const float SCALE)
    {
        sf::Rect<T> r(RECT);
        ScaleRect<T>(r, SCALE);
        return r;
    }

    template <typename T>
    constexpr T RelativeOffset(const T ORIG_POS, const T NEW_POS)
    {
        auto const ORIG_POS_FLOAT{ static_cast<float>(ORIG_POS) };
        auto const NEW_POS_FLOAT{ static_cast<float>(NEW_POS) };
        return static_cast<T>(NEW_POS_FLOAT + ((NEW_POS_FLOAT + ORIG_POS_FLOAT) / NEW_POS_FLOAT));
    }

    template <typename T>
    constexpr const sf::Vector2<T> RelativeOffsetPosition(
        const T ORIG_LEFT, const T ORIG_TOP, const T NEW_LEFT, const T NEW_TOP)
    {
        return sf::Vector2<T>(
            RelativeOffset(ORIG_LEFT, NEW_LEFT), RelativeOffset(ORIG_TOP, NEW_TOP));
    }

    template <typename T>
    constexpr const sf::Vector2<T>
        RelativeOffsetPosition(const sf::Vector2<T> & ORIG_POS_V, const sf::Vector2<T> & NEW_POS_V)
    {
        return RelativeOffsetPosition(ORIG_POS_V.x, ORIG_POS_V.y, NEW_POS_V.x, NEW_POS_V.y);
    }

    void FlipHoriz(sf::Texture &);

    void FlipHorizCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture);

    void FlipVert(sf::Texture &);

    void FlipVertCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture);

    void Invert(sf::Texture & texture, const bool WILL_INVERT_ALPHA = false);

    void Mask(
        sf::Texture & texture,
        const sf::Color & COLOR_TO_MASK = sf::Color::Black,
        const sf::Uint8 NEW_ALPHA = 0);

    void SpriteToRenderTexture(
        sf::Sprite & sprite,
        sf::RenderTexture & renderTexture,
        const sf::RenderStates & RENDER_STATES = sf::RenderStates(),
        const bool WILL_DISPLAY = true);

    void SpriteToImage(
        sf::Sprite & sprite,
        sf::Image & image,
        const sf::RenderStates & RENDER_STATES = sf::RenderStates());

    namespace color
    {
        bool ColorLess(const sf::Color & L, const sf::Color & R);

        bool BlendModeLess(const sf::BlendMode & L, const sf::BlendMode & R);

        const sf::Color TransitionColor(
            const sf::Color & FROM, const sf::Color & TO, const float RATIO_COMPLETE);

        const std::string ColorToString(const sf::Color & C, const bool WILL_WRAP = true);
    } // namespace color

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_HPP_INCLUDED
