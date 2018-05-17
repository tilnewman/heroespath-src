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
#include "sfml-util/loaders.hpp"

namespace heroespath
{
namespace avatar
{

    void PortraitFactory::Make(const Avatar::Enum WHICH_AVATAR, sf::Texture & finalTexture)
    {
        sf::Texture charTexture;
        sfml_util::Loaders::Texture(charTexture, Avatar::ImagePath(WHICH_AVATAR));

        auto const WIDTH{ 64 };
        auto const HEIGHT{ WIDTH };
        sf::Sprite charSprite(charTexture, sf::IntRect(0, 192, WIDTH, HEIGHT));

        sf::RenderTexture renderTexture;
        renderTexture.create(WIDTH, HEIGHT);
        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(charSprite);
        renderTexture.display();

        finalTexture = renderTexture.getTexture();
    }
} // namespace avatar
} // namespace heroespath
