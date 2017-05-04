#ifndef SFMLUTIL_GUIIMAGE_INCLUDED
#define SFMLUTIL_GUIIMAGE_INCLUDED
//
// gui-entity-image.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Encapsulates a GuiEntity that manages three images based on entityMouseState_.
    class GuiImage : public GuiEntity
    {
        //prevent copy construction and copy assignment
        GuiImage(const GuiImage &);
        GuiImage & operator=(const GuiImage &);

    public:
        GuiImage(const std::string &   NAME,
                 const sf::FloatRect & SCREEN_REGION,
                 const sf::Sprite &    SPRITE_UP   = sf::Sprite(),
                 const sf::Sprite &    SPRITE_DOWN = sf::Sprite(),
                 const sf::Sprite &    SPRITE_OVER = sf::Sprite());

        GuiImage(const std::string & NAME,
                 const float         POS_LEFT,
                 const float         POS_TOP,
                 const sf::Sprite &  SPRITE_UP   = sf::Sprite(),
                 const sf::Sprite &  SPRITE_DOWN = sf::Sprite(),
                 const sf::Sprite &  SPRITE_OVER = sf::Sprite());

        virtual ~GuiImage();

        virtual void Setup(const float POS_LEFT, const float POS_TOP);

        //The only member required by sf::Drawable.
        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SpriteColorsSet(const sf::Color & NEW_COLOR);
        virtual void SpriteColorsReset();

        inline sf::Sprite & GetUpSprite()   { return upSprite_;   }
        inline sf::Sprite & GetDownSprite() { return downSprite_; }
        inline sf::Sprite & GetOverSprite() { return overSprite_; }

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}

        sf::Sprite upSprite_;
        sf::Sprite downSprite_;
        sf::Sprite overSprite_;
    };


    using GuiImageSPtr_t = std::shared_ptr<GuiImage>;
    using GuiImageSVec_t = std::vector<GuiImageSPtr_t>;

}
}

#endif //SFMLUTIL_GUIIMAGE_INCLUDED
