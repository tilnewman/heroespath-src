// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-options.cpp
//
#include "image-options.hpp"

#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util-image-manip.hpp"

#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    void ImageOptions::Apply(sf::Texture & texture) const
    {
        texture.setSmooth(option_enum & ImageOpt::Smooth);

        if (BitClearCopy(option_enum, ImageOpt::Smooth) > 0)
        {
            sf::Image image(texture.copyToImage());
            Apply(image);
            texture.loadFromImage(image);
        }
    }

    void ImageOptions::Apply(sf::Image & image) const
    {
        if (BitClearCopy(option_enum, ImageOpt::Smooth) > 0)
        {
            if ((option_enum & ImageOpt::Invert)
                && ((option_enum & ImageOpt::InvertAfterMask) == 0))
            {
                sfml_util::Invert(image, (option_enum & ImageOpt::InvertIncludesAlpha));
            }

            if (HasMask())
            {
                image.createMaskFromColor(mask_color, mask_alpha);
            }

            if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) > 0))
            {
                sfml_util::Invert(image, (option_enum & ImageOpt::InvertIncludesAlpha));
            }

            if (option_enum & ImageOpt::FlipHoriz)
            {
                image.flipHorizontally();
            }

            if (option_enum & ImageOpt::FlipVert)
            {
                image.flipVertically();
            }
        }
    }

    const std::string ImageOptions::ToString(
        const bool WILL_PREFIX, const misc::Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        if (option_enum & ImageOpt::Smooth)
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Smooth";
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) == 0))
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Invert"
               << ((option_enum & ImageOpt::InvertIncludesAlpha) ? "NoAlpha" : "");
        }

        if (HasMask())
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Mask";

            if (IsMaskAlphaDefault() == false)
            {
                ss << "NewAlpha" << unsigned(mask_alpha);
            }
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) > 0))
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Invert"
               << ((option_enum & ImageOpt::InvertIncludesAlpha) ? "ExcludingAlpha" : "");
        }

        if (option_enum & ImageOpt::FlipHoriz)
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "FlipHoriz";
        }

        if (option_enum & ImageOpt::FlipVert)
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "FlipVert";
        }

        auto const PARTS_STR { (
            (WILL_WRAP == misc::Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("ImageOptions").append((WILL_WRAP == misc::Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

} // namespace sfml_util
} // namespace heroespath
