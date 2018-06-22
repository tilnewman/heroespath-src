// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
//
// background-image.hpp
//
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Encapsulates a GuiEntity that manages a single background image with possible gradients.
        // If the BackgroundInfo's texture or texture path are invalid then a simple rectangle will
        // be drawn and filled with the given color.
        class BackgroundImage : public sf::Drawable
        {
        public:
            BackgroundImage(const BackgroundImage &) = delete;
            BackgroundImage(BackgroundImage &&) = delete;
            BackgroundImage & operator=(const BackgroundImage &) = delete;
            BackgroundImage & operator=(BackgroundImage &&) = delete;

        public:
            // if using this constructor then Setup() must be called before any other function
            BackgroundImage();

            explicit BackgroundImage(
                const BackgroundInfo & BG_INFO,
                const float IMAGE_SCALE = 1.0f,
                const bool WILL_SMOOTH_IMAGE = false);

            // use this constructor to create a background tile that is scaled per resolution like
            // is used on the main menu backgrounds
            explicit BackgroundImage(const std::string & MEDIA_PATH_KEY_STR);

            virtual ~BackgroundImage();

            void Setup(
                const BackgroundInfo & BG_INFO,
                const float SCALE = 1.0f,
                const bool WILL_SMOOTH_IMAGE = false);

            void Reset();

            // The only member required by sf::Drawable.
            virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

            virtual void SetColor(const sf::Color & NEW_COLOR);

            virtual void SetPosition(const float POS_LEFT, const float POS_TOP);
            virtual void MovePosition(const float HORIZ, const float VERT);

            virtual const sf::Color GetColor() const { return bgInfo_.color; }
            virtual const sf::FloatRect GetRegion() const { return bgInfo_.region; }
            virtual bool IsImage() const { return (sprite_.getTexture() != nullptr); }
            virtual bool IsRectangle() const { return !IsImage(); }

            void SetImageDetails(const float SCALE = 1.0f, const bool WILL_SMOOTH = false);

            const sfml_util::GradientInfo GradientInfo() const { return bgInfo_.gradient_info; }

            const BackgroundInfo BgInfo() const { return bgInfo_; }

            float ImageScale() const { return sprite_.getScale().x; }

            bool IsTextureSmoothed() const
            {
                if (false == bgInfo_.hasTexture)
                    return false;
                else
                    return true;
            }

        protected:
            GradientRect gradient_;
            sfml_util::gui::BackgroundInfo bgInfo_;
            sf::Sprite sprite_;
        };
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
