// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SHADOWMASKER_HPP_INCLUDED
#define HEROESPATH_SHADOWMASKER_HPP_INCLUDED
//
// shadow-masker.hpp
//
#include "map/layout.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include <string>

namespace heroespath
{
namespace map
{

    // Responsible for changing shadow place-holder colors into shades of gray.
    class ShadowMasker
    {
    public:
        ShadowMasker() = delete;
        ShadowMasker(const ShadowMasker &) = delete;
        ShadowMasker(ShadowMasker &&) = delete;
        ShadowMasker & operator=(const ShadowMasker &) = delete;
        ShadowMasker & operator=(ShadowMasker &&) = delete;

        static void ChangeColors(const std::string & XML_ATTRIB_NAME_SHADOWS, Layout & layout);
        static void ChangeColors(sf::Texture &, const bool IS_SHADOW_IMAGE);

    private:
        // The tileset I found online uses this color as a background,
        // so it needs to be changed to transparen.t
        static const sf::Color TRANSPARENT_MASK_;

        // the tileset I found online uses these magenta colors as shades for shadows
        static const sf::Color SHADOW_MASK1_;
        static const sf::Color SHADOW_MASK2_;
        static const sf::Color SHADOW_MASK2B_;
        static const sf::Color SHADOW_MASK3_;

        // these are the shades of semi-transparent black I chose for shadows
        static const sf::Color SHADOW_COLOR1_;
        static const sf::Color SHADOW_COLOR2_;
        static const sf::Color SHADOW_COLOR3_;

        static const unsigned COLOR_COMPONENT_COUNT_;
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_SHADOWMASKER_HPP_INCLUDED
