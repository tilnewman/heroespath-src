#ifndef SFMLUTIL_SFML_UTIL_INCLUDED
#define SFMLUTIL_SFML_UTIL_INCLUDED
//
// sfml-util.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/sfml-window.hpp"
#include "sfml-util/handy-types.hpp"
#include "sfml-util/real-utils.hpp"

#include "heroespath/assertlogandthrow.hpp"

#include <string>
#include <vector>
#include <sstream>


namespace sfml_util
{

    template<typename T>
    T MagnitudeSFML(const sf::Vector2<T> & V)
    {
        return std::sqrt(V.x * V.x + V.y * V.y);
    }


    template<typename T>
    void NormalizeSFML(sf::Vector2<T> & v)
    {
        const T M(MagnitudeSFML<T>(v));
        if (M > static_cast<T>(0))
            v /= M;
    }


    //assumes 'facing right'
    float GetAngleInDegrees(const sf::Vector2f & BEGIN_POS_V,
                            const sf::Vector2f & END_POS_V);


    const sf::Vector2f ProjectToScreenEdge(const sf::Vector2f & FIRST_POS_V,
                                           const sf::Vector2f & SECOND_POS_V,
                                           const sf::Vector2f & PROJECTED_IMAGE_SIZE);


    template<typename Before_t, typename After_t>
    sf::Vector2<After_t> ConvertVector(const sf::Vector2<Before_t> & V)
    {
        return sf::Vector2<After_t>(static_cast<After_t>(V.x), static_cast<After_t>(V.y));
    }


    template<typename Before_t, typename After_t>
    sf::Rect<After_t> ConvertRect(const sf::Rect<Before_t> & R)
    {
        sf::Rect<After_t> tempRect;
        tempRect.left   = static_cast<After_t>(R.left);
        tempRect.top    = static_cast<After_t>(R.top);
        tempRect.width  = static_cast<After_t>(R.width);
        tempRect.height = static_cast<After_t>(R.height);
        return tempRect;
    }


