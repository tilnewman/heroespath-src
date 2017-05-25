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
#ifndef GAME_INNSTAGE_INCLUDED
#define GAME_INNSTAGE_INCLUDED
//
// inn-stage.hpp
//  A Stage class that allows starting the game
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/i-popup-callback.hpp"
#include "game/horiz-symbol.hpp"

#include <memory>


namespace sfml_util
{
    class SingleTextureAnimation;
    using SingleTextureAnimationSPtr_t = std::shared_ptr<SingleTextureAnimation>;
}
namespace game
{
    class Ouroboros;
    using OuroborosSPtr_t = std::shared_ptr<Ouroboros>;

namespace stage
{

    //A Stage class that allows starting the game
    class InnStage
    :
        public sfml_util::Stage
    {
        //prevent copy construction
        InnStage(const InnStage &) =delete;

        //prevent copy assignment
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
        sfml_util::SingleTextureAnimationSPtr_t candleAnimSPtr_;
        OuroborosSPtr_t ouroborosSPtr_;
        BottomSymbol bottomSymbol_;
    };

}
}
#endif //GAME_INNSTAGE_INCLUDED
