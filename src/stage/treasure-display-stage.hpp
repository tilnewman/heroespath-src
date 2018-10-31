// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
//
// treasure-display-stage.hpp
//
#include "item/item-cache.hpp"
#include "item/treasure-available-enum.hpp"
#include "item/treasure-image-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/color-set.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/image-entity.hpp"
#include "sfml-util/image-text-entity.hpp"
#include "sfml-util/list-box.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/stage-title.hpp"
#include "stage/item-detail-viewer.hpp"
#include "stage/stage-base.hpp"
#include "stage/treasure-stage-mover.hpp"
#include "stage/treasure-stage.hpp"

#include <algorithm>
#include <memory>
#include <string>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace stage
{
    namespace treasure
    {

        // There are two listboxes.  On the left is the Treasure listbox which contains either the
        // items worn/held by defeated enemies or the contents of the lockbox carried by defeated
        // enemies.  On the right is the inventory listbox which contains the unequipped items
        // carried by one of the player's characters.
        enum class WhichListbox
        {
            Treasure,
            Inventory
        };

        // Responsible for calculating and wrapping colors used by TreasureDisplayStage.
        struct ListboxColors
        {
            ListboxColors();

            sf::Color image;
            sf::Color line;
            sf::Color foreground;
            sf::Color background;
            sf::Color title;
            sfml_util::FocusColors colorSet;
            sf::Color icon;
        };

        // Responsible for calculating and wrapping positions used by TreasureDisplayStage.
        struct DisplayMeasurements
        {
            DisplayMeasurements(const float COINS_IMAGE_BOTTOM, const float BOTTOM_SYMBOL_HEIGHT);

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
            float listboxSortIconScale;
            float listboxSortIconSpacer;
            float listboxSortIconTop;
        };

        // Responsible for wrapping all the info needed to display item details.
        struct ItemDetails
        {
            explicit ItemDetails(const sf::FloatRect & RECT, const item::ItemPtr_t ITEM_PTR)
                : rect(RECT)
                , item_ptr(ITEM_PTR)
            {}

            sf::FloatRect rect;
            item::ItemPtr_t item_ptr;
        };

        using ItemDetailsOpt_t = boost::optional<ItemDetails>;
    } // namespace treasure

    // Responsible for all displaying everything (images, listboxes, etc.) for the Treasure Stage.
    class TreasureDisplayStage
        : public stage::StageBase

        , public sfml_util::ListBox<TreasureDisplayStage, item::ItemPtr_t>::Callback_t::IHandler_t
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
    {
        using ItemListBox_t = sfml_util::ListBox<TreasureDisplayStage, item::ItemPtr_t>;
        using ItemListBoxPtr_t = sfml_util::ListBoxPtr_t<TreasureDisplayStage, item::ItemPtr_t>;
        using ItemListBoxUPtr_t = sfml_util::ListBoxUPtr_t<TreasureDisplayStage, item::ItemPtr_t>;

    public:
        TreasureDisplayStage(const TreasureDisplayStage &) = delete;
        TreasureDisplayStage(TreasureDisplayStage &&) = delete;
        TreasureDisplayStage & operator=(const TreasureDisplayStage &) = delete;
        TreasureDisplayStage & operator=(TreasureDisplayStage &&) = delete;

        explicit TreasureDisplayStage();
        virtual ~TreasureDisplayStage() = default;

        bool HandleCallback(const ItemListBox_t::Callback_t::PacketPtr_t &) override;
        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        void SetModelStage(const TreasureStagePtr_t treasureStagePtr)
        {
            treasureStagePtrOpt_ = treasureStagePtr;
        }

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
            const item::TreasureAvailable::Enum, const item::TreasureImage::Enum);

        void UpdateTreasureImage(const item::TreasureImage::Enum);

        bool IsShowingHeldItems() const;
        std::size_t CharacterIndexShowingInventory() const;
        bool IsAnythingAnimating() const;

        bool CanTreasureChange() const;
        void TreasureChange() const;

        void UpdateItemCaches(
            const item::ItemCache & HELD_CACHE, const item::ItemCache & LOCKBOX_CACHE);

        std::size_t CharacterIndex() const;
        std::size_t CharacterIndexMax() const;

        void InventoryChange(const std::size_t);

        bool IsItemDetailMovingOrShowing() const { return itemDetailViewer_.IsMovingOrShowing(); }

        std::size_t WhichCharacterInventoryIsDisplayedIndex();

        const creature::CreaturePtr_t WhichCharacterInventoryIsDisplayed();

        void RefreshAfterCacheUpdate();

        void CanDisplayItemDetail(const bool CAN) { canDisplayItemDetail_ = CAN; }

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
        void SetupForCollection_TreasureListboxSortIcons();
        void SetupForCollection_InventoryCharacterImage();
        void SetupForCollection_InventoryListboxLabel();
        void SetupForCollection_InventoryCoinsText();
        void SetupForCollection_InventoryGemsText();
        void SetupForCollection_InventoryWeightText();
        void SetupForCollection_InventoryRedXImage();
        void SetupForCollection_InventoryListboxSortIcons();
        void SetupForCollection_InstructionsText();
        void SetupForCollection_LowerButtons();

        void SetupLowerButton(
            sfml_util::ImageTextEntityUPtr_t & buttonUPtr,
            const std::string & TEXT,
            const float VERT_POS);

        void SetupListbox(
            const treasure::WhichListbox WHICH_LISTBOX,
            ItemListBoxUPtr_t & listboxUPtr,
            const item::ItemPVec_t &);

        void SetupTreasure_ListboxLabel();
        void SetupTreasure_ListboxSortIcons();
        void SetupInventory_ListboxSortIcons();
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
            sfml_util::TextRegionUPtr_t & textRegionUPtr,
            const std::string & NAME,
            const std::string & TEXT,
            const float HORIZ_POS,
            const float VERT_POS,
            const unsigned int FONT_SIZE);

        float CalculateHorizOffscreenPos() const
        {
            return CreateDisplayMeasurements().screenWidth + 1.0f;
        }

        float CalculateInventoryTextPosLeft() const;

        float CalculateInventoryTextVertShift() const { return sfutil::MapByRes(10.0f, 70.0f); }

        void ItemViewerInterruption();

        template <typename T>
        const sfml_util::IEntityPtrOpt_t GetEntityPtrAndRemoveIfNeeded(const T & GUI_ENTITY_UPTR)
        {
            if (GUI_ENTITY_UPTR)
            {
                const sfml_util::IEntityPtr_t ENTITY_PTR { GUI_ENTITY_UPTR.get() };
                EntityRemove(ENTITY_PTR);
                return ENTITY_PTR;
            }
            else
            {
                return boost::none;
            }
        }

        enum class StageAddEntity
        {
            Will,
            Wont
        };

        void EntityPtrAddCurrAndReplacePrevIfNeeded(
            const sfml_util::IEntityPtrOpt_t & PREV_GUI_ENTITY_PTR_OPT,
            const sfml_util::IEntityPtr_t CURR_GUI_ENTITY_PTR,
            const StageAddEntity WILL_ADD = StageAddEntity::Will);

        void CreateOrReplaceListboxIconImage(
            const std::string & NAME,
            const std::string & IMAGE_PATH_KEY,
            const sf::Color & COLOR,
            const float SCALE,
            sfml_util::ImageTextEntityUPtr_t & sortButtonUPtr);

    private:
        const treasure::ItemDetailsOpt_t
            MouseOverListboxItemDetails(const sf::Vector2f & MOUSE_POS) const;

    private:
        static const float ITEM_DETAIL_TIMEOUT_SEC_;

        TreasureStagePtrOpt_t treasureStagePtrOpt_;
        sfml_util::StageTitle titleImage_;
        sfml_util::BottomSymbol bottomImage_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        treasure::StageMoverUPtr_t stageMoverUPtr_;
        ItemListBoxUPtr_t treasureListboxUPtr_;
        ItemListBoxUPtr_t inventoryListboxUPtr_;
        sfml_util::TextRegionUPtr_t treasureLabelUPtr_;
        sfml_util::TextRegionUPtr_t inventoryLabelUPtr_;
        sfml_util::TextRegionUPtr_t coinsTextUPtr_;
        sfml_util::TextRegionUPtr_t gemsTextUPtr_;
        sfml_util::TextRegionUPtr_t weightTextUPtr_;
        sfml_util::TextRegionUPtr_t instrTextUPtr_;

        sfml_util::CachedTexture bgCachedTexture_;
        sf::Sprite bgSprite_;
        sfml_util::CachedTextureOpt_t corpseCachedTextureOpt_;
        sf::Sprite corpseSprite_;
        sfml_util::CachedTextureOpt_t treasureCachedTextureOpt_;
        sf::Sprite treasureSprite_;
        sfml_util::CachedTexture coinsCachedTexture_;
        sf::Sprite coinsSprite_;
        sfml_util::ImageTextEntityUPtr_t treasureAlphaButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t treasureMoneyButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t treasureWeightButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t inventoryAlphaButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t inventoryMoneyButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t inventoryWeightButtonUPtr_;
        bool isSortOrderReversedTreasureAlpha_;
        bool isSortOrderReversedTreasureMoney_;
        bool isSortOrderReversedTreasureWeight_;
        bool isSortOrderReversedInventoryAlpha_;
        bool isSortOrderReversedInventoryMoney_;
        bool isSortOrderReversedInventoryWeight_;
        sfml_util::ImageEntityUPtr_t characterImageUPtr_;
        item::TreasureAvailable::Enum treasureAvailable_;
        item::TreasureImage::Enum treasureImage_;
        sfml_util::CachedTexture xCachedTexture_;
        sfml_util::ImageEntityUPtr_t redXImageUPtr_;
        float itemDetailTimer_;
        stage::ItemDetailViewer itemDetailViewer_;
        sf::Vector2f mousePos_;
        bool canDisplayItemDetail_;
        sfml_util::ImageTextEntityUPtr_t takeAllButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t doneButtonUPtr_;

        // These members are copies of the real data in TreasureStage
        item::ItemCache heldCache_;
        item::ItemCache lockboxCache_;
    };

    using TreasureDisplayStagePtr_t = misc::NotNull<TreasureDisplayStage *>;
    using TreasureDisplayStagePtrOpt_t = boost::optional<TreasureDisplayStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TREASUREDISPLAYSTAGE_HPP_INCLUDED
