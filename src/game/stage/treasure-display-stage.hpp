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
#ifndef GAME_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
#define GAME_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
//
// treasure-display-stage.hpp
//
#include "sfml-util/stage.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/background-info.hpp"

#include "game/ouroboros.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"
#include "game/stage/treasure-stage-listbox-mover.hpp"
#include "game/item/treasure-image-enum.hpp"
#include "game/item/treasure-available-enum.hpp"
#include "game/item/item-cache.hpp"


namespace game
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}
namespace stage
{
namespace treasure
{

    //There are two listboxes.  On the left is the Treasure listbox which contains either the
    //items worn/held by defeated enemies or the contents of the lockbox carried by defeated
    //enemies.  On the right is the inventory listbox which contains the unequipped items
    //carried by one of the player's characters.
    enum class WhichListbox
    {
        Treasure,
        Inventory
    };


    //Responsible for calculating and wrapping colors used by TreasureDisplayStage.
    struct ListboxColors
    {
        ListboxColors();

        sf::Color image;
        sf::Color line;
        sf::Color foreground;
        sf::Color background;
        sf::Color title;
        sfml_util::gui::ColorSet colorSet;
    };


    //Responsible for calculating and wrapping positions used by TreasureDisplayStage.
    struct DisplayMeasurements
    {
        DisplayMeasurements(const float COINS_IMAGE_BOTTOM);

        float screenWidth;
        float screenHeight;
        float innerPad;
        sf::FloatRect innerRect;
        float creatureImageLeft;
        float creatureImageScale;
        float creatureImageHeight;
        float listboxHeightReduction;
        float listboxScreenEdgeMargin;
        float listboxBetweenSpacer;
        float listboxWidth;
        float treasureListboxLeft;
        float inventoryListboxLeft;
        float listboxTop;
        float listboxHeight;
        sf::FloatRect treasureListboxRegion;
        sf::FloatRect inventoryListboxRegion;
        float listboxMargin;
        float listboxItemSpacer;
    };
}


    class TreasureStage;


    //Responsible for the display of treasure collection graphics (images, listboxes, etc.)
    //for the Treasure Stage.
    class TreasureDisplayStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IListBoxCallbackHandler
    {
        TreasureDisplayStage(const TreasureDisplayStage &) =delete;
        TreasureDisplayStage & operator=(const TreasureDisplayStage &) =delete;

    public:
        explicit TreasureDisplayStage(TreasureStage *);

        inline const std::string HandlerName() const override { return GetStageName(); }
        bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        
        void SetupInitial();
        void SetupAfterPleaseWait(const item::TreasureImage::Enum);

        void UpdateTreasureImage(const item::TreasureImage::Enum);

        void SetupForCollection(
            const item::TreasureAvailable::Enum,
            const item::ItemCache & HELD_CACHE,
            const item::ItemCache & LOCKBOX_CACHE);

        bool IsShowingHeldItems() const;
        std::size_t CharacterIndexShowingInventory() const;
        bool IsAnythingAnimating() const;

    private:
        void SetupInitial_BackgroundImage();
        void SetupInitial_Ouroboros();

        void SetupAfterPleaseWait_CorpseImage();
        void SetupAfterPleaseWait_TreasureImage(const item::TreasureImage::Enum);
        void SetupAfterPleaseWait_CoinsImage();

        const std::string CorpseImageKeyFromEnemyParty() const;
        
        item::ItemPVec_t WhichTreasureItemsAreDisplayed(
            const treasure::ListboxMoverUPtr_t & LISTBOX_MOVER_UPTR,
            const item::ItemCache & HELD_CACHE,
            const item::ItemCache & LOCKBOX_CACHE) const;
        
        creature::CreaturePtr_t WhichCharacterInventoryIsDisplayed(
            const treasure::ListboxMoverUPtr_t & LISTBOX_MOVER_UPTR);

        void SetupListbox(
            const treasure::WhichListbox WHICH_LISTBOX,
            sfml_util::gui::ListBoxUPtr_t & listboxUPtr,
            const item::ItemPVec_t &);

    private:
        TreasureStage * treasureStagePtr_;
        MainMenuTitle titleImage_;
        BottomSymbol bottomImage_;
        OuroborosUPtr_t ouroborosUPtr_;
        treasure::ListboxMoverUPtr_t listboxMoverUPtr_;
        sfml_util::gui::ListBoxUPtr_t treasureListboxUPtr_;
        sfml_util::gui::ListBoxUPtr_t inventoryListboxUPtr_;

        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sf::Texture corpseTexture_;
        sf::Sprite corpseSprite_;
        sf::Texture treasureTexture_;
        sf::Sprite treasureSprite_;
        sf::Texture coinsTexture_;
        sf::Sprite coinsSprite_;
        //sf::Texture characterTexture_;
        //sf::Sprite characterSprite_;
        item::TreasureAvailable::Enum treasureAvailable_;
    };

}
}


#endif //GAME_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
