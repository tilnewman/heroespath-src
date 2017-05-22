#ifndef SFMLUTIL_BOX_INCLUDED
#define SFMLUTIL_BOX_INCLUDED
//
// box.hpp
//  Box drawing utility class
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/line.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box-info.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{
namespace box
{

    float GetMinWidth_Box(const bool WILL_GROW_TO_FIT);
    float GetMinHeight_Box(const bool WILL_GROW_TO_FIT);


    //A simple box drawing utitily class
    //Assumes corner images are symmetrical
    class Box : public GuiEntity
    {
        //prevent copy construction and copy assignment
        Box(const Box &) =delete;
        Box & operator=(const Box &) =delete;

    public:
        //if no box::Info object is given, then Setup() must be called before any other function
        explicit Box(const std::string & NAME, const box::Info & BOX_INFO = box::Info());

        virtual ~Box();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

        void SetupBox(const box::Info & BOX_INFO = box::Info());

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        inline virtual const box::Info BoxInfo() const { return boxInfo_; }

        virtual bool SetHasFocus(const bool);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnColorChange();

        box::Info boxInfo_;
        Line topLine_;
        Line rightLine_;
        Line botLine_;
        Line leftLine_;
        sf::Sprite cTopLeftSprite_;
        sf::Sprite cTopRightSprite_;
        sf::Sprite cBotLeftSprite_;
        sf::Sprite cBotRightSprite_;
        BackgroundImage backgroundImage_;
    };

    using BoxSPtr_t = std::shared_ptr<Box>;
    using BoxSVec_t = std::vector<BoxSPtr_t>;

}
}
}
#endif //SFMLUTIL_BOX_INCLUDED
