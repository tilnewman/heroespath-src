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
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/list-box-event-packet.hpp"
#include "stage/stage-base.hpp"
#include "stage/treasure-stage-mover.hpp" //for treasure::Type::Enumop

#include <memory>
#include <string>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item

namespace sfml_util
{
    template <typename Stage_t, typename Element_t>
    class ListBox;

    template <typename Stage_t, typename Element_t>
    using ListBoxPtr_t = misc::NotNull<ListBox<Stage_t, Element_t> *>;

    template <typename Stage_t, typename Element_t>
    using ListBoxUPtr_t = std::unique_ptr<ListBox<Stage_t, Element_t>>;

} // namespace sfml_util

namespace stage
{

    class TreasureDisplayStage;
    using TreasureDisplayStagePtr_t = misc::NotNull<TreasureDisplayStage *>;
    using TreasureDisplayStagePtrOpt_t = boost::optional<TreasureDisplayStagePtr_t>;

    // A Stage class that allows starting the game
    class TreasureStage
        : public stage::StageBase

        , public sfml_util::PopupCallback_t::IHandler_t
    {
        using ItemListBox_t = sfml_util::ListBox<TreasureDisplayStage, item::ItemPtr_t>;
        using ItemListBoxPtr_t = sfml_util::ListBoxPtr_t<TreasureDisplayStage, item::ItemPtr_t>;
        using ItemListBoxUPtr_t = sfml_util::ListBoxUPtr_t<TreasureDisplayStage, item::ItemPtr_t>;

    public:
        TreasureStage(const TreasureStage &) = delete;
        TreasureStage(TreasureStage &&) = delete;
        TreasureStage & operator=(const TreasureStage &) = delete;
        TreasureStage & operator=(TreasureStage &&) = delete;

        TreasureStage();
        virtual ~TreasureStage();

        void SetViewStage(TreasureDisplayStagePtr_t viewStagePtr)
        {
            displayStagePtrOpt_ = viewStagePtr;
        }

        bool HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        bool HandleListboxCallback(
            const ItemListBoxPtr_t & TREASURE_LISTBOX_PTR,
            const ItemListBoxPtr_t & INVENTORY_LISTBOX_PTR,
            const sfml_util::Callback<
                sfml_util::ListBoxEventPacket<TreasureDisplayStage, item::ItemPtr_t>>::PacketPtr_t &
                PACKET_PTR);

        void TakeAllItems();
        void Exit();

    private:
        item::TreasureAvailable::Enum DetermineTreasureAvailableState(
            const item::ItemCache & CACHE_HELD, const item::ItemCache & CACHE_LOCKBOX);

        void PromptUserBasedOnTreasureAvailability(
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

        TreasureDisplayStagePtrOpt_t displayStagePtrOpt_;
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

    using TreasureStagePtr_t = misc::NotNull<TreasureStage *>;
    using TreasureStagePtrOpt_t = boost::optional<TreasureStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_TREASURESTAGE_HPP_INCLUDED
