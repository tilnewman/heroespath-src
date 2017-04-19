#ifndef APPBASE_SFMLUTIL_BACKGROUNDIMAGE_INCLUDED
#define APPBASE_SFMLUTIL_BACKGROUNDIMAGE_INCLUDED
//
// background-image.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-info.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    //Encapsulates a GuiEntity that manages a single background image with possible gradients.
    //If both path.empty() and textureSPtr.get==nullptr, then a simple rectangle will be drawn and filled with the given color.
    class BackgroundImage : public sf::Drawable
    {
        //prevent copy construction
        BackgroundImage(const BackgroundImage &);

        //prevent copy assignment
        BackgroundImage & operator=(const BackgroundImage &);

    public:
        //if using this constructor then Setup() must be called before any other function
        BackgroundImage();

        BackgroundImage(const BackgroundInfo & BG_INFO,
                        const float            IMAGE_SCALE       = 1.0f,
                        const bool             WILL_SMOOTH_IMAGE = false);

        //use this constructor to create a background tile that is scaled per resolution like is used on the main menu backgrounds
        BackgroundImage(const std::string & MEDIA_PATH_KEY_STR);

        virtual ~BackgroundImage();

        void Setup(const BackgroundInfo & BG_INFO,
                   const float            SCALE             = 1.0f,
                   const bool             WILL_SMOOTH_IMAGE = false);

        void Reset();

        //The only member required by sf::Drawable.
        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetColor(const sf::Color & NEW_COLOR);

        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);
        virtual void MovePosition(const float HORIZ, const float VERT);

        inline virtual const sf::Color      GetColor() const     { return bgInfo_.color; }
        inline virtual const sf::FloatRect  GetRegion() const    { return bgInfo_.region; }
        inline virtual bool                 IsImage() const      { return (sprite_.getTexture() != nullptr); }
        inline virtual bool                 IsRectangle() const  { return ! IsImage(); }

        void SetImageDetails(const float SCALE = 1.0f, const bool WILL_SMOOTH = false);

        inline const sfml_util::GradientInfo GradientInfo() const { return bgInfo_.gradient_info; }

        inline const BackgroundInfo BackgroundInfo() const { return bgInfo_; }

        inline float ImageScale() const { return sprite_.getScale().x; }

        inline bool IsTextureSmoothed() const { if (bgInfo_.textureSPtr.get() == nullptr) return false; else return true; }

    protected:
        GradientRect gradient_;
        sfml_util::gui::BackgroundInfo bgInfo_;
        sf::Sprite sprite_;
    };


    using BackgroundImageSPtr_t = std::shared_ptr<BackgroundImage>;

}
}

#endif //APPBASE_SFMLUTIL_BACKGROUNDIMAGE_INCLUDED
