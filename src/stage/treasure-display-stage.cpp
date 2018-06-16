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
#include "game/game-data-file.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "item/item.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/list-box-helpers.hpp"
#include "sfml-util/gui/list-box-item-factory.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "stage/treasure-stage.hpp"

namespace heroespath
{
namespace stage
{
    namespace treasure
    {

        ListboxColors::ListboxColors()
            : image(sf::Color(255, 255, 255, 190))
            , line(sfml_util::FontManager::Color_GrayDark())
            , foreground(line)
            , background(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220))
            , title(sfml_util::FontManager::Color_Orange() - sf::Color(130, 130, 130, 0))
            , colorSet(foreground, background)
            , icon(sf::Color(255, 255, 255, 127))
        {}

        DisplayMeasurements::DisplayMeasurements(
            const float COINS_IMAGE_BOTTOM, const float BOTTOM_SYMBOL_HEIGHT)
            : screenWidth(sfml_util::Display::Instance()->GetWinWidth())
            , screenHeight(sfml_util::Display::Instance()->GetWinHeight())
            , innerPad(sfml_util::MapByRes(10.0f, 40.0f))
            , innerRect(
                  innerPad,
                  innerPad,
                  screenWidth - (2.0f * innerPad),
                  screenWidth - (2.0f * innerPad))
            , listboxScreenEdgeMargin(sfml_util::MapByRes(35.0f, 100.0f))
            , listboxBetweenSpacer(sfml_util::MapByRes(65.0f, 200.0f))
            , listboxWidth(
                  ((innerRect.width - (2.0f * listboxScreenEdgeMargin)) - listboxBetweenSpacer)
                  * 0.5f)
            , treasureListboxLeft(innerRect.left + listboxScreenEdgeMargin)
            , inventoryListboxLeft(treasureListboxLeft + listboxWidth + listboxBetweenSpacer)
            , listboxTop(COINS_IMAGE_BOTTOM + sfml_util::MapByRes(30.0f, 70.0f))
            , listboxHeight((screenHeight - listboxTop) - BOTTOM_SYMBOL_HEIGHT)
            , treasureListboxRegion(treasureListboxLeft, listboxTop, listboxWidth, listboxHeight)
            , inventoryListboxRegion(inventoryListboxLeft, listboxTop, listboxWidth, listboxHeight)
            , listboxMargin(10.0f)
            , listboxItemSpacer(6.0f)
            , characterImageLeft(inventoryListboxRegion.left)
            , characterImageScale(sfml_util::MapByRes(0.75f, 3.25f))
            , listboxSortIconScale(sfml_util::MapByRes(0.1f, 0.35f))
            , listboxSortIconSpacer(sfml_util::MapByRes(22.0f, 75.0f))
            , listboxSortIconTop(
                  (listboxTop - (127.0f * listboxSortIconScale))
                  - sfml_util::MapByRes(10.0f, 20.0f))
        {}

    } // end of namespace treasure

    const float TreasureDisplayStage::ITEM_DETAIL_TIMEOUT_SEC_{ 3.0f };

    TreasureDisplayStage::TreasureDisplayStage(const TreasureStagePtr_t TREASURE_STAGE_PTR)
        : Stage("TreasureDisplay", {}, false)
        , treasureStagePtr_(TREASURE_STAGE_PTR)
        , titleImage_("treasure-button.png", true, 1.0f, 0.75f)
        , bottomImage_(0.75f, true, sf::Color::White, 0.4f)
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
        , backgroundTexture_()
        , backgroundSprite_()
        , corpseTexture_()
        , corpseSprite_()
        , treasureTexture_()
        , treasureSprite_()
        , coinsTexture_()
        , coinsSprite_()
        , characterTexture_()
        , listboxSortIconAlphaTexture_()
        , listboxSortIconMoneyTexture_()
        , listboxSortIconWeightTexture_()
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
        , redXTexture_()
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