    template<typename T>
    const std::string VectorToString(const sf::Vector2<T> & V, const bool WILL_WRAP = true)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "(" : "") << V.x << "," << V.y << ((WILL_WRAP) ? ")" : "");
        return ss.str();
    }


    template<typename T>
    void VectorToStream(const sf::Vector2<T> V, std::ostream & stream)
    {
        stream << Vector2String(V);
    }


    template<typename T>
    const std::string RectToString(const sf::Rect<T> & R, const bool WILL_WRAP = true)
    {
        std::ostringstream ss;
        ss << ((WILL_WRAP) ? "[" : "") << R.left << "," << R.top << " " << R.width << "x" << R.height << ((WILL_WRAP) ? "]" : "");
        return ss.str();
    }


    template<typename T>
    void RectToStream(const sf::Rect<T> R, std::ostream stream)
    {
        stream << Rect2String(R);
    }


    //Drawing rectangles easily is often handy when testing graphics.
    template<typename T=float>
    void DrawRectangle( sf::RenderTarget & target,
                        sf::RenderStates & states,
                        const T            LEFT,
                        const T            TOP,
                        const T            WIDTH,
                        const T            HEIGHT,
                        const sf::Color    LINE_COLOR     = sf::Color::White,
                        const T            LINE_THICKNESS = static_cast<T>(1),
                        const sf::Color    FILL_COLOR     = sf::Color::Transparent )
    {
        sf::RectangleShape r;
        r.setPosition(static_cast<float>(LEFT), static_cast<float>(TOP));
        r.setSize(sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT)));
        r.setFillColor(FILL_COLOR);
        r.setOutlineColor(LINE_COLOR);
        r.setOutlineThickness(static_cast<float>(LINE_THICKNESS));
        target.draw(r, states);
    }


    template<typename PosV_t, typename SizeV_t, typename Thickness_t>
    void DrawRectangle( sf::RenderTarget &         target,
                        sf::RenderStates &         states,
                        const sf::Vector2<PosV_t>  POS_V,
                        const sf::Vector2<SizeV_t> SIZE_V,
                        const sf::Color            LINE_COLOR     = sf::Color::White,
                        const Thickness_t          LINE_THICKNESS = static_cast<Thickness_t>(1),
                        const sf::Color            FILL_COLOR     = sf::Color::Transparent )
    {
        DrawRectangle(  target,
                        states,
                        static_cast<float>(POS_V.x),
                        static_cast<float>(POS_V.y),
                        static_cast<float>(SIZE_V.x),
                        static_cast<float>(SIZE_V.y),
                        LINE_COLOR,
                        static_cast<float>(LINE_THICKNESS),
                        FILL_COLOR );
    }


    template<typename T>
    void DrawRectangle( sf::RenderTarget &  target,
                        sf::RenderStates &  states,
                        const sf::Rect<T>   REGION_RECT,
                        const sf::Color     LINE_COLOR      = sf::Color::White,
                        const T             LINE_THICKNESS  = static_cast<T>(1),
                        const sf::Color     FILL_COLOR      = sf::Color::Transparent )
    {
        DrawRectangle(  target,
                        states,
                        static_cast<float>(REGION_RECT.left),
                        static_cast<float>(REGION_RECT.top),
                        static_cast<float>(REGION_RECT.width),
                        static_cast<float>(REGION_RECT.height),
                        LINE_COLOR,
                        static_cast<float>(LINE_THICKNESS),
                        FILL_COLOR );
    }


    const std::string ColorToString(const sf::Color & C, const bool WILL_WRAP = true);


    template<typename Given_t, typename Return_t>
    Return_t FindPowerOf2GreaterThan(const Given_t A)
    {
        int x( static_cast<int>(A) );
        int y( static_cast<int>(A) );

        while (x &= (x ^ (~x + 1)))
            y = x << 1;

        return static_cast<Return_t>(y);
    }


    template<typename T>
    RendTextSPtr_t CreateRenderTextureAtPowerOf2Size(const T WIDTH, const T HEIGHT)
    {
        sf::RenderTexture r;
        RendTextSPtr_t sptr(new sf::RenderTexture );

        const unsigned int NEW_WIDTH ( FindPowerOf2GreaterThan<T, unsigned int>(WIDTH)  );
        const unsigned int NEW_HEIGHT( FindPowerOf2GreaterThan<T, unsigned int>(HEIGHT) );

        const bool RESULT( sptr->create(NEW_WIDTH, NEW_HEIGHT) );

        M_ASSERT_OR_LOGANDTHROW_SS((RESULT == true), "sfml_util::CreateRenderTextureAtPowerOf2Size(" << WIDTH << "," << HEIGHT << ") resized to sf::RenderTexture::create(" << NEW_WIDTH << "," << NEW_HEIGHT << ") failed.  See console output for details." );

        return sptr;
    }


    const std::string KeyCodeToString(const sf::Keyboard::Key KEY);


    const std::string VideoModeToString(const sf::VideoMode & VM, const bool WILL_WRAP = true);

    void GetResolutionAreas(float & resAreaCurrent, float & resAreaMin, float & resAreaMax);

    //map a VAL within [IN_MIN, IN_MAX] to the range [OUT_MIN, OUT_MAX]
    template<typename T>
    inline T Map(const T VAL,
                 const T IN_MIN,
                 const T IN_MAX,
                 const T OUT_MIN,
                 const T OUT_MAX)
    {
        return OUT_MIN + (((VAL - IN_MIN) * (OUT_MAX - OUT_MIN)) / (IN_MAX - IN_MIN));
    }


    //return a number within [OUT_MIN, OUT_MAX] proportional to the current resolution vs the max supported resolution
    template<typename T>
    T MapByRes(const T OUT_MIN, const T OUT_MAX)
    {
        float minResArea(0.0f);
        float maxResArea(0.0f);
        float currentResArea(0.0f);
        GetResolutionAreas(currentResArea, minResArea, maxResArea);
        const float ACTUAL_MAX((currentResArea > maxResArea) ? currentResArea : maxResArea);
        return static_cast<T>(Map(currentResArea, minResArea, ACTUAL_MAX, static_cast<float>(OUT_MIN), static_cast<float>(OUT_MAX)));
    }


    //linux SFML lib does not seem to support outline fonts...
    void SetTextColor(sf::Text & text, const sf::Color & COLOR);


    template<typename T>
    void ScaleRect(sf::Rect<T> & rect, const float SCALE)
    {
        rect.left   = static_cast<T>(static_cast<float>(rect.left) * SCALE);
        rect.top    = static_cast<T>(static_cast<float>(rect.top) * SCALE);
        rect.width  = static_cast<T>(static_cast<float>(rect.width) * SCALE);
        rect.height = static_cast<T>(static_cast<float>(rect.height) * SCALE);
    }

    template<typename T>
    const sf::Rect<T> ScaleRectCopy(const sf::Rect<T> & RECT, const float SCALE)
    {
        sf::Rect<T> r(RECT);
        ScaleRect<T>(r, SCALE);
        return r;
    }


    template<typename T>
    T RelativeOffset(const T ORIG_POS, const T NEW_POS)
    {
        const float ORIG_POS_FLOAT(static_cast<float>(ORIG_POS));
        const float NEW_POS_FLOAT(static_cast<float>(NEW_POS));
        return static_cast<T>(NEW_POS_FLOAT + ((NEW_POS_FLOAT + ORIG_POS_FLOAT) / NEW_POS_FLOAT));
    }

    template<typename T>
    sf::Vector2<T> RelativeOffsetPosition(const T ORIG_LEFT, const T ORIG_TOP, const T NEW_LEFT, const T NEW_TOP)
    {
        return sf::Vector2<T>(RelativeOffset(ORIG_LEFT, NEW_LEFT), RelativeOffset(ORIG_TOP, NEW_TOP));
    }

    template<typename T>
    sf::Vector2<T> RelativeOffsetPosition(const sf::Vector2<T> & ORIG_POS_V, const sf::Vector2<T> & NEW_POS_V)
    {
        return RelativeOffsetPosition(ORIG_POS_V.x, ORIG_POS_V.y, NEW_POS_V.x, NEW_POS_V.y);
    }


    void FlipHoriz(sf::Texture &);

    TextureSPtr_t FlipHorizCopy(const sf::Texture &);

    void FlipVert(sf::Texture &);

    TextureSPtr_t FlipVertCopy(const sf::Texture &);


    template<typename T>
    T Average(const std::vector<T> & V)
    {
        if (V.empty())
            return T(0);

        T sum(0);
        for (auto const NEXT_VALUE : V)
            sum += NEXT_VALUE;

        return sum / V.size();
    }

    template<typename T>
    T StandardDeviation(const std::vector<T> & V, T & average)
    {
        if (V.empty())
            return T(0);

        average = Average(V);

        T deviationSum(0);

        for (auto const NEXT_VALUE : V)
            deviationSum += std::pow((NEXT_VALUE - average), 2);

        return std::sqrt(deviationSum / V.size());
    }


    void Invert(sf::Texture & texture, const bool WILL_INVERT_ALPHA = false);

    void Mask(sf::Texture &     texture,
              const sf::Color & COLOR_TO_MASK = sf::Color::Black,
              const sf::Uint8   NEW_ALPHA     = 0);


    void SpriteToRenderTexture(sf::Sprite &             sprite,
                               sf::RenderTexture &      renderTexture,
                               const sf::RenderStates & RENDER_STATES = sf::RenderStates(),
                               const bool               WILL_DISPLAY  = true);


    void SpriteToImage(sf::Sprite & sprite,
                       sf::Image & image,
                       const sf::RenderStates & RENDER_STATES = sf::RenderStates());


    bool ColorLess(const sf::Color & L, const sf::Color & R);

    bool BlendModeLess(const sf::BlendMode & L, const sf::BlendMode & R);


}//end of namespace sfml_util

#endif //SFMLUTIL_SFML_UTIL_INCLUDED
