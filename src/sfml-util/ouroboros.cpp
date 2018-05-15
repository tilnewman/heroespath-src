// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// ouroboros.cpp
//
#include "ouroboros.hpp"

#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

namespace heroespath
{
namespace sfml_util
{

    Ouroboros::Ouroboros(const std::string & NAME, const bool WILL_INVERT)
        : GuiEntity(std::string(NAME).append("_Ouroboros"), sf::FloatRect())
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , rotation_(0.0f)
        , sprite_()
        , texture_()
        , sizeDrifter_(
              sfml_util::MapByRes(0.45f, 2.25f) - 0.1f,
              sfml_util::MapByRes(0.45f, 2.25f) + 0.1f,
              0.1,
              0.35,
              sfml_util::MapByRes(0.45f, 2.25f) - 0.1f,
              sfml_util::MapByRes(0.45f, 2.25f) + 0.1f)
        , shadeDrifter_(5.0f, 25.0f, 0.1, 0.75)
        , rotSpeedDrifter_(1.0f, 10.0f, 0.25, 0.75)
    {
        sfml_util::LoadTexture(
            texture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-ouroboros"));

        if (WILL_INVERT == false)
        {
            sfml_util::Invert(texture_);
        }

        sprite_.setTexture(texture_);
        sprite_.setScale(sfml_util::MapByRes(0.45f, 2.25f), sfml_util::MapByRes(0.45f, 2.25f));
        sprite_.setPosition(
            (SCREEN_WIDTH_ * 0.5f) - (sprite_.getGlobalBounds().width * 0.5f),
            (SCREEN_HEIGHT_ * 0.5f) - (sprite_.getGlobalBounds().height * 0.5f)
                + sfml_util::MapByRes(5.0f, 65.0f));
        sprite_.setColor(sf::Color(255, 255, 255, 20));
    }

    Ouroboros::~Ouroboros() = default;

    void Ouroboros::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite_, states);
    }

    bool Ouroboros::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        // of all the other transforms set the rotation first while still positioned at the origin
        // and orig scale
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);
        sprite_.setOrigin(
            sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);
        rotation_ += -1.0f * ELAPSED_TIME_SEC * rotSpeedDrifter_.Update(ELAPSED_TIME_SEC);
        sprite_.setRotation(rotation_);

        const float NEW_SCALE(sizeDrifter_.Update(ELAPSED_TIME_SEC));
        sprite_.setScale(NEW_SCALE, NEW_SCALE);

        const float POS_LEFT((SCREEN_WIDTH_ * 0.5f));
        const float POS_TOP((SCREEN_HEIGHT_ * 0.5f) + sfml_util::MapByRes(5.0f, 245.0f));
        sprite_.setPosition(POS_LEFT, POS_TOP);

        sprite_.setColor(sf::Color(
            255, 255, 255, static_cast<sf::Uint8>(shadeDrifter_.Update(ELAPSED_TIME_SEC))));

        return true;
    }
} // namespace sfml_util
} // namespace heroespath