    bool TreasureDisplayStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        return treasureStagePtr_->HandleListboxCallback(
            treasureListboxUPtr_.get(), inventoryListboxUPtr_.get(), PACKAGE);
    }

    bool TreasureDisplayStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == treasureAlphaButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemName(
                *treasureListboxUPtr_, isSortOrderReversedTreasureAlpha_);

            return true;
        }
        else if (PACKAGE.PTR_ == treasureMoneyButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemPrice(
                *treasureListboxUPtr_, isSortOrderReversedTreasureMoney_);

            return true;
        }
        else if (PACKAGE.PTR_ == treasureWeightButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemWeight(
                *treasureListboxUPtr_, isSortOrderReversedTreasureWeight_);

            return true;
        }
        else if (PACKAGE.PTR_ == inventoryAlphaButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemName(
                *inventoryListboxUPtr_, isSortOrderReversedInventoryAlpha_);

            return true;
        }
        else if (PACKAGE.PTR_ == inventoryMoneyButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemPrice(
                *inventoryListboxUPtr_, isSortOrderReversedInventoryMoney_);

            return true;
        }
        else if (PACKAGE.PTR_ == inventoryWeightButtonUPtr_.get())
        {
            sfml_util::gui::listbox::SortByItemWeight(
                *inventoryListboxUPtr_, isSortOrderReversedInventoryWeight_);

            return true;
        }
        else if (PACKAGE.PTR_ == takeAllButtonUPtr_.get())
        {
            treasureStagePtr_->TakeAllItems();
            return true;
        }
        else if (PACKAGE.PTR_ == doneButtonUPtr_.get())
        {
            treasureStagePtr_->Exit();
            return true;
        }

        return false;
    }

    void TreasureDisplayStage::Setup()
    {
        Stage::StageRegionSet(sf::FloatRect(
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()));

        SetupInitial();
    }

    void TreasureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);
        target.draw(titleImage_, STATES);
        target.draw(bottomImage_, STATES);
        target.draw(corpseSprite_, STATES);
        target.draw(treasureSprite_, STATES);

        if (instrTextUPtr_)
        {
            target.draw(*instrTextUPtr_, STATES);
        }

        if (characterImageUPtr_)
        {
            target.draw(*characterImageUPtr_, STATES);
        }

        if ((item::TreasureAvailable::HeldAndLockbox == treasureAvailable_)
            || (item::TreasureAvailable::LockboxOnly == treasureAvailable_))
        {
            target.draw(coinsSprite_, STATES);
        }

        Stage::Draw(target, STATES);

        if (redXImageUPtr_)
        {
            target.draw(*redXImageUPtr_, STATES);
        }

        target.draw(itemDetailViewer_, STATES);
    }

    bool TreasureDisplayStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        ItemViewerInterruption();
        return Stage::KeyRelease(KEY_EVENT);
    }

    void TreasureDisplayStage::UpdateMousePos(const sf::Vector2i & MOUSE_POS_V)
    {
        auto const NEW_MOUSE_POS{ sfml_util::ConvertVector<int, float>(MOUSE_POS_V) };

        auto const MAX_DIFF{ 3.0f };
        auto const DIFF_VECTOR{ mousePos_ - NEW_MOUSE_POS };
        if ((std::abs(DIFF_VECTOR.x) > MAX_DIFF) || (std::abs(DIFF_VECTOR.y) > MAX_DIFF))
        {
            ItemViewerInterruption();
        }

        mousePos_ = NEW_MOUSE_POS;

        Stage::UpdateMousePos(MOUSE_POS_V);
    }

    void TreasureDisplayStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        ItemViewerInterruption();
        Stage::UpdateMouseDown(MOUSE_POS_V);
    }

    void TreasureDisplayStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        UpdateTime_StageMover(ELAPSED_TIME_SECONDS);
        UpdateTime_ItemDetailViewer(ELAPSED_TIME_SECONDS);
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);
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

            auto const ITEM_DETAILS_OPT{ MouseOverListboxItemDetails(mousePos_) };
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

        auto const TREASURE_SOURCE{ TreasureSource() };

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
            sfml_util::Loaders::Texture(
                treasureTexture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-chest-open"));
        }
        else if (item::TreasureImage::LockboxOpen == WHICH_IMAGE)
        {
            sfml_util::Loaders::Texture(
                treasureTexture_,
                game::GameDataFile::Instance()->GetMediaPath("media-images-lockbox-open"));
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
            auto const NUM_CHARACTERS{
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
                 bottomImage_.Height() };
    }

    void TreasureDisplayStage::SetupInitial_BackgroundImage()
    {
        sfml_util::Loaders::Texture(
            backgroundTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2"));

        backgroundSprite_.setTexture(backgroundTexture_);
        backgroundSprite_.setPosition(0.0f, 0.0f);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        backgroundSprite_.setScale(
            MEASUREMENTS.screenWidth / backgroundSprite_.getLocalBounds().width,
            MEASUREMENTS.screenHeight / backgroundSprite_.getLocalBounds().height);
    }

    void TreasureDisplayStage::SetupInitial_Ouroboros()
    {
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("TreasureDisplayStage's", true);
        EntityAdd(ouroborosUPtr_.get());
    }

    void TreasureDisplayStage::SetupAfterPleaseWait_CorpseImage()
    {
        sfml_util::Loaders::Texture(
            corpseTexture_,
            game::GameDataFile::Instance()->GetMediaPath(CorpseImageKeyFromEnemyParty()));

        corpseSprite_.setTexture(corpseTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        sfml_util::ScaleSpriteToFit(corpseSprite_, (SCREEN_WIDTH * 0.75f), (SCREEN_HEIGHT * 0.5f));

        auto const CORPSE_IMAGE_LEFT{ (SCREEN_WIDTH * 0.5f)
                                      - (corpseSprite_.getGlobalBounds().width * 0.5f) };

        auto const CORPSE_IMAGE_TOP{ bottomImage_.Top() - corpseSprite_.getGlobalBounds().height };

        corpseSprite_.setPosition(CORPSE_IMAGE_LEFT, CORPSE_IMAGE_TOP);
        corpseSprite_.setColor(sf::Color(255, 255, 255, 50));
    }

    // The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_TreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        auto const TREASURE_IMAGE_KEY{ item::TreasureImage::ToImageKey(WHICH_IMAGE) };

        auto const TREASURE_IMAGE_SCALE_NEED_REDUCTION{
            (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-1")
            || (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-2")
        };

        sfml_util::Loaders::Texture(
            treasureTexture_, game::GameDataFile::Instance()->GetMediaPath(TREASURE_IMAGE_KEY));

        treasureSprite_.setTexture(treasureTexture_);

        auto const TREASURE_IMAGE_SCALE_ADJ{ (
            (TREASURE_IMAGE_SCALE_NEED_REDUCTION) ? 0.75f : 1.0f) };

        auto const TREASURE_IMAGE_MAX_WIDTH{ (
            CreateDisplayMeasurements().screenWidth * 0.36f * TREASURE_IMAGE_SCALE_ADJ) };

        auto const TREASURE_IMAGE_MAX_HEIGHT{ (
            CreateDisplayMeasurements().screenHeight * 0.250f * TREASURE_IMAGE_SCALE_ADJ) };

        sfml_util::ScaleSpriteToFit(
            treasureSprite_, TREASURE_IMAGE_MAX_WIDTH, TREASURE_IMAGE_MAX_HEIGHT);

        treasureSprite_.setPosition(
            sfml_util::MapByRes(50.0f, 350.0f),
            titleImage_.Bottom() - sfml_util::MapByRes(40.0f, 250.0f));

        treasureSprite_.setColor(sf::Color(255, 255, 255, 192));
    }

    // The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_CoinsImage()
    {
        sfml_util::Loaders::Texture(
            coinsTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-coins"));

        coinsSprite_.setTexture(coinsTexture_);

        auto const COINS_IMAGE_WIDTH{ (sfml_util::Display::Instance()->GetWinWidth() * 0.125f) };
        auto const COINS_SCALE{ COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width };
        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));

        auto const COINS_LEFT{ treasureSprite_.getPosition().x
                               + (treasureSprite_.getGlobalBounds().width * 0.80f) };

        auto const COINS_TOP{ treasureSprite_.getPosition().y
                              + (treasureSprite_.getGlobalBounds().height * 0.75f) };

        coinsSprite_.setPosition(COINS_LEFT, COINS_TOP);
    }

    const std::string TreasureDisplayStage::CorpseImageKeyFromEnemyParty() const
    {
        auto const DEAD_ENEMY_CHARACTERS_PVEC{ combat::Encounter::Instance()->DeadNonPlayers() };

        misc::StrVec_t corpseKeyStrVec;
        for (auto const & NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {
            auto const CORPSE_KEY_STR_VEC{ creature::race::CorpseImageKeys(
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

            auto const DEFAULT_CORPSE_KEY_STR_VEC{ creature::race::CorpseImageKeys(
                creature::race::Human) };

            return misc::Vector::SelectRandom(DEFAULT_CORPSE_KEY_STR_VEC);
        }
        else
        {
            // Allow duplicates in corpseKeyStrVec so that the more a race was faced
            // during combat means the more likely that corpse image is shown.
            return misc::Vector::SelectRandom(corpseKeyStrVec);
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

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const TREASURE_ONSCREEN_POS_V{ sf::Vector2f(
            MEASUREMENTS.treasureListboxRegion.left, MEASUREMENTS.treasureListboxRegion.top) };

        auto const TREASURE_OFFSCREEN_POS_V{ sf::Vector2f(
            (MEASUREMENTS.treasureListboxRegion.width * -1.0f) - 10.0f,
            MEASUREMENTS.treasureListboxRegion.top) };

        stageMoverUPtr_->AddTreasureObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(treasureListboxUPtr_.get()),
            TREASURE_ONSCREEN_POS_V,
            TREASURE_OFFSCREEN_POS_V);
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListbox()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const INVENTORY_ONSCREEN_POS_V{ sf::Vector2f(
            MEASUREMENTS.inventoryListboxRegion.left, MEASUREMENTS.inventoryListboxRegion.top) };

        auto const INVENTORY_OFFSCREEN_POS_V{ sf::Vector2f(
            CalculateHorizOffscreenPos(), MEASUREMENTS.inventoryListboxRegion.top) };

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(inventoryListboxUPtr_.get()),
            INVENTORY_ONSCREEN_POS_V,
            INVENTORY_OFFSCREEN_POS_V);
    }

    void TreasureDisplayStage::SetupForCollection_TreasureListboxLabel()
    {
        SetupTreasure_ListboxLabel();

        stageMoverUPtr_->AddTreasureObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(treasureLabelUPtr_.get()),
            treasureLabelUPtr_->GetEntityPos(),
            sf::Vector2f(-750.0f, treasureLabelUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_TreasureListboxSortIcons()
    {
        SetupTreasure_ListboxSortIcons();

        // The -300.0f here works because it is (the negative of) anything over 256.0f,
        // which is the max dimmension of all listbox sort icon images.
        auto const OFFSCREEN_POS_HORIZ{ -300.0f };

        stageMoverUPtr_->AddTreasureObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(treasureAlphaButtonUPtr_.get()),
            treasureAlphaButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureAlphaButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddTreasureObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(treasureMoneyButtonUPtr_.get()),
            treasureMoneyButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureMoneyButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddTreasureObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(treasureWeightButtonUPtr_.get()),
            treasureWeightButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, treasureWeightButtonUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListboxLabel()
    {
        SetupInventory_ListboxLabel();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(inventoryLabelUPtr_.get()),
            inventoryLabelUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryLabelUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryCoinsText()
    {
        SetupInventory_CoinsText();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(coinsTextUPtr_.get()),
            coinsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), coinsTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryGemsText()
    {
        SetupInventory_GemsText();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(gemsTextUPtr_.get()),
            gemsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), gemsTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryCharacterImage()
    {
        SetupInventory_CharacterImage();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(characterImageUPtr_.get()),
            characterImageUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), characterImageUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryWeightText()
    {
        SetupInventory_WeightText();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(weightTextUPtr_.get()),
            weightTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), weightTextUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryRedXImage()
    {
        SetupInventory_RedXImage();

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(redXImageUPtr_.get()),
            inventoryListboxUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryListboxUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InventoryListboxSortIcons()
    {
        SetupInventory_ListboxSortIcons();

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const OFFSCREEN_POS_HORIZ{ MEASUREMENTS.screenWidth + MEASUREMENTS.listboxWidth };

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(inventoryAlphaButtonUPtr_.get()),
            inventoryAlphaButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryAlphaButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(inventoryMoneyButtonUPtr_.get()),
            inventoryMoneyButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryMoneyButtonUPtr_->GetEntityPos().y));

        stageMoverUPtr_->AddInventoryObject(
            sfml_util::gui::IGuiEntityPtrOpt_t(inventoryWeightButtonUPtr_.get()),
            inventoryWeightButtonUPtr_->GetEntityPos(),
            sf::Vector2f(OFFSCREEN_POS_HORIZ, inventoryWeightButtonUPtr_->GetEntityPos().y));
    }

    void TreasureDisplayStage::SetupForCollection_InstructionsText()
    {
        const sfml_util::gui::TextInfo TEXT_INFO(
            "(press spacebar to change treasure, use arrows or numbers to change characters)",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDark(),
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Left);

        // initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        instrTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInstructionText", TEXT_INFO, EMPTY_RECT);

        // the +93 and -10 were found by experiment to look better
        instrTextUPtr_->SetEntityPos(
            (CreateDisplayMeasurements().screenWidth * 0.5f)
                - (instrTextUPtr_->GetEntityRegion().width * 0.5f) + 93.0f,
            titleImage_.Bottom(false) - 10.0f);

        EntityAdd(instrTextUPtr_.get());
    }

    void TreasureDisplayStage::SetupForCollection_LowerButtons()
    {
        auto const TAKEALL_POS_TOP{ instrTextUPtr_->GetEntityPos().y
                                    + instrTextUPtr_->GetEntityRegion().height
                                    + sfml_util::MapByRes(15.0f, 100.0f) };

        SetupLowerButton(takeAllButtonUPtr_, "(T)ake All", TAKEALL_POS_TOP);

        auto const EXIT_POS_TOP{ (TAKEALL_POS_TOP + takeAllButtonUPtr_->GetEntityRegion().height)
                                 - sfml_util::SpacerOld(50.0f) };

        SetupLowerButton(doneButtonUPtr_, "(E)xit", EXIT_POS_TOP);
    }

    void TreasureDisplayStage::SetupLowerButton(
        sfml_util::gui::FourStateButtonUPtr_t & buttonUPtr,
        const std::string & TEXT,
        const float VERT_POS)
    {
        auto const COLOR_UP{ sfml_util::FontManager::Color_GrayDarker() };
        auto const COLOR_OVER{ COLOR_UP - sf::Color(0, 0, 0, 127) };
        auto const COLOR_DOWN{ sf::Color::Black };

        sfml_util::gui::TextInfo textInfo(
            TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            COLOR_UP,
            sfml_util::Justified::Left);

        const sfml_util::gui::MouseTextInfo MOUSE_TEXT_INFO{ textInfo, COLOR_DOWN, COLOR_OVER };

        GetGuiEntityPtrAndRemoveIfNeeded(buttonUPtr);

        buttonUPtr = std::make_unique<sfml_util::gui::FourStateButton>(
            "TreasureStage's" + TEXT, 0.0f, 0.0f, "", "", "", "", MOUSE_TEXT_INFO);

        auto const HORIZ_POS{ (CreateDisplayMeasurements().screenWidth * 0.5f)
                              - (buttonUPtr->GetEntityRegion().width * 0.5f) };

        buttonUPtr->SetEntityPos(HORIZ_POS, VERT_POS);
        buttonUPtr->SetCallbackHandler(this);
        EntityAdd(buttonUPtr.get());
    }

    void TreasureDisplayStage::SetupListbox(
        const treasure::WhichListbox WHICH_LISTBOX,
        sfml_util::gui::ListBoxUPtr_t & listboxUPtr,
        const item::ItemPVec_t & ITEMS_PVEC)
    {
        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(listboxUPtr) };

        treasure::ListboxColors listboxColors;

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const LISTBOX_REGION{ (
            (WHICH_LISTBOX == treasure::WhichListbox::Treasure)
                ? MEASUREMENTS.treasureListboxRegion
                : MEASUREMENTS.inventoryListboxRegion) };

        const sfml_util::gui::box::Info LISTBOX_BOXINFO(
            1,
            true,
            LISTBOX_REGION,
            listboxColors.colorSet,
            sfml_util::gui::BackgroundInfo(listboxColors.background));

        listboxUPtr = std::make_unique<sfml_util::gui::ListBox>(
            "TreasureDisplayStage's_CharacterInventoryListBox",
            LISTBOX_REGION,
            sfml_util::IStagePtr_t(this),
            LISTBOX_BOXINFO,
            listboxColors.line,
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this),
            listboxColors.image);

        sfml_util::gui::TextInfo textInfo(
            "",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker());

        sfml_util::gui::ListBoxItemFactory listBoxItemFactory;
        for (auto const & ITEM_PTR : ITEMS_PVEC)
        {
            textInfo.text = ITEM_PTR->Name();

            listboxUPtr->Add(
                listBoxItemFactory.Make(listboxUPtr->GetEntityName(), textInfo, ITEM_PTR));
        }

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(PREV_ENTITY_PTR, listboxUPtr.get());
    }

    void TreasureDisplayStage::SetupTreasure_ListboxLabel()
    {
        auto const LABEL_TEXT{ [&]() -> std::string {
            if (TreasureSource() == stage::treasure::Type::Held)
            {
                return "Items Worn by Enemies";
            }
            else
            {
                return "Items in the " + item::TreasureImage::ToContainerName(treasureImage_, true);
            }
        }() };

        const sfml_util::gui::TextInfo TEXT_INFO(
            LABEL_TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(treasureLabelUPtr_) };

        // initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        treasureLabelUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sTreasureListboxLabel", TEXT_INFO, EMPTY_RECT);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const LEFT{ MEASUREMENTS.treasureListboxRegion.left };

        auto const TOP{ (MEASUREMENTS.treasureListboxRegion.top
                         - treasureLabelUPtr_->GetEntityRegion().height)
                        + 10.0f };

        treasureLabelUPtr_->SetEntityPos(LEFT, TOP);

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(PREV_ENTITY_PTR, treasureLabelUPtr_.get());
    }

    void TreasureDisplayStage::SetupTreasure_ListboxSortIcons()
    {
        sf::Sprite alphaSprite;
        sf::Sprite moneySprite;
        sf::Sprite weightSprite;

        SetupSortSprites(
            CreateDisplayMeasurements().treasureListboxLeft,
            alphaSprite,
            moneySprite,
            weightSprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Alpha",
            treasureAlphaButtonUPtr_,
            alphaSprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Money",
            treasureMoneyButtonUPtr_,
            moneySprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_TreasureListboxSortIcon_Weight",
            treasureWeightButtonUPtr_,
            weightSprite);
    }

    void TreasureDisplayStage::SetupInventory_ListboxSortIcons()
    {
        sf::Sprite alphaSprite;
        sf::Sprite moneySprite;
        sf::Sprite weightSprite;

        SetupSortSprites(
            CreateDisplayMeasurements().inventoryListboxLeft,
            alphaSprite,
            moneySprite,
            weightSprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Alpha",
            inventoryAlphaButtonUPtr_,
            alphaSprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Money",
            inventoryMoneyButtonUPtr_,
            moneySprite);

        CreateOrReplaceListboxIconImage(
            "TreasureDisplayStage's_InventoryListboxSortIcon_Weight",
            inventoryWeightButtonUPtr_,
            weightSprite);
    }

    void TreasureDisplayStage::SetupInventory_CharacterImage()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };

        sfml_util::gui::CreatureImageLoader creatureImageLoader;
        creatureImageLoader.Load(characterTexture_, CREATURE_PTR);
        sfml_util::Invert(characterTexture_);
        sfml_util::Mask(characterTexture_, sf::Color::White);

        sf::Sprite sprite;
        sprite.setTexture(characterTexture_, true);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        sprite.setScale(MEASUREMENTS.characterImageScale, MEASUREMENTS.characterImageScale);

        sprite.setPosition(
            MEASUREMENTS.characterImageLeft,
            inventoryListboxUPtr_->GetEntityPos().y
                - (sfml_util::gui::CreatureImageLoader::MaxDimmension()
                   * MEASUREMENTS.characterImageScale));

        sprite.setColor(sf::Color(255, 255, 255, 127));

        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(characterImageUPtr_) };

        characterImageUPtr_ = std::make_unique<sfml_util::gui::GuiImage>(
            "TreasureDisplayStage's_CharacterImage", sprite.getGlobalBounds(), sprite);

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(PREV_ENTITY_PTR, characterImageUPtr_.get());
    }

    void TreasureDisplayStage::SetupInventory_ListboxLabel()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        auto const IS_BEAST{ CREATURE_PTR->IsBeast() };

        SetupInventoryText(
            inventoryLabelUPtr_,
            "ListboxLabel",
            ((IS_BEAST) ? "(beasts cannot carry items)" : CREATURE_PTR->Name()),
            CalculateInventoryTextPosLeft(),
            characterImageUPtr_->GetEntityPos().y + sfml_util::MapByRes(25.0f, 50.0f),
            sfml_util::FontManager::Instance()->Size_Large());
    }

    void TreasureDisplayStage::SetupInventory_CoinsText()
    {
        std::ostringstream ss;
        ss << "Coins: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Coins() << "/";

            Coin_t coinSum{ 0_coin };
            for (auto const & NEXT_CREATURE_PTR :
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
            sfml_util::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_GemsText()
    {
        std::ostringstream ss;
        ss << "Gems: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Gems() << "/";

            Gem_t gemSum{ 0_gem };
            for (auto const & NEXT_CREATURE_PTR :
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
            sfml_util::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_WeightText()
    {
        std::ostringstream ss;
        ss << "Weight: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Weight() << "/" << CREATURE_PTR->WeightCanCarry()
               << "  ("
               << static_cast<int>(
                      (CREATURE_PTR->Inventory().Weight().As<float>()
                       / CREATURE_PTR->WeightCanCarry().As<float>())
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
            sfml_util::FontManager::Instance()->Size_Normal());
    }

    void TreasureDisplayStage::SetupInventory_RedXImage()
    {
        if (WhichCharacterInventoryIsDisplayed()->IsBeast())
        {
            sfml_util::Loaders::Texture(
                redXTexture_, game::GameDataFile::Instance()->GetMediaPath("media-images-misc-x"));
        }
        else
        {
            redXTexture_ = sf::Texture();
        }

        sf::Sprite sprite(redXTexture_);
        sprite.setColor(sf::Color(192, 0, 0, 100));

        auto const INVENTORY_RECT{ inventoryListboxUPtr_->GetEntityRegion() };
        sfml_util::CenterAndScaleSpriteToFit(sprite, INVENTORY_RECT);

        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(redXImageUPtr_) };

        redXImageUPtr_ = std::make_unique<sfml_util::gui::GuiImage>(
            "TreasureDisplayStage's_RedXImage", INVENTORY_RECT, sprite);

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(
            PREV_ENTITY_PTR, redXImageUPtr_.get(), StageAddEntity::Wont);
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
        sfml_util::gui::TextRegionUPtr_t & textRegionUPtr,
        const std::string & NAME,
        const std::string & TEXT,
        const float HORIZ_POS,
        const float VERT_POS,
        const unsigned int FONT_SIZE)
    {
        const sfml_util::gui::TextInfo TEXT_INFO(
            TEXT,
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            FONT_SIZE,
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(textRegionUPtr) };

        // initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        textRegionUPtr = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInventory" + NAME, TEXT_INFO, EMPTY_RECT);

        textRegionUPtr->SetEntityPos(HORIZ_POS, VERT_POS);

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(PREV_ENTITY_PTR, textRegionUPtr.get());
    }

    float TreasureDisplayStage::CalculateInventoryTextPosLeft() const
    {
        return characterImageUPtr_->GetEntityPos().x
            + (sfml_util::gui::CreatureImageLoader::MaxDimmension()
               * CreateDisplayMeasurements().characterImageScale);
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
            auto const SELECTED_ITEM_PTR_OPT{ treasureListboxUPtr_->AtPos(MOUSE_POS) };

            if (SELECTED_ITEM_PTR_OPT && SELECTED_ITEM_PTR_OPT.value()->ItemPtrOpt())
            {
                return treasure::ItemDetails(
                    treasureListboxUPtr_->FullItemRect(SELECTED_ITEM_PTR_OPT.value()),
                    SELECTED_ITEM_PTR_OPT.value()->ItemPtrOpt().value());
            }
        }

        if (inventoryListboxUPtr_)
        {
            auto const SELECTED_ITEM_PTR_OPT{ inventoryListboxUPtr_->AtPos(MOUSE_POS) };

            if (SELECTED_ITEM_PTR_OPT && SELECTED_ITEM_PTR_OPT.value()->ItemPtrOpt())
            {
                return treasure::ItemDetails(
                    inventoryListboxUPtr_->FullItemRect(SELECTED_ITEM_PTR_OPT.value()),
                    SELECTED_ITEM_PTR_OPT.value()->ItemPtrOpt().value());
            }
        }

        return boost::none;
    }

    void TreasureDisplayStage::SetupSortSprites(
        const float LISTBOX_LEFT,
        sf::Sprite & alphaSprite,
        sf::Sprite & moneySprite,
        sf::Sprite & weightSprite)
    {
        sfml_util::Loaders::Texture(
            listboxSortIconAlphaTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-abc"));

        sfml_util::Loaders::Texture(
            listboxSortIconMoneyTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-money-bag"));

        sfml_util::Loaders::Texture(
            listboxSortIconWeightTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-misc-weight"));

        alphaSprite.setTexture(listboxSortIconAlphaTexture_, true);
        moneySprite.setTexture(listboxSortIconMoneyTexture_, true);
        weightSprite.setTexture(listboxSortIconWeightTexture_, true);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const SCALE{ MEASUREMENTS.listboxSortIconScale };
        alphaSprite.setScale(SCALE, SCALE);
        moneySprite.setScale(SCALE, SCALE);
        weightSprite.setScale(SCALE, SCALE);

        auto const ICON_POS_HORIZ_ABC{ ((LISTBOX_LEFT + MEASUREMENTS.listboxWidth)
                                        - (alphaSprite.getGlobalBounds().width
                                           + moneySprite.getGlobalBounds().width
                                           + weightSprite.getGlobalBounds().width))
                                       - (MEASUREMENTS.listboxSortIconSpacer * 2.25f) };

        auto const ICON_POS_VERT_ABC{ MEASUREMENTS.listboxSortIconTop
                                      - (alphaSprite.getGlobalBounds().height * 0.5f) };

        auto const ICON_POS_HORIZ_MONEY{ ICON_POS_HORIZ_ABC + alphaSprite.getGlobalBounds().width
                                         + MEASUREMENTS.listboxSortIconSpacer };

        auto const ICON_POS_VERT_MONEY{ MEASUREMENTS.listboxSortIconTop
                                        - (moneySprite.getGlobalBounds().height * 0.5f) };

        auto const ICON_POS_HORIZ_WEIGHT{ ICON_POS_HORIZ_MONEY + moneySprite.getGlobalBounds().width
                                          + MEASUREMENTS.listboxSortIconSpacer };

        auto const ICON_POS_VERT_WEIGHT{ MEASUREMENTS.listboxSortIconTop
                                         - (weightSprite.getGlobalBounds().height * 0.5f) };

        alphaSprite.setPosition(ICON_POS_HORIZ_ABC, ICON_POS_VERT_ABC);
        moneySprite.setPosition(ICON_POS_HORIZ_MONEY, ICON_POS_VERT_MONEY);
        weightSprite.setPosition(ICON_POS_HORIZ_WEIGHT, ICON_POS_VERT_WEIGHT);

        stage::treasure::ListboxColors listboxColors;
        alphaSprite.setColor(listboxColors.icon);
        moneySprite.setColor(listboxColors.icon);
        weightSprite.setColor(listboxColors.icon);
    }

    void TreasureDisplayStage::GuiEntityPtrAddCurrAndReplacePrevIfNeeded(
        const sfml_util::gui::IGuiEntityPtrOpt_t PREV_GUI_ENTITY_PTR_OPT,
        const sfml_util::gui::IGuiEntityPtr_t CURR_GUI_ENTITY_PTR,
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
        sfml_util::gui::FourStateButtonUPtr_t & sortButtonUPtr,
        sf::Sprite & sprite)
    {
        auto const PREV_ENTITY_PTR{ GetGuiEntityPtrAndRemoveIfNeeded(sortButtonUPtr) };

        sortButtonUPtr = std::make_unique<sfml_util::gui::FourStateButton>(
            NAME, sprite.getPosition().x, sprite.getPosition().y, *sprite.getTexture(), true);

        sortButtonUPtr->Color(sprite.getColor());
        sortButtonUPtr->Scale(sprite.getScale().x);
        sortButtonUPtr->SetCallbackHandler(this);

        GuiEntityPtrAddCurrAndReplacePrevIfNeeded(PREV_ENTITY_PTR, sortButtonUPtr.get());
    }

} // namespace stage
} // namespace heroespath
