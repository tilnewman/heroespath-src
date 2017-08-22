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
#ifndef GAME_TREASURESTAGE_INCLUDED
#define GAME_TREASURESTAGE_INCLUDED
//
// treasure-stage.hpp
//  A Stage class that allows finding treasure and advancing after combat.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include "game/i-popup-callback.hpp"
#include "game/creature/race-enum.hpp"
#include "game/combat/treasure-image-enum.hpp"

#include "misc/handy-types.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
}
}
namespace game
{
namespace stage
{

    //A Stage class that allows starting the game
    class TreasureStage
    :
        public sfml_util::Stage
    {
        //prevent copy construction
        TreasureStage(const TreasureStage &) =delete;

        //prevent copy assignment
        TreasureStage & operator=(const TreasureStage &) =delete;

        enum PhaseType
        {
            InitialSetup = 0,
            AwardExperience,
            AwardTitles,
            WaitWithoutTreasure,
            WaitWithTreasure,
            LockPickerSelection,
            Picking,
            Trap,
            Open,
            TakeSelection,
            Count
        };

    public:
        TreasureStage();
        virtual ~TreasureStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

    private:
        void SetupTreasureImage(const combat::TreasureImage::Enum);
        const std::string GetCorpseImageKeyFromEnemyParty() const;
        const misc::StrVec_t GetCorpseImageKeyFromRace(const creature::race::Enum) const;
        combat::TreasureImage::Enum GetInitialTreasureImageFromEnemyParty() const;
        
    private:
        //PhaseType   phase_;
        sf::Texture bgTexture_;
        sf::Sprite  bgSprite_;
        sf::Texture corpseTexture_;
        sf::Sprite  corpseSprite_;
        sf::Texture treasureTexture_;
        sf::Sprite  treasureSprite_;
        sf::Texture coinsTexture_;
        sf::Sprite  coinsSprite_;
        sfml_util::gui::TextRegionUPtr_t blurbTextRegionUPtr_;
    };

}
}
#endif //GAME_TREASURESTAGE_INCLUDED
