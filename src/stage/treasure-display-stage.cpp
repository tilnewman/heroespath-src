// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-display-stage.hpp
//
#include "treasure-display-stage.hpp"

#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "gui/cached-texture.hpp"
#include "gui/display.hpp"
#include "gui/entity-image-info.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-entity.hpp"
#include "gui/image-loaders.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/list-box-helpers.hpp"
#include "gui/mouse-image-info.hpp"
#include "gui/text-entity.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "misc/vectors.hpp"
#include "sfutil/scale.hpp"
#include "stage/treasure-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <vector>

namespace heroespath
{
namespace stage
{
    namespace treasure
    {

        ListboxColors::ListboxColors()
            : image(sf::Color(255, 255, 255, 190))
            , line(sfutil::color::GrayDark)
            , foreground(line)
            , background(sfutil::color::Orange - sf::Color(100, 100, 100, 220))
            , title(sfutil::color::Orange - sf::Color(130, 130, 130, 0))
            , colorSet(foreground, background)
            , icon(sf::Color(255, 255, 255, 127))
        {}

        DisplayMeasurements::DisplayMeasurements(
            const float COINS_IMAGE_BOTTOM, const float BOTTOM_SYMBOL_HEIGHT)
            : screenSizeV(sfutil::DisplaySize())
            , innerPad(sfutil::MapByRes(10.0f, 40.0f))
            , innerRect(
                  innerPad,
                  innerPad,
                  screenSizeV.x - (2.0f * innerPad),
                  screenSizeV.x - (2.0f * innerPad))
            , listboxScreenEdgeMargin(sfutil::MapByRes(35.0f, 100.0f))
            , listboxBetweenSpacer(sfutil::MapByRes(65.0f, 200.0f))
            , listboxWidth(
                  ((innerRect.width - (2.0f * listboxScreenEdgeMargin)) - listboxBetweenSpacer)
                  * 0.5f)
            , treasureListboxLeft(innerRect.left + listboxScreenEdgeMargin)
            , inventoryListboxLeft(treasureListboxLeft + listboxWidth + listboxBetweenSpacer)
            , listboxTop(COINS_IMAGE_BOTTOM + sfutil::MapByRes(30.0f, 70.0f))
            , listboxHeight((screenSizeV.y - listboxTop) - BOTTOM_SYMBOL_HEIGHT)
            , treasureListboxRegion(treasureListboxLeft, listboxTop, listboxWidth, listboxHeight)
            , inventoryListboxRegion(inventoryListboxLeft, listboxTop, listboxWidth, listboxHeight)
            , listboxMargin(10.0f)
            , listboxItemSpacer(6.0f)
            , characterImageLeft(inventoryListboxRegion.left)
            , characterImageScale(sfutil::MapByRes(0.75f, 3.25f))
            , listboxSortIconScale(sfutil::MapByRes(0.1f, 0.35f))
            , listboxSortIconSpacer(sfutil::MapByRes(22.0f, 75.0f))
            , listboxSortIconTop(
                  (listboxTop - (127.0f * listboxSortIconScale)) - sfutil::MapByRes(10.0f, 20.0f))
        {}

    } // end of namespace treasure

    const float TreasureDisplayStage::ITEM_DETAIL_TIMEOUT_SEC_ { 3.0f };

    TreasureDisplayStage::TreasureDisplayStage()
        : StageBase("TreasureDisplay", {})
        , treasureStagePtrOpt_(boost::none)
        , stageTitle_(gui::MenuImage::Treasure, true)
        , bottomImage_(true, 0.75f)
        , ouroborosUPtr_()
        , stageMoverUPtr_()
        , treasureListboxUPtr_()
        , inventoryListboxUPtr_()
        , treasureLabelUPtr_()
        , inventoryLabelUPtr_()
        , coinsTextUPtr_()
        , gemsTextUPtr_()
        , weightTextUPtr_()
        , instrTextUPtr_()
        , bgCachedTexture_("media-image-background-paper-large-gold")
        , bgSprite_(bgCachedTexture_.Get())
        , corpseCachedTextureOpt_()
        , corpseSprite_()
        , treasureCachedTextureOpt_()
        , treasureSprite_()
        , coinsCachedTexture_("media-image-misc-coins")
        , coinsSprite_(
              coinsCachedTexture_.Get(),
              misc::ConfigFile::Instance()->GetTextureRect("media-image-misc-coins"))
        , treasureAlphaButtonUPtr_()
        , treasureMoneyButtonUPtr_()
        , treasureWeightButtonUPtr_()
        , inventoryAlphaButtonUPtr_()
        , inventoryMoneyButtonUPtr_()
        , inventoryWeightButtonUPtr_()
        , isSortOrderReversedTreasureAlpha_(false)
        , isSortOrderReversedTreasureMoney_(false)
        , isSortOrderReversedTreasureWeight_(false)
        , isSortOrderReversedInventoryAlpha_(false)
        , isSortOrderReversedInventoryMoney_(false)
        , isSortOrderReversedInventoryWeight_(false)
        , characterImageUPtr_()
        , treasureAvailable_(item::TreasureAvailable::NoTreasure)
        , treasureImage_(item::TreasureImage::Count)
        , xCachedTexture_("media-image-misc-x")
        , redXImageUPtr_()
        , itemDetailTimer_(0.0f)
        , itemDetailViewer_()
        , mousePos_(0.0f, 0.0f)
        , canDisplayItemDetail_(true)
        , takeAllButtonUPtr_()
        , doneButtonUPtr_()
        , heldCache_()
        , lockboxCache_()
    {}

    TreasureDisplayStage::~TreasureDisplayStage() = default;

    void TreasureDisplayStage::SetModelStage(const TreasureStagePtr_t treasureStagePtr)
    {
        treasureStagePtrOpt_ = treasureStagePtr;
    }

    const std::string TreasureDisplayStage::HandleCallback(
        const ItemListBox_t::Callback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        if (!treasureStagePtrOpt_)
        {
            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "NOT HANDLED because the treasure stage ptr opt is empty/invalid/boost::none");
        }
        else
        {
            return treasureStagePtrOpt_.value()->HandleListboxCallback(
                ItemListBoxPtr_t(treasureListboxUPtr_.get()),
                ItemListBoxPtr_t(inventoryListboxUPtr_.get()),
                PACKET,
                PACKET_DESCRIPTION);
        }
    }

