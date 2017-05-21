//
// combat-sound-effects.hpp
//
#include "combat-sound-effects.hpp"

#include "sfml-util/sound-manager.hpp"

#include "game/item/item.hpp"
#include "game/log-macros.hpp"


namespace game
{
namespace combat
{

    CombatSoundEffects::CombatSoundEffects()
    {}


    void CombatSoundEffects::PlayShoot(const item::ItemSPtr_t & WEAPON_SPTR)
    {
        if (WEAPON_SPTR.get() == nullptr)
        {
            M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayShoot() was given a nullptr WEAPON_SPTR.");
            return;
        }

        auto const WEAPON_TYPE{ WEAPON_SPTR->WeaponType() };

        if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
        {
            sfml_util::SoundManager::Instance()->SoundEffectsSet_BlowpipeShoot()->PlayRandom();
        }
        else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
        {
            sfml_util::SoundManager::Instance()->SoundEffectsSet_ArrowShoot()->PlayRandom();
        }
        else
        {
            M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayShoot(weapon=\"" << WEAPON_SPTR->Name() << "\", category=\"" << item::category::ToString(WEAPON_SPTR->Category(), false) << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false) << ") Unable to find a 'shoot' sound effect to fit that weapon_type.");
        }
    }


    void CombatSoundEffects::PlayHitOrMiss(const HitInfo & HIT_INFO)
    {
        auto const WEAPON_SPTR{ HIT_INFO.Weapon() };
        if (WEAPON_SPTR.get() == nullptr)
        {
            M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayHitOrMiss() was given a nullptr WEAPON_SPTR.");
            return;
        }

        auto const WEAPON_TYPE{ WEAPON_SPTR->WeaponType() };

        if (HIT_INFO.WasHit())
        {
            if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_BlowpipeHit()->PlayRandom();
            }
            else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_ArrowHit()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Fists)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_FistHit()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Tendrils)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_TendrilHit()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Breath)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_BreathHit()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Claws)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_ClawTear()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_WhipHit()->PlayRandom();
            }
            else
            {
                switch (WEAPON_SPTR->MaterialPrimary())
                {

                    case item::material::Wood:              { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitWood); }
                    case item::material::HardLeather:
                    case item::material::SoftLeather:       { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitLeather); }
                    case item::material::Bone:              { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitBone); }

                    case item::material::Water:
                    case item::material::Acid:
                    case item::material::Blood:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitLiquid); }

                    case item::material::Plant:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitPlant); }

                    case item::material::Flesh:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitFlesh); }

                    case item::material::Scale:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitScale); }

                    case item::material::Hide:              { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitHide); }

                    case item::material::Stone:
                    case item::material::Obsidian:
                    case item::material::Amethyst:
                    case item::material::Emerald:
                    case item::material::Pearl:
                    case item::material::Ruby:
                    case item::material::Lapis:
                    case item::material::Sapphire:
                    case item::material::Diamond:           { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitStone); }

                    case item::material::Rope:
                    case item::material::Cloth:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitCloth); }

                    case item::material::Claw:
                    case item::material::Horn:              { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitHorn); }

                    case item::material::Tooth:             { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitTooth); }

                    case item::material::Tin:
                    case item::material::Gold:
                    case item::material::Iron:
                    case item::material::Bronze:
                    case item::material::Silver:
                    case item::material::Steel:
                    case item::material::Platinum:          { return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->PlayRandom(); }

                    case item::material::Feathers:
                    case item::material::Fur:
                    case item::material::Hair:
                    case item::material::Glass:
                    case item::material::Gas:
                    case item::material::Paper:
                    case item::material::Ether:
                    case item::material::Dirt:
                    case item::material::Nothing:
                    case item::material::Count:
                    default:
                    {
                        return sfml_util::SoundManager::Instance()->SoundEffectsSet_MaterialHitMisc()->Play(sfml_util::sound_effect::MaterialHitMisc);
                    }
                }
            }
        }
        else
        {
            if (WEAPON_TYPE & item::weapon_type::Projectile)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_ProjectileMiss()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_WhipMiss()->PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Melee)
            {
                sfml_util::SoundManager::Instance()->SoundEffectsSet_MeleeMiss()->PlayRandom();
            }
            else
            {
                M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayHitOrMiss(weapon=\"" << WEAPON_SPTR->Name() << "\", category=\"" << item::category::ToString(WEAPON_SPTR->Category(), false) << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false) << ") Unable to find a 'miss' sound effect to fit that weapon_type.");
            }
        }
    }

}
}
