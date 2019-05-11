// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
        std::string str;
        str.reserve(64);

        if (WILL_PREFIX)
        {
            str += std::string("ImageOptions").append((WILL_WRAP == Wrap::Yes) ? "" : "=");
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            str += '(';
        }

        auto separatorIf = [&]() -> std::string { return ((str.empty()) ? "" : SEPARATOR); };

        if (option_enum & ImageOpt::Smooth)
        {
            str += separatorIf();
            str += "Smooth";
        }

        if (option_enum & ImageOpt::Repeated)
        {
            str += separatorIf();
            str += "Repeated";
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) == 0))
        {
            str += separatorIf();
            str += "Invert";
            str += ((option_enum & ImageOpt::InvertIncludesAlpha) ? "NoAlpha" : "");
        }

        if (HasMask())
        {
            const auto C { mask_color_opt.value() };

            str += separatorIf();
            str += "Mask";
            str += std::to_string(int(C.r));
            str += '-';
            str += std::to_string(int(C.g));
            str += '-';
            str += std::to_string(int(C.b));

            if (C.a < 255)
            {
                str += '-';
                str += std::to_string(int(C.a));
            }

            if (IsMaskAlphaDefault() == false)
            {
                str += SEPARATOR;
                str += "MaskAlpha";
                str += std::to_string(unsigned(mask_alpha));
            }
        }

        if ((option_enum & ImageOpt::Invert) && ((option_enum & ImageOpt::InvertAfterMask) > 0))
        {
            str += separatorIf();
            str += "Invert";
            str += ((option_enum & ImageOpt::InvertIncludesAlpha) ? "ExcludingAlpha" : "");
        }

        if (option_enum & ImageOpt::FlipHoriz)
        {
            str += separatorIf();
            str += "FlipHoriz";
        }

        if (option_enum & ImageOpt::FlipVert)
        {
            str += separatorIf();
            str += "FlipVert";
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            str += ')';
        }

        return str;
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