    const std::string TreasureDisplayStage::HandleCallback(
        const gui::ImageTextEntity::Callback_t::Packet_t & PACKET,
        const std::string & PACKET_DESCRIPTION)
    {
        if (PACKET.entity_ptr == treasureAlphaButtonUPtr_.get())
        {
            gui::listbox::SortByName(*treasureListboxUPtr_, isSortOrderReversedTreasureAlpha_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "saw click on image for sort-treasure-list-by-name and did it");
        }
        else if (PACKET.entity_ptr == treasureMoneyButtonUPtr_.get())
        {
            gui::listbox::SortByPrice(*treasureListboxUPtr_, isSortOrderReversedTreasureMoney_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for sort-treasure-list-by-price and did it");
        }
        else if (PACKET.entity_ptr == treasureWeightButtonUPtr_.get())
        {
            gui::listbox::SortByWeight(*treasureListboxUPtr_, isSortOrderReversedTreasureWeight_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for sort-treasure-list-by-weight and did it");
        }
        else if (PACKET.entity_ptr == inventoryAlphaButtonUPtr_.get())
        {
            gui::listbox::SortByName(*inventoryListboxUPtr_, isSortOrderReversedInventoryAlpha_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for sort-inventory-list-by-name and did it");
        }
        else if (PACKET.entity_ptr == inventoryMoneyButtonUPtr_.get())
        {
            gui::listbox::SortByPrice(*inventoryListboxUPtr_, isSortOrderReversedInventoryMoney_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for sort-inventory-list-by-price and did it");
        }
        else if (PACKET.entity_ptr == inventoryWeightButtonUPtr_.get())
        {
            gui::listbox::SortByWeight(*inventoryListboxUPtr_, isSortOrderReversedInventoryWeight_);

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for sort-inventory-list-by-weight and did it");
        }
        else if ((PACKET.entity_ptr == takeAllButtonUPtr_.get()) && treasureStagePtrOpt_)
        {
            treasureStagePtrOpt_.value()->TakeAllItems();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for take-all-from-treasure-list and did it");
        }
        else if ((PACKET.entity_ptr == doneButtonUPtr_.get()) && treasureStagePtrOpt_)
        {
            treasureStagePtrOpt_.value()->Exit();

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION,
                "saw click on image for done-taking-treasure and transitioned to the Adventure "
                "Stage");
        }

        return MakeCallbackHandlerMessage(PACKET_DESCRIPTION, "ImageTextEntity callback UNHANDLED");
    }

    void TreasureDisplayStage::Setup()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (treasureStagePtrOpt_),
            "stage::TreasureDisplayStage::Setup() called but treasureStagePtrOpt_ was invalid.  It "
            "should be set before this function is called by stage::StageFactory::Make().");

        // move the bottom symbol down to make more room on screen for other gui stuff above it
        if ((sfutil::Bottom(bottomImage_.Region()) > StageRegion().height) == false)
        {
            bottomImage_.MovePos(0.0f, (bottomImage_.Region().height * 0.4f));
        }

        SetupInitial();
    }

    void TreasureDisplayStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(bgSprite_, states);
        target.draw(stageTitle_, states);
        target.draw(bottomImage_, states);
        target.draw(corpseSprite_, states);
        target.draw(treasureSprite_, states);

        if (instrTextUPtr_)
        {
            target.draw(*instrTextUPtr_, states);
        }

        if (characterImageUPtr_)
        {
            target.draw(*characterImageUPtr_, states);
        }

        if ((item::TreasureAvailable::HeldAndLockbox == treasureAvailable_)
            || (item::TreasureAvailable::LockboxOnly == treasureAvailable_))
        {
            target.draw(coinsSprite_, states);
        }

        StageBase::draw(target, states);

        if (redXImageUPtr_)
        {
            target.draw(*redXImageUPtr_, states);
        }

        target.draw(itemDetailViewer_, states);
    }

