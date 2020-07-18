// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-sound-effects.hpp
//
#include "combat-sound-effects.hpp"

#include "creature/creature.hpp"
#include "gui/sound-manager.hpp"
#include "item/item.hpp"
#include "misc/log-macros.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace combat
{

    void CombatSoundEffects::PlayShoot(const item::ItemPtr_t WEAPON_PTR)
    {
        const auto PROJECTILE_TYPE { WEAPON_PTR->WeaponInfo().MinorAs<item::Projectiles>() };

        if ((PROJECTILE_TYPE == item::Projectiles::Blowpipe)
            || (PROJECTILE_TYPE == item::Projectiles::Sling))
        {
            Play(Playlist(gui::sound_effect_set::BlowpipeShoot));
        }
        else
        {
            // the sound for bows and crossbows should be different, but I don't have one yet...
            Play(Playlist(gui::sound_effect_set::ArrowShoot));
        }
    }

    void CombatSoundEffects::PlayHitOrMiss(
        const creature::CreaturePtr_t CREATURE_PTR, const HitInfo & HIT_INFO)
    {
        const auto WEAPON_PTR_OPT { HIT_INFO.Weapon() };

        if (!WEAPON_PTR_OPT)
        {
            M_HP_LOG_ERR(
                "No weapon.  HIT_INFO.Weapon() is nullptr.  HIT_INFO=" << HIT_INFO.ToString());

            return;
        }

        const auto WEAPON_PTR = WEAPON_PTR_OPT.value();
        const auto & WEAPON_INFO = WEAPON_PTR->WeaponInfo();

        if (HIT_INFO.WasHit())
        {
            const auto WEAPON_MATERIAL_PRI = WEAPON_PTR->MaterialPrimary();

            if (!Play(HitPlaylist(WEAPON_INFO, WEAPON_MATERIAL_PRI)))
            {
                M_HP_LOG_ERR(
                    "Failed to find any hit sfx to play.  CREATURE="
                    << CREATURE_PTR->ToString() << ", HIT_INFO=" << HIT_INFO.ToString());
            }
        }
        else
        {
            if (!Play(MissPlaylist(WEAPON_INFO)))
            {
                M_HP_LOG_ERR(
                    "Failed to find any miss sfx to play.  CREATURE="
                    << CREATURE_PTR->ToString() << ", HIT_INFO=" << HIT_INFO.ToString());
            }
        }
    }

    void CombatSoundEffects::PlaySpell(const spell::SpellPtr_t SPELL_PTR)
    {
        if (!Play(SpellPlaylist(SPELL_PTR->Which())))
        {
            M_HP_LOG_ERR("Failed to find any spell sfx to play.  SPELL=" << SPELL_PTR->ToString());
        }
    }

    void CombatSoundEffects::PlaySong(const song::SongPtr_t SONG_PTR)
    {
        if (!Play(SongPlaylist(SONG_PTR->Which())))
        {
            M_HP_LOG_ERR("Failed to find any song sfx to play.  SONG=" << SONG_PTR->ToString());
        }
    }

    void CombatSoundEffects::PlayRoar(const creature::CreaturePtr_t CREATURE_PTR)
    {
        if (!Play(RoarPlaylist(CREATURE_PTR)))
        {
            M_HP_LOG_ERR(
                "Failed to find any roar sfx to play.  CREATURE=" << CREATURE_PTR->ToString());
        }
    }

    void CombatSoundEffects::PlayDeath(const creature::CreaturePtr_t CREATURE_PTR)
    {
        if (!Play(DeathPlaylist(CREATURE_PTR)))
        {
            M_HP_LOG_ERR("Failed to find any sfx to play.  CREATURE=" << CREATURE_PTR->ToString());
        }
    }

    bool CombatSoundEffects::Play(const Playlist & PLAYLIST)
    {
        if (!PLAYLIST.sfx_vec.empty())
        {
            gui::SoundManager::Instance()->SoundEffectPlay(misc::RandomSelect(PLAYLIST.sfx_vec));

            if (PLAYLIST.sfx_after < gui::sound_effect::Count)
            {
                gui::SoundManager::Instance()->SoundEffectPlay(
                    PLAYLIST.sfx_after, PLAYLIST.sfx_after_delay_sec);
            }

            return true;
        }
        else if (PLAYLIST.sfx_set < gui::sound_effect_set::Count)
        {
            gui::SoundManager::Instance()->PlayRandomFromSet(PLAYLIST.sfx_set);
            return true;
        }
        else
        {
            return false;
        }
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::RoarPlaylist(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto ROLE { CREATURE_PTR->Role() };

        if (ROLE == creature::role::Wolfen)
        {
            switch (CREATURE_PTR->WolfenClassType())
            {
                case creature::WolfenClass::Pup: return Playlist(gui::sound_effect::RoarWolfenPup);

                case creature::WolfenClass::Juvenile:
                    return Playlist(gui::sound_effect::RoarWolfenJuvenile);

                case creature::WolfenClass::Adult:
                    return Playlist(gui::sound_effect::RoarWolfenAdult);

                case creature::WolfenClass::Noble:
                    return Playlist(gui::sound_effect::RoarWolfenNoble);

                case creature::WolfenClass::Highborn:
                    return Playlist(gui::sound_effect::RoarWolfenHighborn);

                case creature::WolfenClass::Elder:
                    return Playlist(gui::sound_effect::RoarWolfenElder);

                case creature::WolfenClass::Count:
                default: return Playlist();
            }
        }

        if (ROLE == creature::role::Firebrand)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandHatchling);

                case creature::dragon_class::Whelp:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandWhelp);

                case creature::dragon_class::Fledgling:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandFledgling);

                case creature::dragon_class::Juvenile:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandJuvenile);

                case creature::dragon_class::Adult:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandAdult);

                case creature::dragon_class::Wyrm:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandWyrm);

                case creature::dragon_class::Skycaster:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandSkycaster);

                case creature::dragon_class::Elder:
                    return Playlist(gui::sound_effect::RoarDragonFirebrandElder);

                case creature::dragon_class::Count:
                default: return Playlist();
            }
        }

        if (ROLE == creature::role::Sylavin)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                    return Playlist(gui::sound_effect::RoarDragonSylavinHatchling);

                case creature::dragon_class::Whelp:
                    return Playlist(gui::sound_effect::RoarDragonSylavinWhelp);

                case creature::dragon_class::Fledgling:
                    return Playlist(gui::sound_effect::RoarDragonSylavinFledgling);

                case creature::dragon_class::Juvenile:
                    return Playlist(gui::sound_effect::RoarDragonSylavinJuvenile);

                case creature::dragon_class::Adult:
                    return Playlist(gui::sound_effect::RoarDragonSylavinAdult);

                case creature::dragon_class::Wyrm:
                    return Playlist(gui::sound_effect::RoarDragonSylavinWyrm);

                case creature::dragon_class::Skycaster:
                    return Playlist(gui::sound_effect::RoarDragonSylavinSkycaster);

                case creature::dragon_class::Elder:
                    return Playlist(gui::sound_effect::RoarDragonSylavinElder);

                case creature::dragon_class::Count:
                default: return Playlist();
            }
        }

        return Playlist();
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::SpellPlaylist(const spell::Spells::Enum SPELL)
    {
        switch (SPELL)
        {
            case spell::Spells::Sparks: return Playlist(gui::sound_effect::SpellSparks);
            case spell::Spells::Bandage: return Playlist(gui::sound_effect_set::SpellBandage);
            case spell::Spells::Sleep: return Playlist(gui::sound_effect::SpellSleep);
            case spell::Spells::Awaken: return Playlist(gui::sound_effect::SpellAwaken);

            case spell::Spells::Trip:
                return Playlist(gui::sound_effect::SpellTrip, gui::sound_effect::Trip, 1.0f);

            case spell::Spells::Lift: return Playlist(gui::sound_effect::SpellLift);
            case spell::Spells::Daze: return Playlist(gui::sound_effect::SpellDaze);
            case spell::Spells::Panic: return Playlist(gui::sound_effect::SpellFrighten);
            case spell::Spells::ClearMind: return Playlist(gui::sound_effect::SpellClearMind);
            case spell::Spells::Poison: return Playlist(gui::sound_effect::SpellPoison);
            case spell::Spells::Antidote: return Playlist(gui::sound_effect::SpellAntidote);
            case spell::Spells::PoisonCloud: return Playlist(gui::sound_effect::SpellPoisonCloud);

            case spell::Spells::Count:
            default: return Playlist();
        }
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::SongPlaylist(const song::Songs::Enum SONG)
    {
        switch (SONG)
        {
            case song::Songs::RallyDrum: return Playlist(gui::sound_effect::SongRallyDrum);

            case song::Songs::SpiritResonance:
                return Playlist(gui::sound_effect::SongSpiritResonance);

            case song::Songs::RousingRhythm: return Playlist(gui::sound_effect::SongRousingRhythm);
            case song::Songs::TripBeat: return Playlist(gui::sound_effect::SongTripBeat);
            case song::Songs::PanicStrings: return Playlist(gui::sound_effect::SongPanicStrings);
            case song::Songs::Lullaby: return Playlist(gui::sound_effect::SongLullaby);

            case song::Songs::Count:
            default: return Playlist();
        }
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::DeathPlaylist(const creature::CreaturePtr_t CREATURE_PTR)
    {
        switch (CREATURE_PTR->Race())
        {
            case creature::race::Human:
            case creature::race::Gnome:
            {
                Playlist playlist({ gui::sound_effect::DeathHuman1,
                                    gui::sound_effect::DeathHuman2,
                                    gui::sound_effect::DeathHuman3,
                                    gui::sound_effect::DeathHuman4,
                                    gui::sound_effect::DeathHuman5 });

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale1);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale2);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale3);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale4);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale5);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale6);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale7);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanFemale1);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanFemale3);
                }

                return playlist;
            }

            case creature::race::Halfling:
            {
                Playlist playlist(
                    { gui::sound_effect::DeathHuman2, gui::sound_effect::DeathHuman4 });

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale3);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale5);
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanMale6);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    playlist.sfx_vec.emplace_back(gui::sound_effect::DeathHumanFemale2);
                }

                return playlist;
            }

            case creature::race::Pixie:
            {
                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    return Playlist(
                        { gui::sound_effect::DeathHumanMale2, gui::sound_effect::DeathHumanMale4 });
                }
                else
                {
                    return Playlist({ gui::sound_effect::DeathHumanFemale1,
                                      gui::sound_effect::DeathHumanFemale2,
                                      gui::sound_effect::DeathHumanFemale3 });
                }
            }

            case creature::race::Witch:
            case creature::race::Harpy:
            {
                return Playlist({ gui::sound_effect::DeathHumanFemale1,
                                  gui::sound_effect::DeathHumanFemale2,
                                  gui::sound_effect::DeathHumanFemale3 });
            }

            case creature::race::Golem:
            {
                return Playlist(gui::sound_effect::DeathBeastCaveTroll1);
            }

            case creature::race::ThreeHeadedHound:
            case creature::race::Werewolf:
            case creature::race::Wolfen:
            {
                return Playlist(gui::sound_effect::DeathWolf);
            }

            case creature::race::Hydra:
            case creature::race::Dragon:
            case creature::race::Wyvern:
            {
                return Playlist(
                    { gui::sound_effect::DeathDragon1a, gui::sound_effect::DeathDragon1b });
            }

            case creature::race::Newt:
            case creature::race::Goblin:
            case creature::race::Pug:
            {
                return Playlist({ gui::sound_effect::DeathBeastSmallRaspy1,
                                  gui::sound_effect::DeathBeastSmallRaspy2,
                                  gui::sound_effect::DeathBeastSmallRaspy3,
                                  gui::sound_effect::DeathBeastSmallRaspy4 });
            }

            case creature::race::Troll:
            {
                return Playlist({ gui::sound_effect::DeathBeastRough1,
                                  gui::sound_effect::DeathBeastRough2,
                                  gui::sound_effect::DeathBeastRough3 });
            }

            case creature::race::Orc:
            {
                return Playlist(gui::sound_effect::DeathBeast5);
            }

            case creature::race::Beetle:
            case creature::race::Spider:
            {
                return Playlist({ gui::sound_effect::DeathSpider1,
                                  gui::sound_effect::DeathSpider2,
                                  gui::sound_effect::DeathSpider3 });
            }

            case creature::race::Bog:
            {
                if (CREATURE_PTR->Role() == creature::role::Smasher)
                {
                    return Playlist(gui::sound_effect::DeathBeast3);
                }
                else
                {
                    return Playlist(gui::sound_effect::DeathBeastSmallRaspy4);
                }
            }

            case creature::race::CaveCrawler:
            {
                return Playlist(gui::sound_effect::DeathThing1);
            }

            case creature::race::LizardWalker:
            {
                return Playlist(gui::sound_effect::DeathBeastTalker1);
            }

            case creature::race::Minotaur:
            {
                return Playlist(gui::sound_effect::DeathBeast1);
            }

            case creature::race::Ogre:
            {
                return Playlist({ gui::sound_effect::DeathBeastBig1,
                                  gui::sound_effect::DeathBeastBig2,
                                  gui::sound_effect::DeathBeastBig3 });
            }

            case creature::race::Plant:
            {
                return Playlist(gui::sound_effect::DeathBeastWet);
            }

            case creature::race::Shade:
            {
                return Playlist({ gui::sound_effect::DeathGhostly1a,
                                  gui::sound_effect::DeathGhostly1b,
                                  gui::sound_effect::DeathGhostly1c });
            }

            case creature::race::Skeleton:
            {
                return Playlist({ gui::sound_effect::DeathCreepy1,
                                  gui::sound_effect::DeathCreepy2,
                                  gui::sound_effect::DeathCreepy3,
                                  gui::sound_effect::DeathCreepy4,
                                  gui::sound_effect::DeathCreepy5,
                                  gui::sound_effect::DeathCreepy6 });
            }

            case creature::race::Boar:
            {
                return Playlist(gui::sound_effect::DeathBoar);
            }

            case creature::race::Demon:
            {
                return Playlist(gui::sound_effect::DeathHowlDemon);
            }

            case creature::race::Griffin:
            {
                return Playlist(gui::sound_effect::DeathGriffin);
            }

            case creature::race::Lion:
            {
                return Playlist(gui::sound_effect::DeathLion);
            }

            case creature::race::Wereboar:
            case creature::race::LionBoar:
            {
                return Playlist(gui::sound_effect::DeathLionboar);
            }

            case creature::race::Naga:
            {
                return Playlist(gui::sound_effect::DeathBeastTalker2Roar);
            }

            case creature::race::Ramonaut:
            {
                return Playlist(gui::sound_effect::DeathBeast3);
            }

            case creature::race::Serpent:
            case creature::race::Cobra:
            {
                return Playlist({ gui::sound_effect::DeathSnake1,
                                  gui::sound_effect::DeathSnake2,
                                  gui::sound_effect::DeathSnake3,
                                  gui::sound_effect::DeathSnake4,
                                  gui::sound_effect::DeathSnake5,
                                  gui::sound_effect::DeathSnake6 });
            }

            case creature::race::Werebear:
            {
                return Playlist(gui::sound_effect::DeathBeast4);
            }

            case creature::race::Werecat:
            {
                return Playlist(gui::sound_effect::DeathThing3);
            }

            case creature::race::Bat:
            case creature::race::Werebat:
            {
                return Playlist(gui::sound_effect::DeathThing2);
            }

            case creature::race::Giant:
            {
                return Playlist(
                    { gui::sound_effect::DeathBeastHuge1a, gui::sound_effect::DeathBeastHuge1b });
            }

            case creature::race::Ghoul:
            {
                return Playlist(gui::sound_effect::DeathBeast6);
            }

            case creature::race::Count:
            default: return Playlist();
        }
    }

    const CombatSoundEffects::Playlist CombatSoundEffects::HitPlaylist(
        const item::WeaponTemplate & WEAPON_INFO, const item::Material::Enum WEAPON_MATERIAL)
    {
        if (WEAPON_INFO.IsBlowpipe())
        {
            return Playlist(gui::sound_effect_set::BlowpipeHit);
        }

        if (WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Sling))
        {
            // The horn hit sfx sounded good for slings too
            return Playlist(gui::sound_effect::MaterialHitHorn);
        }

        if (WEAPON_INFO.IsBow()
            || WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Crossbow))
        {
            return Playlist(gui::sound_effect_set::ArrowHit);
        }

        if (WEAPON_INFO.IsBodyPartBase())
        {
            switch (WEAPON_INFO.MinorAs<item::BodyPartWeapons>())
            {
                case item::BodyPartWeapons::Fists: return Playlist(gui::sound_effect_set::FistHit);

                case item::BodyPartWeapons::Tentacles:
                    return Playlist(gui::sound_effect_set::TentacleHit);

                case item::BodyPartWeapons::BreathOfFire:
                    return Playlist(gui::sound_effect::BreathHitFirebrand);

                case item::BodyPartWeapons::BreathOfFrost:
                    return Playlist(gui::sound_effect::BreathHitSylavin);

                    // TODO still need a bite sfx, use claws in the mean time
                case item::BodyPartWeapons::Bite:
                case item::BodyPartWeapons::Claws: return Playlist(gui::sound_effect_set::ClawHit);

                case item::BodyPartWeapons::Count:
                default: return Playlist();
            }
        }

        if (WEAPON_INFO.IsWhip())
        {
            return Playlist(gui::sound_effect_set::WhipHit);
        }

        return HitOfMaterialPlaylist(WEAPON_MATERIAL);
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::HitOfMaterialPlaylist(const item::Material::Enum MATERIAL)
    {
        switch (MATERIAL)
        {
            case item::Material::Wood: return Playlist(gui::sound_effect::MaterialHitWood);
            case item::Material::Leather: return Playlist(gui::sound_effect::MaterialHitLeather);
            case item::Material::Bone: return Playlist(gui::sound_effect::MaterialHitBone);

            case item::Material::Water:
            case item::Material::Acid:
            case item::Material::Blood: return Playlist(gui::sound_effect::MaterialHitLiquid);

            case item::Material::Plant: return Playlist(gui::sound_effect::MaterialHitPlant);
            case item::Material::Hide: return Playlist(gui::sound_effect::MaterialHitFlesh);
            case item::Material::Scales: return Playlist(gui::sound_effect::MaterialHitScale);
            case item::Material::DriedFlesh: return Playlist(gui::sound_effect::MaterialHitHide);

            case item::Material::Stone:
            case item::Material::Jade:
            case item::Material::Obsidian:
            case item::Material::Amethyst:
            case item::Material::Emerald:
            case item::Material::Pearl:
            case item::Material::Ruby:
            case item::Material::Lazuli:
            case item::Material::Sapphire:
            case item::Material::Diamond: return Playlist(gui::sound_effect::MaterialHitStone);

            case item::Material::Rope:
            case item::Material::Cloth:
            case item::Material::Silk: return Playlist(gui::sound_effect::MaterialHitCloth);

            case item::Material::Claw:
            case item::Material::Horn: return Playlist(gui::sound_effect::MaterialHitHorn);

            case item::Material::Tooth: return Playlist(gui::sound_effect::MaterialHitTooth);

            case item::Material::Tin:
            case item::Material::Gold:
            case item::Material::Iron:
            case item::Material::Bronze:
            case item::Material::Silver:
            case item::Material::Steel:
            case item::Material::Platinum: return Playlist(gui::sound_effect_set::MaterialHitMetal);

            case item::Material::Feather:
            case item::Material::Fur:
            case item::Material::Hair:
            case item::Material::Glass:
            case item::Material::Gas:
            case item::Material::Paper:
            case item::Material::Spirit:
            case item::Material::Dirt: return Playlist(gui::sound_effect::MaterialHitMisc);

            case item::Material::Count:
            default: return Playlist();
        }
    }

    const CombatSoundEffects::Playlist
        CombatSoundEffects::MissPlaylist(const item::WeaponTemplate & WEAPON_INFO)
    {
        if (WEAPON_INFO.IsProjectile())
        {
            return Playlist(gui::sound_effect_set::ProjectileMiss);
        }
        else if (WEAPON_INFO.IsWhip())
        {
            return Playlist(gui::sound_effect_set::WhipMiss);
        }
        else if (WEAPON_INFO.IsMelee())
        {
            return Playlist(gui::sound_effect_set::MeleeMiss);
        }
        else
        {
            return Playlist();
        }
    }

} // namespace combat
} // namespace heroespath
