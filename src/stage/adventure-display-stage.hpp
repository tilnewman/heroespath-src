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
#ifndef HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
//
// adventure-display-stage.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/gui/picture-frame.hpp"

#include "stage/adventure-stage-char-list.hpp"

#include <memory>


namespace heroespath
{
namespace map
{
    class Map;
    using MapUPtr_t = std::unique_ptr<Map>;
}
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

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    private:
        void Setup_CharacterList();
        void Setup_BackgroundImage();
        void Setup_Map();

        void HandleMovementKeypresses(
            const sfml_util::Direction::Enum,
            bool & wasPressed,
            const bool IS_PRESSED);

    private:
        static const float TIME_BETWEEN_MAP_MOVES_SEC_;
        
        AdventureCharacterListUPtr_t characterListUPtr_;
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sfml_util::BottomSymbol bottomImage_;
        sfml_util::MainMenuTitle topImage_;
        map::MapUPtr_t mapUPtr_;
        sfml_util::gui::PictureFrame mapFrame_;
        float moveTimerSec_;
        bool wasPressedLeft_;
        bool wasPressedRight_;
        bool wasPressedUp_;
        bool wasPressedDown_;
    };

}
}

#endif //HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
