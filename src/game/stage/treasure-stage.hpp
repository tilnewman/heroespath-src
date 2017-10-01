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
#ifndef GAME_TREASURESTAGE_HPP_INCLUDED
#define GAME_TREASURESTAGE_HPP_INCLUDED
//
// treasure-stage.hpp
//  A Stage class that allows finding treasure and advancing after combat.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include "popup/i-popup-callback.hpp"

#include "game/combat/turn-action-info.hpp"
#include "game/combat/fight-results.hpp"
#include "game/creature/race-enum.hpp"
#include "game/item/treasure-image-enum.hpp"
#include "game/item/item-cache.hpp"
#include "game/item/treasure-available-enum.hpp"
#include "game/trap.hpp"

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

    class TreasureDisplayStage;


    //A Stage class that allows starting the game
    class TreasureStage
    :
        public sfml_util::Stage,
        public popup::IPopupHandler_t
    {
        TreasureStage(const TreasureStage &) =delete;
        TreasureStage & operator=(const TreasureStage &) =delete;

    public:
        TreasureStage();
        virtual ~TreasureStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const popup::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

    private:
        void HandleCountdownAndPleaseWaitPopup();
        void SetupAfterDelay();

        item::TreasureAvailable::Enum DetermineTreasureAvailableState(
            const item::ItemCache & CACHE_HELD,
            const item::ItemCache & CACHE_LOCKBOX);

        void PromptUserBasedonTreasureAvailability(
            const item::TreasureAvailable::Enum, const item::TreasureImage::Enum);

        void SetupStageForTreasureCollectionWithoutLockbox();
        void PromptPlayerWhichCharacterWillPickLock();
        std::size_t FindCharacterIndexWhoPrevAttemptedLockPicking() const;
        const misc::StrVec_t MakeInvalidLockPickCharacterMessages() const;
        bool AreAnyStringsEmpty(const misc::StrVec_t & INVALID_MSGS) const;
        void PromptPlayerWithLockPickPopup(const std::string & CHAR_PICKING_NAME);
        bool DetermineIfLockPickingSucceeded(const creature::CreaturePtr_t) const;

        sfml_util::sound_effect::Enum SelectRandomLockPickingSfx() const;
        sfml_util::sound_effect::Enum SelectRandomTreasureOpeningSfx() const;

        void LockPickSuccess();
        void LockPickFailure();

        enum class DamagePopup
        {
            Displayed,
            AllFinished
        };

        DamagePopup PromptPlayerWithDamagePopups();

        //returns true if all characters were killed by the trap
        bool CheckAndHandleAllKilledByTrap();

        void LockboxOpen();
        void SetupForCollection();
        
    private:
        static const std::string POPUP_NAME_ITEMPROFILE_PLEASEWAIT_;
        static const std::string POPUP_NAME_ALL_ENEMIES_RAN_;
        static const std::string POPUP_NAME_WORN_ONLY_;
        static const std::string POPUP_NAME_LOCKBOX_ONLY_;
        static const std::string POPUP_NAME_LOCKBOX_AND_HELD_;
        static const std::string POPUP_NAME_CHAR_SELECT_;
        static const std::string POPUP_NAME_NO_CHARS_CAN_PICK_THE_LOCK_;
        static const std::string POPUP_NAME_LOCK_PICK_ATTEMPT_;
        static const std::string POPUP_NAME_LOCK_PICK_FAILURE_;
        static const std::string POPUP_NAME_DAMAGE_REPORT_;
        static const std::string POPUP_NAME_LOCKBOX_OPEN_;
        static const std::string POPUP_NAME_ALL_CHARACTERS_DIED_;

    private:
        TreasureDisplayStage * displayStagePtr_;
        int setupCountdown_;
        item::TreasureImage::Enum treasureImageType_;
        item::ItemCache itemCacheHeld_;
        item::ItemCache itemCacheLockbox_;
        item::TreasureAvailable::Enum treasureAvailable_;
        game::Trap trap_;
        combat::FightResult fightResult_;
        std::size_t creatureEffectIndex_;
    };

}
}
#endif //GAME_TREASURESTAGE_HPP_INCLUDED
