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
#ifndef GAME_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
#define GAME_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
//
// adventure-display-stage.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"

#include "game/stage/adventure-stage-char-list.hpp"


namespace game
{
namespace stage
{

    class AdventureStage;


    //Responsible for all drawin operations of the AdventureStage.
    class AdventureDisplayStage : public sfml_util::Stage
    {
        AdventureDisplayStage(const AdventureDisplayStage &) = delete;
        AdventureDisplayStage & operator=(const AdventureDisplayStage &) = delete;

    public:
        AdventureDisplayStage(AdventureStage * const);
        virtual ~AdventureDisplayStage();

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;

    private:
        void Setup_CharacterList();
        void Setup_BackgroundImage();

    private:
        //AdventureStage * const adventureStagePtr_;
        AdventureCharacterListUPtr_t characterListUPtr_;
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
    };

}
}

#endif //GAME_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
