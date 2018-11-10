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

#include "map/shadow-masker.hpp"
#include "sfutil/color.hpp"
#include "sfutil/display.hpp"
#include "sfutil/image-manip.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <ostream>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace gui
{

    void ImageOptions::Apply(sf::Texture & texture) const
    {
        if (WillApplyTextureChanges())
        {
            texture.setSmooth(option_enum & ImageOpt::Smooth);
            texture.setRepeated(option_enum & ImageOpt::Repeated);
        }

        if (WillApplyImageChanges())
        {
            sf::Image image(texture.copyToImage());
            Apply(image);
            texture.loadFromImage(image);
        }
    }

    void ImageOptions::Apply(sf::Image & image) const
    {
        if (WillApplyImageChanges() == false)
        {
            return;
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) == 0))
        {
            sfutil::Invert(image, (option_enum & ImageOpt::InvertIncludesAlpha));
        }

        if (HasMask())
        {
            image.createMaskFromColor(mask_color_opt.value(), mask_alpha);
        }

        if (option_enum & (ImageOpt::ShadowMaskNormal | ImageOpt::ShadowMaskForShadowImage))
        {
            map::ShadowMasker::ChangeColors(
                image, (option_enum & ImageOpt::ShadowMaskForShadowImage));
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) > 0))
        {
            sfutil::Invert(image, (option_enum & ImageOpt::InvertIncludesAlpha));
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

    const std::string ImageOptions::ToString(
        const bool WILL_PREFIX, const Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        if (option_enum & ImageOpt::Smooth)
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Smooth";
        }

        if (option_enum & ImageOpt::Repeated)
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Repeated";
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) == 0))
        {
            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Invert"
               << ((option_enum & ImageOpt::InvertIncludesAlpha) ? "NoAlpha" : "");
        }

        if (HasMask())
        {
            const auto C { mask_color_opt.value() };

            ss << ((ss.str().empty()) ? "" : SEPARATOR) << "Mask" << int(C.r) << "-" << int(C.g)
               << "-" << int(C.b);

            if (C.a < 255)
            {
                ss << "-" << int(C.a);
            }

            if (IsMaskAlphaDefault() == false)
            {
                ss << SEPARATOR << "MaskAlpha" << unsigned(mask_alpha);
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

        const auto PARTS_STR { ((WILL_WRAP == Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("ImageOptions").append((WILL_WRAP == Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

    bool operator<(const ImageOptions & L, const ImageOptions & R)
    {
        return std::tie(L.option_enum, L.mask_color_opt, L.mask_alpha)
            < std::tie(R.option_enum, R.mask_color_opt, R.mask_alpha);
    }

    bool operator==(const ImageOptions & L, const ImageOptions & R)
    {
        return std::tie(L.option_enum, L.mask_color_opt, L.mask_alpha)
            == std::tie(R.option_enum, R.mask_color_opt, R.mask_alpha);
    }

} // namespace gui

std::ostream & operator<<(std::ostream & os, const gui::ImageOptions & OPTIONS)
{
    os << OPTIONS.ToString();
    return os;
}

} // namespace heroespath
