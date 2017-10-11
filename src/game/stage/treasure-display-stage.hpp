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
#include "sfml-util/gui/gui-entity-image.hpp"

#include "game/ouroboros.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"
#include "game/stage/item-detail-viewer.hpp"
#include "game/stage/treasure-stage-mover.hpp"
#include "game/item/treasure-image-enum.hpp"
#include "game/item/treasure-available-enum.hpp"
#include "game/item/item-cache.hpp"

#include <memory>


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
        DisplayMeasurements(
            const float COINS_IMAGE_BOTTOM,
            const float BOTTOM_SYMBOL_HEIGHT);

        float screenWidth;
        float screenHeight;
        float innerPad;
        sf::FloatRect innerRect;
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
        float characterImageLeft;
        float characterImageScale;
    };


    //Responsible for wrapping all the info needed to display item details.
    struct ItemDetails
    {
        ItemDetails(
            const sf::FloatRect & RECT = sfml_util::gui::ListBox::ERROR_RECT_,
            const item::ItemPtr_t ITEM_PTR = nullptr)
        :
            rect(RECT),
            item_ptr(ITEM_PTR)
        {}

        inline bool IsValid() const
        {
            return (
                (sfml_util::gui::ListBox::ERROR_RECT_ != rect) &&
                (nullptr != item_ptr) );
        }

        sf::FloatRect rect;
        item::ItemPtr_t item_ptr;
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
        bool KeyRelease(const sf::Event::KeyEvent &) override;
        void UpdateMousePos(const sf::Vector2i &) override;
        void UpdateMouseDown(const sf::Vector2f &) override;

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void UpdateTime_StageMover(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_ItemDetailViewer(const float ELAPSED_TIME_SECONDS);

        void SetupInitial();

        void SetupAfterPleaseWait(const item::TreasureImage::Enum);

        void SetupForCollection(
            const item::TreasureAvailable::Enum,
            const item::TreasureImage::Enum);

        void UpdateTreasureImage(const item::TreasureImage::Enum);

        bool IsShowingHeldItems() const;
        std::size_t CharacterIndexShowingInventory() const;
        bool IsAnythingAnimating() const;

        bool CanTreasureChange() const;
        void TreasureChange() const;

        void UpdateItemCaches(
            const item::ItemCache & HELD_CACHE,
            const item::ItemCache & LOCKBOX_CACHE);

        std::size_t CharacterIndex() const;
        std::size_t CharacterIndexMax() const;
        
        void InventoryChange(const std::size_t);

        inline bool IsItemDetailMovingOrShowing() const
        {
            return itemDetailViewer_.IsMovingOrShowing();
        }

        std::size_t WhichCharacterInventoryIsDisplayedIndex();

        creature::CreaturePtr_t WhichCharacterInventoryIsDisplayed();

        void RefreshAfterCacheUpdate();

        inline void CanDisplayItemDetail(const bool CAN) { canDisplayItemDetail_ = CAN; }

    private:
        treasure::DisplayMeasurements CreateDisplayMeasurements() const;

        void SetupInitial_BackgroundImage();
        void SetupInitial_Ouroboros();

        void SetupAfterPleaseWait_CorpseImage();
        void SetupAfterPleaseWait_TreasureImage(const item::TreasureImage::Enum);
        void SetupAfterPleaseWait_CoinsImage();

        const std::string CorpseImageKeyFromEnemyParty() const;
        
        void SetupForCollection_TreasureListbox(const stage::treasure::Type);
        void SetupForCollection_InventoryListbox();
        void SetupForCollection_TreasureListboxLabel();
        void SetupForCollection_InventoryCharacterImage();
        void SetupForCollection_InventoryListboxLabel();
        void SetupForCollection_InventoryCoinsText();
        void SetupForCollection_InventoryGemsText();
        void SetupForCollection_InventoryWeightText();
        void SetupForCollection_InventoryRedXImage();
        void SetupForCollection_InstructionsText();

        void SetupListbox(
            const treasure::WhichListbox WHICH_LISTBOX,
            sfml_util::gui::ListBoxUPtr_t & listboxUPtr,
            const item::ItemPVec_t &);

        void SetupTreasure_ListboxLabel();
        void SetupInventory_CharacterImage();
        void SetupInventory_ListboxLabel();
        void SetupInventory_CoinsText();
        void SetupInventory_GemsText();
        void SetupInventory_WeightText();
        void SetupInventory_RedXImage();
        
        stage::treasure::Type TreasureSource() const;

        void UpdateTreasureVisuals();
        void UpdateInventoryVisuals();

        void SetupInventoryText(
            sfml_util::gui::TextRegionUPtr_t & textRegionUPtr,
            const std::string & NAME,
            const std::string & TEXT,
            const float HORIZ_POS,
            const float VERT_POS,
            const unsigned int FONT_SIZE);

        inline float CalculateHorizOffscreenPos() const
        {
            return CreateDisplayMeasurements().screenWidth + 1.0f;
        }

        float CalculateInventoryTextPosLeft() const;

        float CalculateInventoryTextVertShift() const
        {
            return sfml_util::MapByRes(10.0f, 70.0f);
        }

        void ItemViewerInterruption();

        const treasure::ItemDetails MouseOverListboxItemDetails(
            const sf::Vector2f & MOUSE_POS) const;
        
    private:
        static const float ITEM_DETAIL_TIMEOUT_SEC_;

        TreasureStage * treasureStagePtr_;
        MainMenuTitle titleImage_;
        BottomSymbol bottomImage_;
        OuroborosUPtr_t ouroborosUPtr_;
        treasure::StageMoverUPtr_t stageMoverUPtr_;
        sfml_util::gui::ListBoxUPtr_t treasureListboxUPtr_;
        sfml_util::gui::ListBoxUPtr_t inventoryListboxUPtr_;
        sfml_util::gui::TextRegionUPtr_t treasureLabelUPtr_;
        sfml_util::gui::TextRegionUPtr_t inventoryLabelUPtr_;
        sfml_util::gui::TextRegionUPtr_t coinsTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t gemsTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t weightTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t instrTextUPtr_;

        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sf::Texture corpseTexture_;
        sf::Sprite corpseSprite_;
        sf::Texture treasureTexture_;
        sf::Sprite treasureSprite_;
        sf::Texture coinsTexture_;
        sf::Sprite coinsSprite_;
        sf::Texture characterTexture_;
        sfml_util::gui::GuiImageUPtr_t characterImageUPtr_;
        item::TreasureAvailable::Enum treasureAvailable_;
        item::TreasureImage::Enum treasureImage_;
        sf::Texture redXTexture_;
        sfml_util::gui::GuiImageUPtr_t redXImageUPtr_;
        float itemDetailTimer_;
        stage::ItemDetailViewer itemDetailViewer_;
        sf::Vector2f mousePos_;
        bool canDisplayItemDetail_;

        //These members are copies of the real data in TreasureStage
        item::ItemCache heldCache_;
        item::ItemCache lockboxCache_;
    };

}
}


#endif //GAME_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
