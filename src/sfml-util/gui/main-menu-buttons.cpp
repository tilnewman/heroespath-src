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

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/filesystem-helpers.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        MainMenuButton::MainMenuButton(
            const LoopState::Enum TRANSITION_TO,
            const callback::IFourStateButtonCallbackHandlerPtrOpt_t CALLBACK_HANDLER_PTR_OPT,
            const float SCALE,
            const sf::Vector2f & POS_V)
            : FourStateButton(
                  "MainMenuButton_" + LoopState::ToString(TRANSITION_TO),
                  POS_V,
                  MakeButtonStateImageKeys(TRANSITION_TO),
                  sfml_util::gui::MouseTextInfo(),
                  sfml_util::gui::TextInfo(),
                  false,
                  false,
                  sf::Color::White,
                  SCALE)
            , transitionTo_(TRANSITION_TO)
        {
            if (CALLBACK_HANDLER_PTR_OPT)
            {
                SetCallbackHandler(CALLBACK_HANDLER_PTR_OPT.value());
            }
        }

        void MainMenuButton::OnClick(const sf::Vector2f & MOUSE_POS_V)
        {
            if (LoopState::WillTransition(transitionTo_))
            {
                game::LoopManager::Instance()->TransitionTo(transitionTo_);
            }

            FourStateButton::OnClick(MOUSE_POS_V);
        }

        const ButtonStateImageKeys
            MainMenuButton::MakeButtonStateImageKeys(const LoopState::Enum TRANSITION_TO)
        {
            const std::string IMAGE_PATH_KEY_PREFIX("media-images-buttons-mainmenu-");

            auto const LOOPSTATE_NAME { boost::algorithm::to_lower_copy(
                LoopState::ToString(TRANSITION_TO)) };

            ButtonStateImageKeys keys(
                IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-normal",
                "",
                IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-lit");

            if (LoopState::HasFadedImage(TRANSITION_TO))
            {
                keys.disabled = IMAGE_PATH_KEY_PREFIX + LOOPSTATE_NAME + "-faded";
            }

            return keys;
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
