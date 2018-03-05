// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// gui-elements.cpp
//
#include "gui-elements.hpp"

#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"

#include "sfml-util/loaders.hpp"

#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        std::unique_ptr<GuiElements> GuiElements::instanceUPtr_{ nullptr };

        GuiElements::GuiElements()
            : elementsTexture_()
            , woodBgTexture_()
            , darkKnotBgTexture_()
            , radioButtonBrightOffSpriteRect_(sf::IntRect(0, 0, 20, 20))
            , radioButtonBrightOnSpriteRect_(sf::IntRect(20, 0, 20, 20))
            , radioButtonMedOffSpriteRect_(sf::IntRect(40, 0, 20, 20))
            , radioButtonMedOnSpriteRect_(sf::IntRect(60, 0, 20, 20))
            , radioButtonDarkOffSpriteRect_(sf::IntRect(80, 0, 20, 20))
            , radioButtonDarkOnSpriteRect_(sf::IntRect(100, 0, 20, 20))
            , checkBoxMedOffSpriteRect_(sf::IntRect(120, 0, 26, 23))
            , checkBoxMedOnSpriteRect_(sf::IntRect(146, 0, 26, 23))
            , checkBoxBrightOffSpriteRect_(sf::IntRect(172, 0, 26, 23))
            , checkBoxBrightOnSpriteRect_(sf::IntRect(198, 0, 26, 23))
            , checkBoxDarkOffSpriteRect_(sf::IntRect(224, 0, 26, 23))
            , checkBoxDarkOnSpriteRect_(sf::IntRect(0, 23, 26, 23))
            , arrowMinusLeftMedSpriteRect_(sf::IntRect(26, 23, 27, 28))
            , arrowPlusRightMedSpriteRect_(sf::IntRect(53, 23, 27, 28))
            , arrowMinusLeftBrightSpriteRect_(sf::IntRect(80, 23, 27, 28))
            , arrowPlusRightBrightSpriteRect_(sf::IntRect(107, 23, 27, 28))
            , arrowMinusLeftDarkSpriteRect_(sf::IntRect(134, 23, 27, 28))
            , arrowPlusRightDarkSpriteRect_(sf::IntRect(161, 23, 27, 28))
            , arrowMinusDownBrightSpriteRect_(sf::IntRect(188, 23, 27, 27))
            , arrowPlusUpBrightSpriteRect_(sf::IntRect(215, 23, 27, 27))
            , arrowMinusDownMedSpriteRect_(sf::IntRect(0, 51, 27, 27))
            , arrowPlusUpMedSpriteRect_(sf::IntRect(27, 51, 27, 27))
            , arrowMinusDownDarkSpriteRect_(sf::IntRect(54, 51, 27, 27))
            , arrowPlusUpDarkSpriteRect_(sf::IntRect(81, 51, 27, 27))
            , arrowLeftMedSpriteRect_(sf::IntRect(108, 51, 27, 27))
            , arrowRightMedSpriteRect_(sf::IntRect(135, 51, 27, 27))
            , arrowDownMedSpriteRect_(sf::IntRect(162, 51, 27, 27))
            , arrowUpMedSpriteRect_(sf::IntRect(189, 51, 27, 27))
            , arrowLeftBrightSpriteRect_(sf::IntRect(216, 51, 27, 27))
            , arrowRightBrightSpriteRect_(sf::IntRect(0, 78, 27, 27))
            , arrowDownBrightSpriteRect_(sf::IntRect(27, 78, 27, 27))
            , arrowUpBrightSpriteRect_(sf::IntRect(54, 78, 27, 27))
            , arrowLeftDarkSpriteRect_(sf::IntRect(81, 78, 27, 27))
            , arrowRightDarkSpriteRect_(sf::IntRect(108, 78, 27, 27))
            , arrowDownDarkSpriteRect_(sf::IntRect(135, 78, 27, 27))
            , arrowUpDarkSpriteRect_(sf::IntRect(162, 78, 27, 27))
            , arrowLargeLeftSpriteRect_(sf::IntRect(189, 78, 36, 36))
            , arrowLargeUpSpriteRect_(sf::IntRect(0, 114, 36, 36))
            , arrowLargeRightSpriteRect_(sf::IntRect(36, 114, 36, 36))
            , arrowLargeDownSpriteRect_(sf::IntRect(72, 114, 36, 36))
            , arrowSmallLeftSpriteRect_(sf::IntRect(108, 114, 28, 28))
            , arrowSmallUpSpriteRect_(sf::IntRect(136, 114, 28, 28))
            , arrowSmallRightSpriteRect_(sf::IntRect(164, 114, 28, 28))
            , arrowSmallDownSpriteRect_(sf::IntRect(192, 114, 28, 28))
            , boxSmallSpriteRect_(sf::IntRect(0, 150, 42, 42))
            , padVerticalSmallSpriteRect_(sf::IntRect(42, 150, 17, 28))
            , padVerticalLargeSpriteRect_(sf::IntRect(59, 150, 22, 39))
            , padHorizontalLargeSpriteRect_(sf::IntRect(81, 150, 40, 21))
            , horizontalLineLargeSpriteRect_(sf::IntRect(0, 192, 204, 15))
            , horizontalLineSmallSpriteRect_(sf::IntRect(0, 207, 201, 9))
            , lineSmallVerticalLeftSpriteRect_(sf::IntRect(228, 81, 12, 59))
            , lineSmallVerticalRightSpriteRect_(sf::IntRect(242, 81, 12, 59))
            , lineSmallEndLeftSpriteRect_(sf::IntRect(83, 174, 14, 12))
            , lineSmallEndRightSpriteRect_(sf::IntRect(98, 174, 14, 12))
            , lineSmallEndTopSpriteRect_(sf::IntRect(243, 50, 12, 14))
            , lineSmallEndBotSpriteRect_(sf::IntRect(243, 65, 12, 14))
            , lineSmallCornerTopLeftSpriteRect_(sf::IntRect(0, 217, 19, 19))
            , lineSmallCornerTopRightSpriteRect_(sf::IntRect(20, 217, 19, 19))
            , lineSmallCornerBotLeftSpriteRect_(sf::IntRect(0, 237, 19, 19))
            , lineSmallCornerBotRightSpriteRect_(sf::IntRect(20, 237, 19, 19))
            , lineSmallHorizontalTopSpriteRect_(sf::IntRect(122, 144, 74, 12))
            , lineSmallHorizontalBotSpriteRect_(sf::IntRect(122, 156, 74, 12))
            , lineSmallCrossSpriteRect_(sf::IntRect(60, 223, 26, 26))
            , lineSmallTTopSpriteRect_(sf::IntRect(87, 230, 26, 19))
            , lineSmallTLeftSpriteRect_(sf::IntRect(40, 223, 19, 26))
            , lineSmallTBotSpriteRect_(sf::IntRect(114, 223, 26, 19))
            , lineSmallTRightSpriteRect_(sf::IntRect(141, 223, 19, 26))
        {
            M_HP_LOG_DBG("Singleton Construction: GuiElements");

            sfml_util::LoadTexture(
                elementsTexture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-gui-elements"));

            sfml_util::LoadTexture(
                woodBgTexture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-wood"));

            sfml_util::LoadTexture(
                darkKnotBgTexture_,
                game::GameDataFile::Instance()->GetMediaPath(
                    "media-images-backgrounds-tile-darkknot"));
        }

        GuiElements::~GuiElements() { M_HP_LOG_DBG("Singleton Destruction: GuiElements"); }

        GuiElements * GuiElements::Instance()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                M_HP_LOG_WRN("Singleton Instance() before Acquire(): GuiElements");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void GuiElements::Acquire()
        {
            if (instanceUPtr_.get() == nullptr)
            {
                instanceUPtr_ = std::make_unique<GuiElements>();
            }
            else
            {
                M_HP_LOG_WRN("Singleton Acquire() after Construction: GuiElements");
            }
        }

        void GuiElements::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_.get() != nullptr),
                "sfml_util::gui::GuiElements::Release() found instanceUPtr that was null.");

            instanceUPtr_.reset();
        }
    }
}
}
