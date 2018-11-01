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

#include "sfml-util/box-entity.hpp"
#include "sfutil/display.hpp"

namespace heroespath
{
namespace gui
{

    MainMenuBackground::MainMenuBackground()
        : boxUPtr_()
    {
        gui::BoxEntityInfo backgroundBoxInfo;

        backgroundBoxInfo.SetupImage(
            gui::CachedTexture(
                "media-images-backgrounds-tile-darkknot",
                gui::ImageOpt::Default | gui::ImageOpt::Repeated),
            sfutil::ScreenRatioToPixelsHoriz(0.075f));

        backgroundBoxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 200),
            gui::Side::None,
            gui::Corner::TopLeft | gui::Corner::BottomRight);

        boxUPtr_ = std::make_unique<BoxEntity>(
            "MainMenuBackground_", sfutil::DisplayRect(), backgroundBoxInfo);
    }

    void MainMenuBackground::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*boxUPtr_, states);
    }

} // namespace gui
} // namespace heroespath
