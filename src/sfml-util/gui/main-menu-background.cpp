// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main-menu-background.cpp
//
#include "main-menu-background.hpp"

#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/sfml-util-display.hpp"

namespace heroespath
{
namespace sfml_util
{

    MainMenuBackground::MainMenuBackground()
        : boxUPtr_()
    {
        sfml_util::BoxEntityInfo backgroundBoxInfo;

        backgroundBoxInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-darkknot",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.075f));

        backgroundBoxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 200),
            sfml_util::Side::None,
            sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        boxUPtr_ = std::make_unique<BoxEntity>(
            "MainMenuBackground_", sfml_util::DisplayRect(), backgroundBoxInfo);
    }

    void MainMenuBackground::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*boxUPtr_, states);
    }

} // namespace sfml_util
} // namespace heroespath
