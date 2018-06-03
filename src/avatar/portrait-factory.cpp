// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// portrait-factory.hpp
//
#include "portrait-factory.hpp"
#include "avatar/lpc-view.hpp"
#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace avatar
{

    void PortraitFactory::Make(const Avatar::Enum WHICH_AVATAR, sf::Texture & finalTexture)
    {
        sf::Texture texture;
        sfml_util::Loaders::Texture(texture, Avatar::ImagePath(WHICH_AVATAR));

        sf::Sprite sprite(texture, LPCView::GetStandingRightFrameRect());

        const sf::Vector2u SIZE_V(
            static_cast<unsigned>(sprite.getLocalBounds().width),
            static_cast<unsigned>(sprite.getLocalBounds().height));

        sf::RenderTexture renderTexture;
        renderTexture.create(SIZE_V.x, SIZE_V.y);
        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(sprite);
        renderTexture.display();

        finalTexture = renderTexture.getTexture();
    }
} // namespace avatar
} // namespace heroespath
