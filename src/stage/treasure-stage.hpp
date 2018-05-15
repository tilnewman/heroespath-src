// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TREASURESTAGE_HPP_INCLUDED
#define HEROESPATH_TREASURESTAGE_HPP_INCLUDED
//
// treasure-stage.hpp
//  A Stage class that allows finding treasure and advancing after combat.
//
#include "combat/creature-interaction.hpp"
#include "combat/fight-results.hpp"
#include "combat/trap.hpp"
#include "combat/turn-action-info.hpp"
#include "creature/race-enum.hpp"
#include "interact/lock-interactions.hpp"
#include "item/item-cache.hpp"
#include "item/treasure-available-enum.hpp"
#include "item/treasure-image-enum.hpp"
#include "misc/handy-types.hpp"
#include "misc/not-null.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "stage/treasure-stage-mover.hpp" //for treasure::Type::Enum

#include <memory>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util

namespace stage
{

    class TreasureDisplayStage;
    using TreasureDisplayStagePtr_t = misc::NotNull<TreasureDisplayStage *>;

    // A Stage class that allows starting the game
    class TreasureStage
        : public sfml_util::Stage
        , public popup::IPopupHandler_t
    {
    public:
        TreasureStage(const TreasureStage &) = delete;
        TreasureStage(TreasureStage &&) = delete;
        TreasureStage & operator=(const TreasureStage &) = delete;
        TreasureStage & operator=(TreasureStage &&) = delete;

    public:
        TreasureStage();
        virtual ~TreasureStage();

        const std::string HandlerName() const override { return GetStageName(); }
        bool HandleCallback(const popup::PopupResponse &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        bool HandleListboxCallback(
            const sfml_util::gui::ListBox * const TREASURE_LISTBOX_PTR,
            const sfml_util::gui::ListBox * const INVENTORY_LISTBOX_PTR,
            const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE);

        void TakeAllItems();
        void Exit();

    private:
        item::TreasureAvailable::Enum DetermineTreasureAvailableState(
            const item::ItemCache & CACHE_HELD, const item::ItemCache & CACHE_LOCKBOX);

        void PromptUserBasedonTreasureAvailability(
            const item::TreasureAvailable::Enum, const item::TreasureImage::Enum);

        void SetupForCollectionWithoutLockbox();
        bool DetermineIfLockPickingSucceeded(const creature::CreaturePtr_t) const;

        sfml_util::sound_effect::Enum SelectRandomTreasureOpeningSfx() const;

        void LockPickSuccess();
        void LockPickFailure();

        enum class DamagePopup
        {
            Displayed,
            AllFinished
        };

        DamagePopup PromptPlayerWithDamagePopups();

        // returns true if all characters were killed by the trap
        bool CheckAndHandleAllKilledByTrap();

        void LockboxOpen();

        enum class ShareType
        {
            Coins,
            Gems
        };

        // returns true if a popup was displayed
        bool ShareAndShowPopupIfNeeded(const ShareType);

        int Share(const ShareType);

        void SetupForCollection();

        bool HandleKeypress_Space();
        bool HandleKeypress_Number(const sf::Keyboard::Key);
        bool HandleKeypress_LeftRight(const sf::Keyboard::Key);

        void PlaySoundEffect_KeypressValid() const;
        void PlaySoundEffect_KeypressInvalid() const;

        void TakeItem(const item::ItemPtr_t);
        void PutItemBack(const item::ItemPtr_t);

        void UpdateItemDisplay();

        // returns true if a popup is displayed
        bool ProcessLockpickTitleAndPopupIfNeeded();

        void TransitionToAdventureStage();

    private:
        static const std::string POPUP_NAME_NO_TREASURE_;
        static const std::string POPUP_NAME_WORN_ONLY_;
        static const std::string POPUP_NAME_LOCKBOX_ONLY_;
        static const std::string POPUP_NAME_LOCKBOX_AND_HELD_;
        static const std::string POPUP_NAME_LOCK_PICK_FAILURE_;
        static const std::string POPUP_NAME_DAMAGE_REPORT_;
        static const std::string POPUP_NAME_ALL_CHARACTERS_DIED_;
        static const std::string POPUP_NAME_COIN_SHARE_;
        static const std::string POPUP_NAME_GEM_SHARE_;
        static const std::string POPUP_NAME_ITEM_TAKE_REJECTION_;
        static const std::string POPUP_NAME_ALL_ITEMS_TAKEN_;
        static const std::string POPUP_NAME_NOT_ALL_ITEMS_TAKEN_;

        TreasureDisplayStagePtr_t displayStagePtr_;
        item::TreasureImage::Enum treasureImageType_;
        item::ItemCache itemCacheHeld_;
        item::ItemCache itemCacheLockbox_;
        item::TreasureAvailable::Enum treasureAvailable_;
        combat::Trap trap_;
        combat::FightResult fightResult_;
        std::size_t creatureEffectIndex_;
        bool updateItemDisplayNeeded_;
        bool willProcessLockpickTitle_;
        interact::LockPicking lockPicking_;
        combat::CreatureInteraction creatureInteraction_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_TREASURESTAGE_HPP_INCLUDED
