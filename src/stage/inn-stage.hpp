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
#ifndef HEROESPATH_INNSTAGE_HPP_INCLUDED
#define HEROESPATH_INNSTAGE_HPP_INCLUDED
//
// inn-stage.hpp
//  A Stage class that allows starting the game
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "sfml-util/main-menu-buttons.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/horiz-symbol.hpp"

#include <memory>


namespace sfml_util
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;
}

namespace heroespath
{
namespace stage
{

    //A Stage class that allows starting the game
    class InnStage
    :
        public sfml_util::Stage
    {
        InnStage(const InnStage &) =delete;
        InnStage & operator=(const InnStage &) =delete;

    public:
        InnStage();
        virtual ~InnStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sf::Texture titleSymbolTexture_;
        sf::Sprite  titleSymbolSprite_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sf::Texture candleTexture_;
        sf::Sprite  candleSprite_;
        sfml_util::AnimationUPtr_t candleAnimUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
    };

}
}
#endif //HEROESPATH_INNSTAGE_HPP_INCLUDED
