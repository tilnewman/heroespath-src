//
// combat-sound-effects.hpp
//
#include "combat-sound-effects.hpp"

#include "sfml-util/static-sounds.hpp"
#include "sfml-util/sound-manager.hpp"

#include "heroespath/item/item.hpp"


namespace heroespath
{
namespace combat
{

    CombatSoundEffects::CombatSoundEffects()
    :
        blowpipeShootSoundsSPtr_   (sfml_util::SoundManager::Instance()->StaticSounds_BlowpipeShoot()),
        blowpipeHitSoundsSPtr_     (sfml_util::SoundManager::Instance()->StaticSounds_BlowpipeHit()),
        arrowShootSoundsSPtr_      (sfml_util::SoundManager::Instance()->StaticSounds_ArrowShoot()),
        arrowHitSoundsSPtr_        (sfml_util::SoundManager::Instance()->StaticSounds_ArrowHit()),
        projectileMissSoundsSPtr_  (sfml_util::SoundManager::Instance()->StaticSounds_ProjectileMiss()),
        meleeMissSoundsSPtr_       (sfml_util::SoundManager::Instance()->StaticSounds_MeleeMiss()),
        fistHitSoundsSPtr_         (sfml_util::SoundManager::Instance()->StaticSounds_FistHit()),
        breathHitSoundsSPtr_       (sfml_util::SoundManager::Instance()->StaticSounds_BreathHit()),
        tendrilHitSoundsSPtr_      (sfml_util::SoundManager::Instance()->StaticSounds_TendrilHit()),
        whipHitSoundsSPtr_         (sfml_util::SoundManager::Instance()->StaticSounds_WhipHit()),
        whipMissSoundsSPtr_        (sfml_util::SoundManager::Instance()->StaticSounds_WhipMiss()),
        materialHitMetalSoundsSPtr_(sfml_util::SoundManager::Instance()->StaticSounds_MaterialHitMetal()),
        materialHitMiscSoundsSPtr_ (sfml_util::SoundManager::Instance()->StaticSounds_MaterialHitMisc()),
        clawTearSoundsSPtr_        (sfml_util::SoundManager::Instance()->StaticSounds_ClawTear())
    {}


    CombatSoundEffects::~CombatSoundEffects()
    {
        blowpipeShootSoundsSPtr_->ReleaseAll();
        blowpipeHitSoundsSPtr_->ReleaseAll();
        arrowShootSoundsSPtr_->ReleaseAll();
        arrowHitSoundsSPtr_->ReleaseAll();
        projectileMissSoundsSPtr_->ReleaseAll();
        meleeMissSoundsSPtr_->ReleaseAll();
        fistHitSoundsSPtr_->ReleaseAll();
        breathHitSoundsSPtr_->ReleaseAll();
        tendrilHitSoundsSPtr_->ReleaseAll();
        whipHitSoundsSPtr_->ReleaseAll();
        whipMissSoundsSPtr_->ReleaseAll();
        materialHitMetalSoundsSPtr_->ReleaseAll();
        materialHitMiscSoundsSPtr_->ReleaseAll();
        clawTearSoundsSPtr_->ReleaseAll();
    }


    void CombatSoundEffects::PlayProjectileShoot(const item::ItemSPtr_t & WEAPON_SPTR)
    {
        if (WEAPON_SPTR.get() != nullptr)
        {
            auto const WEAPON_TYPE{ WEAPON_SPTR->WeaponType() };

            if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
            {
                blowpipeShootSoundsSPtr_->PlayRandom();
            }
            else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
            {
                arrowShootSoundsSPtr_->PlayRandom();
            }
        }
    }


    void CombatSoundEffects::Play(const FightResult & )
    {
        /*auto const HIT_INFO_VEC{ FIGHT_RESULT.HitInfoVec() };
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            Play(NEXT_HIT_INFO);*/
    }


    void CombatSoundEffects::Play(const HitInfo & HIT_INFO)
    {
        auto const WEAPON_SPTR{ HIT_INFO.Weapon() };

        if (WEAPON_SPTR.get() != nullptr)
        {
            auto const WEAPON_TYPE{ WEAPON_SPTR->WeaponType() };

            if (HIT_INFO.WasHit())
            {
                if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
                {
                    blowpipeHitSoundsSPtr_->PlayRandom();
                }
                else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
                {
                    arrowHitSoundsSPtr_->PlayRandom();
                }
                else if (WEAPON_TYPE & item::weapon_type::Fists)
                {
                    fistHitSoundsSPtr_->PlayRandom();
                }
                else if (WEAPON_TYPE & item::weapon_type::Tendrils)
                {
                    tendrilHitSoundsSPtr_->PlayRandom();
                }
                else if (WEAPON_TYPE & item::weapon_type::Breath)
                {
                    breathHitSoundsSPtr_->PlayRandom();
                }
                else if (WEAPON_TYPE & item::weapon_type::Claws)
                {
                    clawTearSoundsSPtr_->PlayRandom();
                }
                else if (WEAPON_TYPE & item::weapon_type::Whip)
                {
                    whipHitSoundsSPtr_->PlayRandom();
                }
                else
                {
                    switch (WEAPON_SPTR->MaterialPrimary())
                    {

                        case item::material::Wood:              { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitWood); }
                        case item::material::HardLeather:
                        case item::material::SoftLeather:       { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitLeather); }
                        case item::material::Bone:              { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitBone); }

                        case item::material::Water:
                        case item::material::Acid:
                        case item::material::Blood:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitLiquid); }

                        case item::material::Plant:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitPlant); }

                        case item::material::Flesh:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitFlesh); }

                        case item::material::Scale:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitScale); }

                        case item::material::Hide:              { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitHide); }

                        case item::material::Stone:
                        case item::material::Obsidian:
                        case item::material::Amethyst:
                        case item::material::Emerald:
                        case item::material::Pearl:
                        case item::material::Ruby:
                        case item::material::Lapis:
                        case item::material::Sapphire:
                        case item::material::Diamond:           { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitStone); }

                        case item::material::Rope:
                        case item::material::Cloth:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitCloth); }

                        case item::material::Claw:
                        case item::material::Horn:              { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitHorn); }

                        case item::material::Tooth:             { return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitTooth); }

                        case item::material::Tin:
                        case item::material::Gold:
                        case item::material::Iron:
                        case item::material::Bronze:
                        case item::material::Silver:
                        case item::material::Steel:
                        case item::material::Platinum:          { return materialHitMetalSoundsSPtr_->PlayRandom(); }

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
                            return materialHitMiscSoundsSPtr_->Play(sfml_util::sound_effect::MaterialHitMisc);
                        }
                    }
                }
            }
            else
            {
                meleeMissSoundsSPtr_->PlayRandom();
            }
        }
    }

}
}
