#ifndef SFMLUTIL_LINE_INCLUDED
#define SFMLUTIL_LINE_INCLUDED
//
// line.hpp
//  Line drawing utility class
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/orientation-enum.hpp"
#include "sfml-util/side-enum.hpp"
#include "gui-entity.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //A simple line drawing gui entity
    class Line : public GuiEntity
    {
        //prevent copy construction
        Line(const Line &);

        //prevent copy assignment
        Line & operator=(const Line &);

    public:
        Line(const std::string & NAME,
             const float         LEFT,
             const float         TOP,
             const std::size_t   LENGTH,
             Orientation::Enum   ORIENTATION,
             const Side::Enum    SIDE,
             const bool          WILL_CAP_ENDS = true);

        //Use this constructor when position and length are not initially know,
        //but must call Setup() before any other function after construction.
        Line(const std::string & NAME,
             Orientation::Enum   ORIENTATION,
             const Side::Enum    SIDE,
             const bool          WILL_CAP_ENDS);

        virtual ~Line();

        void Setup(const float  LEFT,
                   const float  TOP,
                   const std::size_t LENGTH);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnColorChange();

        void SetupBaseSprites();
        void SetupMiddleSprites();
        void SetupOffScreenTexture();

    private:
        std::size_t             length_;
        const Orientation::Enum ORIENTATION_;
        const Side::Enum        SIDE_;
        const bool              WILL_CAP_ENDS_;
        std::size_t             pixelsOfMiddleToUse_;
        std::size_t             middleCount_;
        sf::Sprite              middleSprite_;
        sf::Sprite              endTopOrLeftSprite_;
        sf::Sprite              endBotOrRightSprite_;
        sf::Sprite              finalSprite_;//used to draw to screen
        sf::RenderTexture       offScreenTexture_;
    };


    using LineSPtr_t = std::shared_ptr<Line>;
    using LineSVec_t = std::vector<LineSPtr_t>;

}
}
#endif //SFMLUTIL_LINE_INCLUDED
