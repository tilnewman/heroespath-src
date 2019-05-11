// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main-menu-buttons.cpp
//
#include "main-menu-buttons.hpp"

#include "game/game-controller.hpp"
#include "gui/mouse-text-info.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "sfutil/display.hpp"

namespace heroespath
{
namespace gui
{

    const float MainMenuButton::DEFAULT_HEIGHT_SCREEN_SIZE_RATIO_ { 0.07f };
    const sf::Uint8 MainMenuButton::DISABLED_COLOR_ALPHA_ { 100 };

    MainMenuButton::MainMenuButton(
        const MenuImage::Enum MENU_IMAGE,
        const stage::Stage::Enum TRANSITION_TO,
        const ImageTextEntity::Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        const sf::Vector2f & POS_V,
        const bool WILL_TRANSITION_STAGE,
        const bool WILL_INCLUDE_DISABLED,
        const float HEIGHT_SCREEN_RATIO)
        : ImageTextEntity(
              "MainMenuButton_" + NAMEOF_ENUM_STR(TRANSITION_TO),
              MakeMouseImageInfo(MENU_IMAGE, WILL_INCLUDE_DISABLED, HEIGHT_SCREEN_RATIO, POS_V),
              MouseTextInfo(),
              CALLBACK_HANDLER_PTR_OPT,
              ImageTextEntity::MouseStateSync::Image)
        , transitionTo_(TRANSITION_TO)
        , willTransitionStage_(WILL_TRANSITION_STAGE)
    {}

    void MainMenuButton::OnClick(const sf::Vector2f & MOUSE_POS_V)
    {
        if (willTransitionStage_)
        {
            if (stage::Stage::IsPlayableAndNotPopup(transitionTo_))
            {
                game::GameController::Instance()->TransitionTo(transitionTo_);
            }
        }

        ImageTextEntity::OnClick(MOUSE_POS_V);
    }

    const MouseImageInfo MainMenuButton::MakeMouseImageInfo(
        const MenuImage::Enum MENU_IMAGE,
        const bool WILL_INCLUDE_DISABLED,
        const float HEIGHT_SCREEN_RATIO,
        const sf::Vector2f & POS_V)
    {
        const auto HEIGHT { sfutil::ScreenRatioToPixelsVert(HEIGHT_SCREEN_RATIO) };

        const sf::FloatRect IMAGE_REGION(POS_V, sf::Vector2f(0.0f, HEIGHT));

        const auto CONFIG_FILE_KEY_NORMAL { MenuImage::ConfigFileKey(MENU_IMAGE, false) };

        const auto TEXTURE_RECT_NORMAL { misc::ConfigFile::Instance()->GetTextureRect(
            CONFIG_FILE_KEY_NORMAL) };

        const auto CONFIG_FILE_KEY_LIT { MenuImage::ConfigFileKey(MENU_IMAGE, true) };

        const auto TEXTURE_RECT_LIT { misc::ConfigFile::Instance()->GetTextureRect(
            CONFIG_FILE_KEY_LIT) };

        MouseImageInfo mouseImageInfo(
            true,
            EntityImageInfo(
                CachedTexture(CONFIG_FILE_KEY_NORMAL), IMAGE_REGION, TEXTURE_RECT_NORMAL),
            EntityImageInfo(),
            EntityImageInfo(CachedTexture(CONFIG_FILE_KEY_LIT), IMAGE_REGION, TEXTURE_RECT_LIT));

        if (WILL_INCLUDE_DISABLED)
        {
            mouseImageInfo.disabled = EntityImageInfo(
                CachedTexture(CONFIG_FILE_KEY_NORMAL),
                IMAGE_REGION,
                TEXTURE_RECT_NORMAL,
                sf::Color(255, 255, 255, DISABLED_COLOR_ALPHA_));
        }

        return mouseImageInfo;
    }

} // namespace gui
} // namespace heroespath