    bool TreasureDisplayStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        ItemViewerInterruption();
        return StageBase::KeyRelease(KEY_EVENT);
    }

    void TreasureDisplayStage::UpdateMousePos(const sf::Vector2i & MOUSE_POS_V)
    {
        const sf::Vector2f NEW_MOUSE_POS { MOUSE_POS_V };

        const auto MAX_DIFF { 3.0f };
        const auto DIFF_VECTOR { mousePos_ - NEW_MOUSE_POS };
        if ((misc::Abs(DIFF_VECTOR.x) > MAX_DIFF) || (misc::Abs(DIFF_VECTOR.y) > MAX_DIFF))
        {
            ItemViewerInterruption();
        }

        mousePos_ = NEW_MOUSE_POS;

        StageBase::UpdateMousePos(MOUSE_POS_V);
    }

    void TreasureDisplayStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        ItemViewerInterruption();
        StageBase::UpdateMouseDown(MOUSE_POS_V);
    }

    void TreasureDisplayStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        UpdateTime_StageMover(ELAPSED_TIME_SECONDS);
        UpdateTime_ItemDetailViewer(ELAPSED_TIME_SECONDS);
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);
    }

    void TreasureDisplayStage::UpdateTime_StageMover(const float ELAPSED_TIME_SECONDS)
    {
        if (stageMoverUPtr_)
        {
            if (stageMoverUPtr_->UpdateTimeTreasure(ELAPSED_TIME_SECONDS))
            {
                UpdateTreasureVisuals();
            }

            if (stageMoverUPtr_->UpdateTimeInventory(ELAPSED_TIME_SECONDS))
            {
                UpdateInventoryVisuals();
            }
        }
    }

    void TreasureDisplayStage::UpdateTime_ItemDetailViewer(const float ELAPSED_TIME_SECONDS)
    {
        itemDetailViewer_.UpdateTime(ELAPSED_TIME_SECONDS);

        if (canDisplayItemDetail_)
        {
            itemDetailTimer_ += ELAPSED_TIME_SECONDS;
        }
        else
        {
            itemDetailTimer_ = 0;
        }

        if (itemDetailTimer_ > ITEM_DETAIL_TIMEOUT_SEC_)
        {
            itemDetailTimer_ = 0;

            const auto ITEM_DETAILS_OPT { MouseOverListboxItemDetails(mousePos_) };
            if (ITEM_DETAILS_OPT && canDisplayItemDetail_)
            {
                itemDetailViewer_.FadeIn(ITEM_DETAILS_OPT->item_ptr, ITEM_DETAILS_OPT->rect);
            }
        }
    }

    void TreasureDisplayStage::SetupInitial()
    {
        SetupInitial_BackgroundImage();
        SetupInitial_Ouroboros();
    }

    void TreasureDisplayStage::SetupAfterPleaseWait(const item::TreasureImage::Enum WHICH_IMAGE)
    {
        SetupAfterPleaseWait_CorpseImage();
        SetupAfterPleaseWait_TreasureImage(WHICH_IMAGE);
        SetupAfterPleaseWait_CoinsImage();
    }

    void TreasureDisplayStage::SetupForCollection(
        const item::TreasureAvailable::Enum TREASURE_AVAILABLE,
        const item::TreasureImage::Enum TREASURE_IMAGE)
    {
        treasureAvailable_ = TREASURE_AVAILABLE;
        treasureImage_ = TREASURE_IMAGE;

        const auto TREASURE_SOURCE { TreasureSource() };

        stageMoverUPtr_ = std::make_unique<stage::treasure::StageMover>(
            TREASURE_SOURCE, WhichCharacterInventoryIsDisplayedIndex());

        SetupForCollection_TreasureListbox(TREASURE_SOURCE);
        SetupForCollection_InventoryListbox();

        SetupForCollection_TreasureListboxLabel();
        SetupForCollection_TreasureListboxSortIcons();

        SetupForCollection_InventoryListboxSortIcons();
        SetupForCollection_InventoryCharacterImage();
        SetupForCollection_InventoryListboxLabel();
        SetupForCollection_InventoryCoinsText();
        SetupForCollection_InventoryGemsText();
        SetupForCollection_InventoryWeightText();
        SetupForCollection_InventoryRedXImage();

        SetupForCollection_InstructionsText();
        SetupForCollection_LowerButtons();

        stageMoverUPtr_->StartAll();
    }

    void TreasureDisplayStage::UpdateTreasureImage(const item::TreasureImage::Enum WHICH_IMAGE)
    {
        if (item::TreasureImage::ChestOpen == WHICH_IMAGE)
        {
            treasureCachedTextureOpt_ = gui::CachedTexture("media-image-misc-chest-open");
        }
        else if (item::TreasureImage::LockboxOpen == WHICH_IMAGE)
        {
            treasureCachedTextureOpt_ = gui::CachedTexture("media-image-misc-lockbox-open");
        }
    }

    bool TreasureDisplayStage::IsShowingHeldItems() const
    {
        if (stageMoverUPtr_)
        {
            return stageMoverUPtr_->Source() == stage::treasure::Type::Held;
        }
        else
        {
            return false;
        }
    }

    std::size_t TreasureDisplayStage::CharacterIndexShowingInventory() const
    {
        if (stageMoverUPtr_)
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
        else
        {
            return 0;
        }
    }

    bool TreasureDisplayStage::IsAnythingAnimating() const
    {
        return (stageMoverUPtr_ && stageMoverUPtr_->IsEitherMoving());
    }

    bool TreasureDisplayStage::CanTreasureChange() const
    {
        return (item::TreasureAvailable::HeldAndLockbox == treasureAvailable_);
    }

    void TreasureDisplayStage::TreasureChange() const
    {
        if (stageMoverUPtr_)
        {
            stageMoverUPtr_->TreasureSwitch();
        }
    }

    void TreasureDisplayStage::UpdateItemCaches(
        const item::ItemCache & HELD_CACHE, const item::ItemCache & LOCKBOX_CACHE)
    {
        heldCache_ = HELD_CACHE;
        lockboxCache_ = LOCKBOX_CACHE;
    }

    std::size_t TreasureDisplayStage::CharacterIndex() const
    {
        if (!stageMoverUPtr_)
        {
            return 0;
        }
        else
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
    }

    std::size_t TreasureDisplayStage::CharacterIndexMax() const
    {
        return game::Game::Instance()->State().Party().Characters().size() - 1;
    }

    void TreasureDisplayStage::InventoryChange(const std::size_t CHARACTER_INDEX)
    {
        if (stageMoverUPtr_)
        {
            UpdateInventoryVisuals();
            stageMoverUPtr_->InventoryIndexSet(CHARACTER_INDEX);
        }
    }

    std::size_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayedIndex()
    {
        if (!stageMoverUPtr_)
        {
            const auto NUM_CHARACTERS {
                game::Game::Instance()->State().Party().Characters().size()
            };

            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                if (game::Game::Instance()->State().Party().GetAtOrderPos(i)->IsBeast() == false)
                {
                    return i;
                }
            }

            // should never reach this code since one party member must be non-beast
            return 0;
        }
        else
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
    }

    const creature::CreaturePtr_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayed()
    {
        return game::Game::Instance()->State().Party().GetAtOrderPos(
            WhichCharacterInventoryIsDisplayedIndex());
    }

    void TreasureDisplayStage::RefreshAfterCacheUpdate()
    {
        UpdateTreasureVisuals();
        UpdateInventoryVisuals();
    }

    treasure::DisplayMeasurements TreasureDisplayStage::CreateDisplayMeasurements() const
    {
        return { coinsSprite_.getPosition().y + coinsSprite_.getGlobalBounds().height,
                 bottomImage_.Region().height };
    }

    void TreasureDisplayStage::SetupInitial_BackgroundImage()
    {
        bgSprite_.setPosition(0.0f, 0.0f);

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto NEW_SCALE_X
            = (MEASUREMENTS.screenSizeV.x / sfutil::Size(bgSprite_.getLocalBounds()).x);

        const auto NEW_SCALE_Y
            = (MEASUREMENTS.screenSizeV.y / sfutil::Size(bgSprite_.getLocalBounds()).y);

        bgSprite_.setScale(NEW_SCALE_X, NEW_SCALE_Y);
    }

    void TreasureDisplayStage::SetupInitial_Ouroboros()
    {
        ouroborosUPtr_ = std::make_unique<gui::Ouroboros>("TreasureDisplayStage's", true);
        EntityAdd(ouroborosUPtr_);
    }

    void TreasureDisplayStage::SetupAfterPleaseWait_CorpseImage()
    {
        const auto PATH_CONFIG_KEY { CorpseImageKeyFromEnemyParty() };
        corpseCachedTextureOpt_ = gui::CachedTexture(PATH_CONFIG_KEY);
        corpseSprite_.setTexture(corpseCachedTextureOpt_->Get(), true);

        const auto TEXTURE_RECT { misc::ConfigFile::Instance()->GetTextureRect(PATH_CONFIG_KEY) };
        if ((TEXTURE_RECT.width > 0) && (TEXTURE_RECT.height > 0))
        {
            corpseSprite_.setTextureRect(TEXTURE_RECT);
        }

        const auto CORPSE_IMAGE_REGION { sfutil::ScaleAndReCenterCopy(
            sfutil::DisplayRect(), sf::Vector2f(0.75f, 0.5f)) };

        sfutil::FitAndCenterTo(corpseSprite_, CORPSE_IMAGE_REGION);

        corpseSprite_.setColor(sf::Color(255, 255, 255, 50));
    }

    // The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_TreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        const auto PATH_CONFIG_KEY { item::TreasureImage::ImageConfigKey(WHICH_IMAGE) };
        treasureCachedTextureOpt_ = gui::CachedTexture(PATH_CONFIG_KEY);
        treasureSprite_.setTexture(treasureCachedTextureOpt_->Get(), true);

        const auto TEXTURE_RECT { misc::ConfigFile::Instance()->GetTextureRect(PATH_CONFIG_KEY) };
        if ((TEXTURE_RECT.width > 0) && (TEXTURE_RECT.height > 0))
        {
            treasureSprite_.setTextureRect(TEXTURE_RECT);
        }

        const sf::Vector2f SIZE_CONSTRAINT_V(
            sfutil::ScreenRatioToPixelsHoriz(0.36f), sfutil::ScreenRatioToPixelsVert(0.25f));

        sfutil::Fit(treasureSprite_, SIZE_CONSTRAINT_V);

        treasureSprite_.setPosition(
            sfutil::MapByRes(50.0f, 350.0f),
            sfutil::Bottom(stageTitle_.Region()) - sfutil::MapByRes(40.0f, 250.0f));

        treasureSprite_.setColor(sf::Color(255, 255, 255, 192));
    }

    // The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_CoinsImage()
    {
        const auto COINS_IMAGE_WIDTH { sfutil::ScreenRatioToPixelsHoriz(0.125f) };
        const auto COINS_SCALE { COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width };
        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        const auto COINS_LEFT { treasureSprite_.getPosition().x
                                + (treasureSprite_.getGlobalBounds().width * 0.80f) };

        const auto COINS_TOP { treasureSprite_.getPosition().y
                               + (treasureSprite_.getGlobalBounds().height * 0.75f) };

        coinsSprite_.setPosition(COINS_LEFT, COINS_TOP);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));
    }

    const std::string TreasureDisplayStage::CorpseImageKeyFromEnemyParty() const
    {
        const auto DEAD_ENEMY_CHARACTERS_PVEC { combat::Encounter::Instance()->DeadNonPlayers() };

        std::vector<std::string> corpseKeyStrVec;
        for (const auto & NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {
            const auto CORPSE_KEY_STR_VEC { creature::race::CorpseImageKeys(
                NEXT_ENEMY_CHARACTER_PTR->Race()) };

            std::copy(
                CORPSE_KEY_STR_VEC.begin(),
                CORPSE_KEY_STR_VEC.end(),
                std::back_inserter(corpseKeyStrVec));
        }

        if (DEAD_ENEMY_CHARACTERS_PVEC.empty() || corpseKeyStrVec.empty())
        {
            M_HP_LOG_ERR(
                "stage::TreasureDisplayStage::GetCorpseImageKeyFromEnemyParty() "
                << "was unable to gather any key strings.  Using default image.  "
                << "DEAD_ENEMY_CHARACTERS_PVEC.size()=" << DEAD_ENEMY_CHARACTERS_PVEC.size()
                << ", corpseKeyStrVec.size()=" << corpseKeyStrVec.size());

            const auto DEFAULT_CORPSE_KEY_STR_VEC { creature::race::CorpseImageKeys(
                creature::race::Human) };

            return misc::RandomSelect(DEFAULT_CORPSE_KEY_STR_VEC);
        }
        else
        {
            // Allow duplicates in corpseKeyStrVec so that the more a race was faced
            // during combat means the more likely that corpse image is shown.
            return misc::RandomSelect(corpseKeyStrVec);
        }
    }

    void TreasureDisplayStage::SetupForCollection_TreasureListbox(
        const stage::treasure::Type TREASURE_SOURCE)
    {
        SetupListbox(
            treasure::WhichListbox::Treasure,
            treasureListboxUPtr_,
            ((TREASURE_SOURCE == stage::treasure::Type::Container) ? lockboxCache_.items_pvec
                                                                   : heldCache_.items_pvec));

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto TREASURE_ONSCREEN_POS_V { sf::Vector2f(
            MEASUREMENTS.treasureListboxRegion.left, MEASUREMENTS.treasureListboxRegion.top) };

        const auto TREASURE_OFFSCREEN_POS_V { sf::Vector2f(
            (MEASUREMENTS.treasureListboxRegion.width * -1.0f) - 10.0f,
            MEASUREMENTS.treasureListboxRegion.top) };

        stageMoverUPtr_->AddTreasureObject(
            gui::IEntityPtrOpt_t(treasureListboxUPtr_.get()),
            TREASURE_ONSCREEN_POS_V,
            TREASURE_OFFSCREEN_POS_V);
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListbox()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto INVENTORY_ONSCREEN_POS_V { sf::Vector2f(
            MEASUREMENTS.inventoryListboxRegion.left, MEASUREMENTS.inventoryListboxRegion.top) };

        const auto INVENTORY_OFFSCREEN_POS_V { sf::Vector2f(
            CalculateHorizOffscreenPos(), MEASUREMENTS.inventoryListboxRegion.top) };

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(inventoryListboxUPtr_.get()),
            INVENTORY_ONSCREEN_POS_V,
            INVENTORY_OFFSCREEN_POS_V);
    }

    void TreasureDisplayStage::SetupForCollection_TreasureListboxLabel()
    {
        SetupTreasure_ListboxLabel();

        stageMoverUPtr_->AddTreasureObject(
            gui::IEntityPtrOpt_t(treasureLabelUPtr_.get()),
            treasureLabelUPtr_->GetEntityPos(),
            sf::Vector2f(-750.0f, treasureLabelUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_TreasureListboxSortIcons()
    {
        SetupTreasure_ListboxSortIcons();

        // The -300.0f here works because it is (the negative of) anything over 256.0f,
        // which is the max dimension of all listbox sort icon images.
        const auto OFFSCREEN_POS_HORIZ { -300.0f };

        stageMoverUPtr_->AddTreasureObject(
            gui::IEntityPtrOpt_t(treasureAlphaButtonUPtr_.get()),
            treasureAlphaButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureAlphaButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddTreasureObject(
            gui::IEntityPtrOpt_t(treasureMoneyButtonUPtr_.get()),
            treasureMoneyButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureMoneyButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddTreasureObject(
            gui::IEntityPtrOpt_t(treasureWeightButtonUPtr_.get()),
            treasureWeightButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureWeightButtonUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListboxLabel()
    {
        SetupInventory_ListboxLabel();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(inventoryLabelUPtr_.get()),
            inventoryLabelUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryLabelUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryCoinsText()
    {
        SetupInventory_CoinsText();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(coinsTextUPtr_.get()),
            coinsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), coinsTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryGemsText()
    {
        SetupInventory_GemsText();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(gemsTextUPtr_.get()),
            gemsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), gemsTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryCharacterImage()
    {
        SetupInventory_CharacterImage();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(characterImageUPtr_.get()),
            characterImageUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), characterImageUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryWeightText()
    {
        SetupInventory_WeightText();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(weightTextUPtr_.get()),
            weightTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), weightTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryRedXImage()
    {
        SetupInventory_RedXImage();

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(redXImageUPtr_.get()),
            inventoryListboxUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryListboxUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListboxSortIcons()
    {
        SetupInventory_ListboxSortIcons();

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto OFFSCREEN_POS_HORIZ { MEASUREMENTS.screenSizeV.x + MEASUREMENTS.listboxWidth };

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(inventoryAlphaButtonUPtr_.get()),
            inventoryAlphaButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryAlphaButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(inventoryMoneyButtonUPtr_.get()),
            inventoryMoneyButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryMoneyButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddInventoryObject(
            gui::IEntityPtrOpt_t(inventoryWeightButtonUPtr_.get()),
            inventoryWeightButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryWeightButtonUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InstructionsText()
    {
        const gui::TextInfo TEXT_INFO(
            "(press spacebar to change treasure, use arrows or numbers to change characters)",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDark,
            gui::Justified::Left,
            sf::Text::Italic);

        instrTextUPtr_
            = std::make_unique<gui::TextRegion>("TreasureStage'sInstructionText", TEXT_INFO);

        // the +93 and -10 were found by experiment to look better
        instrTextUPtr_->SetEntityPos(
            (CreateDisplayMeasurements().screenSizeV.x * 0.5f)
                - (instrTextUPtr_->GetEntityRegion().width * 0.5f) + 93.0f,
            sfutil::Bottom(stageTitle_.Region()) - 10.0f);

        EntityAdd(instrTextUPtr_);
    }

    void TreasureDisplayStage::SetupForCollection_LowerButtons()
    {
        const auto TAKEALL_POS_TOP { instrTextUPtr_->GetEntityPos().y
                                     + instrTextUPtr_->GetEntityRegion().height
                                     + sfutil::MapByRes(15.0f, 100.0f) };

        SetupLowerButton(takeAllButtonUPtr_, "(T)ake All", TAKEALL_POS_TOP);

        const auto EXIT_POS_TOP { (TAKEALL_POS_TOP + takeAllButtonUPtr_->GetEntityRegion().height)
                                  - sfutil::SpacerOld(50.0f) };

        SetupLowerButton(doneButtonUPtr_, "(E)xit", EXIT_POS_TOP);
    }

    void TreasureDisplayStage::SetupLowerButton(
        gui::ImageTextEntityUPtr_t & buttonUPtr, const std::string & TEXT, const float VERT_POS)
    {
        const auto COLOR_UP { sfutil::color::GrayDarker };
        const auto COLOR_OVER { COLOR_UP - sf::Color(0, 0, 0, 127) };
        const auto COLOR_DOWN { sf::Color::Black };

        gui::TextInfo textInfo(
            TEXT,
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Largeish(),
            COLOR_UP,
            gui::Justified::Left);

        const gui::MouseTextInfo MOUSE_TEXT_INFO {
            textInfo, COLOR_DOWN, COLOR_OVER, sf::Color::White
        };

        GetEntityPtrAndRemoveIfNeeded(buttonUPtr);

        buttonUPtr = std::make_unique<gui::ImageTextEntity>(
            "TreasureStage's" + TEXT,
            gui::MouseImageInfo(),
            MOUSE_TEXT_INFO,
            gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            gui::ImageTextEntity::MouseStateSync::Text);

        const auto HORIZ_POS { (CreateDisplayMeasurements().screenSizeV.x * 0.5f)
                               - (buttonUPtr->GetEntityRegion().width * 0.5f) };

        buttonUPtr->SetEntityPos(HORIZ_POS, VERT_POS);
        EntityAdd(buttonUPtr);
    }

    void TreasureDisplayStage::SetupListbox(
        const treasure::WhichListbox WHICH_LISTBOX,
        gui::ListBoxUPtr_t<TreasureDisplayStage, item::ItemPtr_t> & listboxUPtr,
        const item::ItemPVec_t & ITEMS_PVEC)
    {
        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(listboxUPtr) };

        treasure::ListboxColors listboxColors;

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto LISTBOX_REGION { (
            (WHICH_LISTBOX == treasure::WhichListbox::Treasure)
                ? MEASUREMENTS.treasureListboxRegion
                : MEASUREMENTS.inventoryListboxRegion) };

        gui::BoxEntityInfo listBoxInfo;

        listBoxInfo.SetupBorder(true, 1.0f);

        listBoxInfo.focus_colors = listboxColors.colorSet;

        listBoxInfo.SetupColor(listboxColors.background);

        gui::ListBoxPacket listBoxPacket(
            LISTBOX_REGION, listBoxInfo, listboxColors.line, listboxColors.image);

        listboxUPtr = std::make_unique<gui::ListBox<TreasureDisplayStage, item::ItemPtr_t>>(
            "TreasureDisplayStage's_CharacterInventoryListBox",
            misc::MakeNotNull(this),
            misc::MakeNotNull(this),
            listBoxPacket);

        gui::TextInfo textInfo(
            "",
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Smallish(),
            sfutil::color::GrayDarker);

        for (const auto & ITEM_PTR : ITEMS_PVEC)
        {
            listboxUPtr->Append(std::make_unique<gui::ListElement<item::ItemPtr_t>>(
                ITEM_PTR, gui::TextInfo(textInfo, ITEM_PTR->Name())));
        }

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(listboxUPtr.get()));
    }

    void TreasureDisplayStage::SetupTreasure_ListboxLabel()
    {
        const auto LABEL_TEXT { [&]() -> std::string {
            if (TreasureSource() == stage::treasure::Type::Held)
            {
                return "Items Worn by Enemies";
            }
            else
            {
                return "Items in the " + item::TreasureImage::ToContainerName(treasureImage_, true);
            }
        }() };

        const gui::TextInfo TEXT_INFO(
            LABEL_TEXT,
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Large(),
            sfutil::color::GrayDarker,
            gui::Justified::Left);

        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(treasureLabelUPtr_) };

        treasureLabelUPtr_
            = std::make_unique<gui::TextRegion>("TreasureStage'sTreasureListboxLabel", TEXT_INFO);

        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const auto LEFT { MEASUREMENTS.treasureListboxRegion.left };

        const auto TOP { (MEASUREMENTS.treasureListboxRegion.top
                          - treasureLabelUPtr_->GetEntityRegion().height)
                         + 10.0f };

        treasureLabelUPtr_->SetEntityPos(LEFT, TOP);

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(treasureLabelUPtr_.get()));
    }

    void TreasureDisplayStage::SetupTreasure_ListboxSortIcons()
    {
        const auto MEASUREMENTS { CreateDisplayMeasurements() };
        const auto SCALE { MEASUREMENTS.listboxSortIconScale };
        const auto LISTBOX_COLORS { stage::treasure::ListboxColors() };

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Alpha",
            "media-image-misc-abc",
            LISTBOX_COLORS.icon,
            SCALE,
            treasureAlphaButtonUPtr_);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Money",
            "media-image-misc-money-bag",
            LISTBOX_COLORS.icon,
            SCALE,
            treasureMoneyButtonUPtr_);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Weight",
            "media-image-misc-weight",
            LISTBOX_COLORS.icon,
            SCALE,
            treasureWeightButtonUPtr_);

        const auto LISTBOX_LEFT { MEASUREMENTS.treasureListboxLeft };

        const auto ICON_POS_HORIZ_ABC { ((LISTBOX_LEFT + MEASUREMENTS.listboxWidth)
                                         - (treasureAlphaButtonUPtr_->GetEntityRegion().width
                                            + treasureMoneyButtonUPtr_->GetEntityRegion().width
                                            + treasureWeightButtonUPtr_->GetEntityRegion().width))
                                        - (MEASUREMENTS.listboxSortIconSpacer * 2.25f) };

        const auto ICON_POS_VERT_ABC { MEASUREMENTS.listboxSortIconTop
                                       - (treasureAlphaButtonUPtr_->GetEntityRegion().height
                                          * 0.5f) };

        const auto ICON_POS_HORIZ_MONEY { ICON_POS_HORIZ_ABC
                                          + treasureAlphaButtonUPtr_->GetEntityRegion().width
                                          + MEASUREMENTS.listboxSortIconSpacer };

        const auto ICON_POS_VERT_MONEY { MEASUREMENTS.listboxSortIconTop
                                         - (treasureMoneyButtonUPtr_->GetEntityRegion().height
                                            * 0.5f) };

        const auto ICON_POS_HORIZ_WEIGHT { ICON_POS_HORIZ_MONEY
                                           + treasureMoneyButtonUPtr_->GetEntityRegion().width
                                           + MEASUREMENTS.listboxSortIconSpacer };

        const auto ICON_POS_VERT_WEIGHT { MEASUREMENTS.listboxSortIconTop
                                          - (treasureWeightButtonUPtr_->GetEntityRegion().height
                                             * 0.5f) };

        treasureAlphaButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_ABC, ICON_POS_VERT_ABC);
        treasureMoneyButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_MONEY, ICON_POS_VERT_MONEY);
        treasureWeightButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_WEIGHT, ICON_POS_VERT_WEIGHT);
    }

    void TreasureDisplayStage::SetupInventory_ListboxSortIcons()
    {
        const auto MEASUREMENTS { CreateDisplayMeasurements() };
        const auto SCALE { MEASUREMENTS.listboxSortIconScale };
        const auto LISTBOX_COLORS { stage::treasure::ListboxColors() };

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Alpha",
            "media-image-misc-abc",
            LISTBOX_COLORS.icon,
            SCALE,
            inventoryAlphaButtonUPtr_);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Money",
            "media-image-misc-money-bag",
            LISTBOX_COLORS.icon,
            SCALE,
            inventoryMoneyButtonUPtr_);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Weight",
            "media-image-misc-weight",
            LISTBOX_COLORS.icon,
            SCALE,
            inventoryWeightButtonUPtr_);

        const auto LISTBOX_LEFT { MEASUREMENTS.inventoryListboxLeft };

        const auto ICON_POS_HORIZ_ABC { ((LISTBOX_LEFT + MEASUREMENTS.listboxWidth)
                                         - (inventoryAlphaButtonUPtr_->GetEntityRegion().width
                                            + inventoryMoneyButtonUPtr_->GetEntityRegion().width
                                            + inventoryWeightButtonUPtr_->GetEntityRegion().width))
                                        - (MEASUREMENTS.listboxSortIconSpacer * 2.25f) };

        const auto ICON_POS_VERT_ABC { MEASUREMENTS.listboxSortIconTop
                                       - (inventoryAlphaButtonUPtr_->GetEntityRegion().height
                                          * 0.5f) };

        const auto ICON_POS_HORIZ_MONEY { ICON_POS_HORIZ_ABC
                                          + inventoryAlphaButtonUPtr_->GetEntityRegion().width
                                          + MEASUREMENTS.listboxSortIconSpacer };

        const auto ICON_POS_VERT_MONEY { MEASUREMENTS.listboxSortIconTop
                                         - (inventoryMoneyButtonUPtr_->GetEntityRegion().height
                                            * 0.5f) };

        const auto ICON_POS_HORIZ_WEIGHT { ICON_POS_HORIZ_MONEY
                                           + inventoryMoneyButtonUPtr_->GetEntityRegion().width
                                           + MEASUREMENTS.listboxSortIconSpacer };

        const auto ICON_POS_VERT_WEIGHT { MEASUREMENTS.listboxSortIconTop
                                          - (inventoryWeightButtonUPtr_->GetEntityRegion().height
                                             * 0.5f) };

        inventoryAlphaButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_ABC, ICON_POS_VERT_ABC);
        inventoryMoneyButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_MONEY, ICON_POS_VERT_MONEY);
        inventoryWeightButtonUPtr_->SetEntityPos(ICON_POS_HORIZ_WEIGHT, ICON_POS_VERT_WEIGHT);
    }

    void TreasureDisplayStage::SetupInventory_CharacterImage()
    {
        const auto MEASUREMENTS { CreateDisplayMeasurements() };

        const sf::Vector2f SCALE_V(
            MEASUREMENTS.characterImageScale, MEASUREMENTS.characterImageScale);

        const sf::Vector2f POS_V(
            MEASUREMENTS.characterImageLeft,
            inventoryListboxUPtr_->GetEntityPos().y - (256.0f * MEASUREMENTS.characterImageScale));

        const gui::EntityImageInfo ENTITY_IMAGE_INFO(
            gui::LoadAndCacheImage(
                WhichCharacterInventoryIsDisplayed(),
                gui::ImageOptions::InvertedCharacterOptions()),
            POS_V,
            SCALE_V,
            boost::none,
            sf::Color(255, 255, 255, 127));

        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(characterImageUPtr_) };

        characterImageUPtr_ = std::make_unique<gui::ImageEntity>(
            "TreasureDisplayStage's_CharacterImage", gui::MouseImageInfo(true, ENTITY_IMAGE_INFO));

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(characterImageUPtr_.get()));
    }

    void TreasureDisplayStage::SetupInventory_ListboxLabel()
    {
        const auto CREATURE_PTR { WhichCharacterInventoryIsDisplayed() };
        const auto IS_BEAST { CREATURE_PTR->IsBeast() };

        SetupInventoryText(
            inventoryLabelUPtr_,
            "ListboxLabel",
            ((IS_BEAST) ? "(beasts cannot carry items)" : CREATURE_PTR->Name()),
            CalculateInventoryTextPosLeft(),
            characterImageUPtr_->GetEntityPos().y + sfutil::MapByRes(25.0f, 50.0f),
            gui::FontManager::Instance()->Size_Large());
    }

    void TreasureDisplayStage::SetupInventory_CoinsText()
    {
        std::ostringstream ss;
        ss << "Coins: ";

        const auto CREATURE_PTR { WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Coins() << "/";

            Coin_t coinSum { 0_coin };
            for (const auto & NEXT_CREATURE_PTR :
                 game::Game::Instance()->State().Party().Characters())
            {
                coinSum += NEXT_CREATURE_PTR->Inventory().Coins();
            }

            ss << coinSum;
        }

        SetupInventoryText(
            coinsTextUPtr_,
            "CoinsText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (inventoryLabelUPtr_->GetEntityPos().y + inventoryLabelUPtr_->GetEntityRegion().height)
                - CalculateInventoryTextVertShift(),
            gui::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_GemsText()
    {
        std::ostringstream ss;
        ss << "Gems: ";

        const auto CREATURE_PTR { WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Gems() << "/";

            Gem_t gemSum { 0_gem };
            for (const auto & NEXT_CREATURE_PTR :
                 game::Game::Instance()->State().Party().Characters())
            {
                gemSum += NEXT_CREATURE_PTR->Inventory().Gems();
            }

            ss << gemSum;
        }

        SetupInventoryText(
            gemsTextUPtr_,
            "GemsText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (coinsTextUPtr_->GetEntityPos().y + coinsTextUPtr_->GetEntityRegion().height)
                - CalculateInventoryTextVertShift(),
            gui::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_WeightText()
    {
        std::ostringstream ss;
        ss << "Weight: ";

        const auto CREATURE_PTR { WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Weight() << "/" << CREATURE_PTR->WeightCanCarry()
               << "  ("
               << static_cast<int>(
                      (CREATURE_PTR->Inventory().Weight().GetAs<float>()
                       / CREATURE_PTR->WeightCanCarry().GetAs<float>())
                      * 100.0f)
               << "%)";
        }

        SetupInventoryText(
            weightTextUPtr_,
            "WeightText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (gemsTextUPtr_->GetEntityPos().y + gemsTextUPtr_->GetEntityRegion().height)
                - CalculateInventoryTextVertShift(),
            gui::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_RedXImage()
    {
        auto makeEntityImageInfo = [&]() {
            if (WhichCharacterInventoryIsDisplayed()->IsBeast())
            {
                return gui::EntityImageInfo(
                    xCachedTexture_,
                    inventoryListboxUPtr_->GetEntityRegion(),
                    boost::none,
                    sf::Color(192, 0, 0, 100));
            }
            else
            {
                return gui::EntityImageInfo();
            }
        };

        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(redXImageUPtr_) };

        redXImageUPtr_ = std::make_unique<gui::ImageEntity>(
            "TreasureDisplayStage's_RedXImage", gui::MouseImageInfo(true, makeEntityImageInfo()));

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(redXImageUPtr_.get()), StageAddEntity::Wont);
    }

    stage::treasure::Type TreasureDisplayStage::TreasureSource() const
    {
        if (!stageMoverUPtr_)
        {
            return (
                ((treasureAvailable_ == item::TreasureAvailable::HeldAndLockbox)
                 || (treasureAvailable_ == item::TreasureAvailable::LockboxOnly))
                    ? stage::treasure::Type::Container
                    : stage::treasure::Type::Held);
        }
        else
        {
            return stageMoverUPtr_->Source();
        }
    }

    void TreasureDisplayStage::UpdateTreasureVisuals()
    {
        SetupListbox(
            treasure::WhichListbox::Treasure,
            treasureListboxUPtr_,
            ((TreasureSource() == stage::treasure::Type::Container) ? lockboxCache_.items_pvec
                                                                    : heldCache_.items_pvec));

        SetupTreasure_ListboxLabel();
    }

    void TreasureDisplayStage::UpdateInventoryVisuals()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());

        SetupInventory_ListboxLabel();
        SetupInventory_CharacterImage();
        SetupInventory_CoinsText();
        SetupInventory_GemsText();
        SetupInventory_WeightText();
        SetupInventory_RedXImage();
    }

    void TreasureDisplayStage::SetupInventoryText(
        gui::TextRegionUPtr_t & textRegionUPtr,
        const std::string & NAME,
        const std::string & TEXT,
        const float HORIZ_POS,
        const float VERT_POS,
        const unsigned int FONT_SIZE)
    {
        const gui::TextInfo TEXT_INFO(
            TEXT, gui::GuiFont::System, FONT_SIZE, sfutil::color::GrayDarker, gui::Justified::Left);

        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(textRegionUPtr) };

        textRegionUPtr
            = std::make_unique<gui::TextRegion>("TreasureStage'sInventory" + NAME, TEXT_INFO);

        textRegionUPtr->SetEntityPos(HORIZ_POS, VERT_POS);

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(textRegionUPtr.get()));
    }

    float TreasureDisplayStage::CalculateInventoryTextPosLeft() const
    {
        return characterImageUPtr_->GetEntityPos().x
            + (256.0f * CreateDisplayMeasurements().characterImageScale);
    }

    void TreasureDisplayStage::ItemViewerInterruption()
    {
        itemDetailTimer_ = 0.0f;
        itemDetailViewer_.FadeOut();
    }

    const treasure::ItemDetailsOpt_t
        TreasureDisplayStage::MouseOverListboxItemDetails(const sf::Vector2f & MOUSE_POS) const
    {
        if (treasureListboxUPtr_)
        {
            const auto MOUSEOVER_ELEMENT_PTR_OPT { treasureListboxUPtr_->AtPos(MOUSE_POS) };

            if (MOUSEOVER_ELEMENT_PTR_OPT)
            {
                return treasure::ItemDetails(
                    treasureListboxUPtr_->ElementRegion(MOUSEOVER_ELEMENT_PTR_OPT.value(), true),
                    MOUSEOVER_ELEMENT_PTR_OPT.value()->Element());
            }
        }

        if (inventoryListboxUPtr_)
        {
            const auto MOUSEOVER_ELEMENT_PTR_OPT { inventoryListboxUPtr_->AtPos(MOUSE_POS) };

            if (MOUSEOVER_ELEMENT_PTR_OPT)
            {
                return treasure::ItemDetails(
                    inventoryListboxUPtr_->ElementRegion(MOUSEOVER_ELEMENT_PTR_OPT.value(), true),
                    MOUSEOVER_ELEMENT_PTR_OPT.value()->Element());
            }
        }

        return boost::none;
    }

    void TreasureDisplayStage::EntityPtrAddCurrAndReplacePrevIfNeeded(
        const gui::IEntityPtrOpt_t & PREV_GUI_ENTITY_PTR_OPT,
        const gui::IEntityPtr_t CURR_GUI_ENTITY_PTR,
        const StageAddEntity WILL_ADD)
    {
        if (WILL_ADD == StageAddEntity::Will)
        {
            EntityAdd(CURR_GUI_ENTITY_PTR);
        }

        if ((stageMoverUPtr_) && PREV_GUI_ENTITY_PTR_OPT)
        {
            stageMoverUPtr_->ReplaceEntity(PREV_GUI_ENTITY_PTR_OPT.value(), CURR_GUI_ENTITY_PTR);
        }
    }

    void TreasureDisplayStage::CreateOrReplaceListboxIconImage(
        const std::string & NAME,
        const std::string & IMAGE_PATH_KEY,
        const sf::Color & COLOR,
        const float SCALE,
        gui::ImageTextEntityUPtr_t & sortButtonUPtr)
    {
        const auto PREV_ENTITY_PTR { GetEntityPtrAndRemoveIfNeeded(sortButtonUPtr) };

        const gui::EntityImageInfo SORT_IMAGE_ENTITY_INFO_UP(
            gui::CachedTexture(IMAGE_PATH_KEY),
            sf::Vector2f(0.0f, 0.0f),
            sf::Vector2f(SCALE, SCALE),
            boost::none,
            COLOR);

        sortButtonUPtr = std::make_unique<gui::ImageTextEntity>(
            NAME,
            gui::MouseImageInfo(true, SORT_IMAGE_ENTITY_INFO_UP),
            gui::MouseTextInfo(),
            gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
            gui::ImageTextEntity::MouseStateSync::Image);

        EntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, gui::IEntityPtr_t(sortButtonUPtr.get()));
    }

} // namespace stage
} // namespace heroespath
