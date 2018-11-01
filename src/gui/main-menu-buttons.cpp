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
#include "gui/text-info.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"

#include <string>

namespace heroespath
{
namespace gui
{

    const float MainMenuButton::SCREEN_SIZE_RATIO_WIDTH_DEFAULT_(0.1f);

    MainMenuButton::MainMenuButton(
        const stage::Stage::Enum TRANSITION_TO,
        const ImageTextEntity::Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        const float FORCED_IMAGE_WIDTH,
        const sf::Vector2f & POS_V)
        : ImageTextEntity(
            "MainMenuButton_" + stage::Stage::ToString(TRANSITION_TO),
            MakeMouseImageInfo(TRANSITION_TO, POS_V, FORCED_IMAGE_WIDTH),
            MouseTextInfo(),
            CALLBACK_HANDLER_PTR_OPT,
            ImageTextEntity::MouseStateSync::Image)
        , transitionTo_(TRANSITION_TO)
    {}

    float MainMenuButton::DefaultWidth()
    {
        return sfutil::ScreenRatioToPixelsHoriz(SCREEN_SIZE_RATIO_WIDTH_DEFAULT_);
    }

    void MainMenuButton::OnClick(const sf::Vector2f & MOUSE_POS_V)
    {
        if (stage::Stage::IsPlayableAndNotPopup(transitionTo_))
        {
            game::GameController::Instance()->TransitionTo(transitionTo_);
        }

        ImageTextEntity::OnClick(MOUSE_POS_V);
    }

    const MouseImageInfo MainMenuButton::MakeMouseImageInfo(
        const stage::Stage::Enum TRANSITION_TO,
        const sf::Vector2f & POS_V,
        const float FORCED_IMAGE_WIDTH_ORIG)
    {
        const auto FORCED_IMAGE_WIDTH { (
            (FORCED_IMAGE_WIDTH_ORIG < 0.0f) ? DefaultWidth() : FORCED_IMAGE_WIDTH_ORIG) };

        const sf::FloatRect IMAGE_REGION(POS_V, sf::Vector2f(FORCED_IMAGE_WIDTH, 0.0f));

        const std::string IMAGE_PATH_KEY_PREFIX("media-images-buttons-mainmenu-");

        const auto LOOPSTATE_NAME { boost::algorithm::to_lower_copy(
            stage::Stage::ToString(TRANSITION_TO)) };

        MouseImageInfo mouseImageInfo(
            true,
            EntityImageInfo(
                CachedTexture(IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-normal"), IMAGE_REGION),
            EntityImageInfo(),
            EntityImageInfo(
                CachedTexture(IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-lit"), IMAGE_REGION));

        if (stage::Stage::HasFadedImage(TRANSITION_TO))
        {
            mouseImageInfo.disabled = EntityImageInfo(
                CachedTexture(IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-faded"), IMAGE_REGION);
        }

        return mouseImageInfo;
    }

} // namespace gui
} // namespace heroespath
