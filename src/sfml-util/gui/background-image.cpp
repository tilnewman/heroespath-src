// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// background-image.cpp
//
#include "background-image.hpp"

#include "misc/random.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/game-data-file.hpp"
#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        BackgroundImage::BackgroundImage()
            : gradient_()
            , bgInfo_()
            , sprite_()
        {}

        BackgroundImage::BackgroundImage(
            const sfml_util::gui::BackgroundInfo & BG_INFO,
            const float IMAGE_SCALE,
            const bool WILL_SMOOTH_IMAGE)
            : gradient_()
            , bgInfo_(BG_INFO)
            , sprite_()
        {
            Setup(bgInfo_, IMAGE_SCALE, WILL_SMOOTH_IMAGE);
        }

        BackgroundImage::BackgroundImage(const std::string & MEDIA_PATH_KEY_STR)
            : gradient_()
            , bgInfo_()
            , sprite_()
        {
            auto const FULLSCREEN_RECT{ sfml_util::Display::Instance()->FullScreenRect() };

            const sfml_util::GradientInfo GRADIENT_INFO(
                sf::Color(0, 0, 0, 200),
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

            gradient_.Setup(FULLSCREEN_RECT, GRADIENT_INFO);

            bgInfo_ = sfml_util::gui::BackgroundInfo(
                game::GameDataFile::Instance()->GetMediaPath(MEDIA_PATH_KEY_STR),
                FULLSCREEN_RECT,
                sf::Color::White,
                GRADIENT_INFO);

            Setup(bgInfo_, sfml_util::MapByRes(1.0f, 4.0f), true);
        }

        BackgroundImage::~BackgroundImage() = default;

        void BackgroundImage::Setup(
            const sfml_util::gui::BackgroundInfo & BG_INFO,
            const float IMAGE_SCALE,
            const bool WILL_SMOOTH_IMAGE)
        {
            // set the internal states even if they are not valid
            bgInfo_ = BG_INFO;

            // setup the gradient
            gradient_.Setup(bgInfo_.region, BG_INFO.gradient_info);

            // can't create a background image or rectangle with a dimmension less than 1
            if (bgInfo_.IsValid() == false)
            {
                return;
            }

            // can't create an image with no texture and no path to a texture
            // if no texture and no path are given, then only draw a filled
            // rectangle (with the gradient on top)
            if ((false == bgInfo_.hasTexture) && (bgInfo_.path.empty() == true))
            {
                return;
            }

            // load the texture if not loaded already
            if (false == bgInfo_.hasTexture)
            {
                sfml_util::Loaders::Texture(bgInfo_.texture, bgInfo_.path);
            }

            // set to repeated in case the region is bigger than the texture (if we need to tile)
            bgInfo_.texture.setRepeated(true);

            // if the background image is bigger than the size needed (tiling),
            // use a random region from within that image.
            float textRectLeftToUse(0.0f);
            float textRectTopToUse(0.0f);

            auto const WIDTH_DIFF{ static_cast<float>(bgInfo_.texture.getSize().x)
                                   - bgInfo_.region.width };

            if (WIDTH_DIFF > 10.0f)
            {
                textRectLeftToUse = misc::random::Float(0.0f, WIDTH_DIFF);
            }

            auto const HEIGHT_DIFF{ static_cast<float>(bgInfo_.texture.getSize().y)
                                    - bgInfo_.region.height };

            if (HEIGHT_DIFF > 10.0f)
            {
                textRectTopToUse = misc::random::Float(0.0f, HEIGHT_DIFF);
            }

            auto const SCALE_MULT{ 1.0f / IMAGE_SCALE };

            const sf::FloatRect TEXTURE_RECT(
                textRectLeftToUse,
                textRectTopToUse,
                bgInfo_.region.width * SCALE_MULT,
                bgInfo_.region.height * SCALE_MULT);

            // setup the sprite
            sprite_.setTexture(bgInfo_.texture);
            sprite_.setTextureRect(sfml_util::ConvertRect<float, int>(TEXTURE_RECT));
            sprite_.setPosition(bgInfo_.region.left, bgInfo_.region.top);
            sprite_.setColor(bgInfo_.color);
            SetImageDetails(IMAGE_SCALE, WILL_SMOOTH_IMAGE);
        }

        void BackgroundImage::Reset()
        {
            auto newBgInfo{ bgInfo_ };

            newBgInfo.region = sf::FloatRect(
                0.0f,
                0.0f,
                sfml_util::Display::Instance()->GetWinWidth(),
                sfml_util::Display::Instance()->GetWinHeight());

            Setup(newBgInfo, sfml_util::MapByRes(1.0f, 4.0f), IsTextureSmoothed());
        }

        void BackgroundImage::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (IsImage())
            {
                target.draw(sprite_, states);
            }
            else
            {
                sfml_util::DrawRectangle<float>(
                    target, states, bgInfo_.region, bgInfo_.color, 0, bgInfo_.color);
            }

            gradient_.draw(target, states);
        }

        void BackgroundImage::SetColor(const sf::Color & NEW_COLOR)
        {
            bgInfo_.color = NEW_COLOR;
            sprite_.setColor(NEW_COLOR);
        }

        void BackgroundImage::SetPosition(const float POS_LEFT, const float POS_TOP)
        {
            sprite_.setPosition(POS_LEFT, POS_TOP);

            bgInfo_.region.left = POS_LEFT;
            bgInfo_.region.top = POS_TOP;

            gradient_.SetPosition(POS_LEFT, POS_TOP);
        }

        void BackgroundImage::MovePosition(const float HORIZ, const float VERT)
        {
            sprite_.move(HORIZ, VERT);

            bgInfo_.region.left += HORIZ;
            bgInfo_.region.top += VERT;

            gradient_.MovePosition(HORIZ, VERT);
        }

        void BackgroundImage::SetImageDetails(const float SCALE, const bool WILL_SMOOTH)
        {
            sprite_.setScale(SCALE, SCALE);

            if (bgInfo_.hasTexture)
            {
                bgInfo_.texture.setSmooth(WILL_SMOOTH);
            }
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
