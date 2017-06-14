// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// sound-manager.cpp
//
#include "sound-manager.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/music-info.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"

#include "stringutil/stringhelp.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <thread>


namespace sfml_util
{

    std::string SoundManager::soundsDirectoryPath_("");
    std::string SoundManager::musicDirectoryPath_("");
    std::unique_ptr<SoundManager> SoundManager::instanceUPtr_{ nullptr };


    SoundManager::SoundManager()
    :
        musicVolume_            (0.0f),
        effectsVolume_          (0.0f),
        soundEffectsSetVec_     (),
        combatIntroMusicInfoVec_(),
        songsVec_               (),
        soundEffectsToPlayVec_  (),
        soundEffectsVec_        ()
    {
        M_HP_LOG_DBG("Singleton Construction: SoundManager");
        CacheMusicInfo_CombatIntro();
        soundEffectsVec_.resize(static_cast<std::size_t>(sound_effect::Count));
        soundEffectsSetVec_.resize(static_cast<std::size_t>(SfxSet::Count));
    }


    SoundManager::~SoundManager()
    {
        M_HP_LOG_DBG("Singleton Destruction: SoundManager");
    }


    SoundManager * SoundManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): SoundManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void SoundManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new SoundManager);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: SoundManager");
        }
    }


    void SoundManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "sfml_util::SoundManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    void SoundManager::SetSoundsDirectory(const std::string & SOUND_DIR_PATH,
                                          const std::string & MUSIC_DIR_PATH)
    {
        soundsDirectoryPath_ = SOUND_DIR_PATH;
        musicDirectoryPath_ = MUSIC_DIR_PATH;
    }


    void SoundManager::LoadSoundSets()
    {
        const SoundEffectEnumVec_t PROMPT_SOUNDS_ENUM_VEC = {
            sound_effect::PromptGeneric,
            sound_effect::PromptQuestion,
            sound_effect::PromptWarn };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Prompt)] =
            SoundEffectsSet(PROMPT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t SWITCH_SOUNDS_ENUM_VEC = {
            sound_effect::Switch1,
            sound_effect::Switch2,
            sound_effect::Switch3 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Switch)] =
            SoundEffectsSet(SWITCH_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t TICKON_SOUNDS_ENUM_VEC = {
            sound_effect::TickOn1,
            sound_effect::TickOn2 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TickOn)] =
            SoundEffectsSet(TICKON_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t TICKOFF_SOUNDS_ENUM_VEC = {
            sound_effect::TickOff1,
            sound_effect::TickOff2 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TickOff)] =
            SoundEffectsSet(TICKOFF_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t THOCK_SOUNDS_ENUM_VEC = {
            sound_effect::Thock1,
            sound_effect::Thock2,
            sound_effect::Thock3,
            sound_effect::Thock4,
            sound_effect::Thock5,
            sound_effect::Thock6 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Thock)] =
            SoundEffectsSet(THOCK_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t COIN_SOUNDS_ENUM_VEC = {
            sound_effect::Coins1,
            sound_effect::Coins2,
            sound_effect::Coins3,
            sound_effect::Coins4,
            sound_effect::Coins5,
            sound_effect::Coins6,
            sound_effect::Coins7,
            sound_effect::Coins8,
            sound_effect::Coins9,
            sound_effect::Coins10,
            sound_effect::Coins11,
            sound_effect::Coins12,
            sound_effect::Coins13,
            sound_effect::Coins14,
            sound_effect::Coins15,
            sound_effect::Coins16 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Coin)] =
            SoundEffectsSet(COIN_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t GEM_SOUNDS_ENUM_VEC = { sound_effect::Gems };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Gem)] =
            SoundEffectsSet(GEM_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t METEORSHARD_SOUNDS_ENUM_VEC = { sound_effect::MeteorShards };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MeteorShard)] =
            SoundEffectsSet(METEORSHARD_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t ITEMGIVE_SOUNDS_ENUM_VEC = { sound_effect::ItemGive };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ItemGive)] =
            SoundEffectsSet(ITEMGIVE_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t ITEMDROP_SOUNDS_ENUM_VEC = { sound_effect::ItemDrop };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ItemDrop)] =
            SoundEffectsSet(ITEMDROP_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t BLOWPIPE_SHOOT_SOUNDS_ENUM_VEC = {
            sound_effect::BlowpipeShoot1,
            sound_effect::BlowpipeShoot2,
            sound_effect::BlowpipeShoot3 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BlowpipeShoot)] =
            SoundEffectsSet(BLOWPIPE_SHOOT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t BLOWPIPE_HIT_SOUNDS_ENUM_VEC = {
            sound_effect::BlowpipeHit1,
            sound_effect::BlowpipeHit2,
            sound_effect::BlowpipeHit3 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BlowpipeHit)] =
            SoundEffectsSet(BLOWPIPE_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t ARROW_SHOOT_SOUNDS_ENUM_VEC = {
            sound_effect::ArrowShoot1,
            sound_effect::ArrowShoot2,
            sound_effect::ArrowShoot3,
            sound_effect::ArrowShoot4,
            sound_effect::ArrowShoot5,
            sound_effect::ArrowShoot6 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ArrowShoot)] =
            SoundEffectsSet(ARROW_SHOOT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t ARROW_HIT_SOUNDS_ENUM_VEC = { sound_effect::ArrowHit };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ArrowHit)] =
            SoundEffectsSet(ARROW_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t PROJECTILE_MISS_SOUNDS_ENUM_VEC = {
            sound_effect::ProjectileMiss1,
            sound_effect::ProjectileMiss2,
            sound_effect::ProjectileMiss3,
            sound_effect::ProjectileMiss4,
            sound_effect::ProjectileMiss5,
            sound_effect::ProjectileMiss6,
            sound_effect::ProjectileMiss7,
            sound_effect::ProjectileMiss8,
            sound_effect::ProjectileMiss9,
            sound_effect::ProjectileMiss10,
            sound_effect::ProjectileMiss11,
            sound_effect::ProjectileMiss12,
            sound_effect::ProjectileMiss13 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ProjectileMiss)] =
            SoundEffectsSet(PROJECTILE_MISS_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t MELEE_MISS_SOUNDS_ENUM_VEC = {
            sound_effect::MeleeMiss1,
            sound_effect::MeleeMiss2,
            sound_effect::MeleeMiss3,
            sound_effect::MeleeMiss4,
            sound_effect::MeleeMiss5,
            sound_effect::MeleeMiss6,
            sound_effect::MeleeMiss7,
            sound_effect::MeleeMiss8,
            sound_effect::MeleeMiss9,
            sound_effect::MeleeMiss10,
            sound_effect::MeleeMiss11,
            sound_effect::MeleeMiss12,
            sound_effect::MeleeMiss13,
            sound_effect::MeleeMiss14,
            sound_effect::MeleeMiss15,
            sound_effect::MeleeMiss16,
            sound_effect::MeleeMiss17,
            sound_effect::MeleeMiss18,
            sound_effect::MeleeMiss19,
            sound_effect::MeleeMiss20,
            sound_effect::MeleeMiss21,
            sound_effect::MeleeMiss22,
            sound_effect::MeleeMiss23 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MeleeMiss)] =
            SoundEffectsSet(MELEE_MISS_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t FIST_HIT_SOUNDS_ENUM_VEC = {
            sound_effect::FistHit1,
            sound_effect::FistHit2,
            sound_effect::FistHit3,
            sound_effect::FistHit4,
            sound_effect::FistHit5,
            sound_effect::FistHit6,
            sound_effect::FistHit7,
            sound_effect::FistHit8 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::FistHit)] =
            SoundEffectsSet(FIST_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t BREATH_HIT_SOUNDS_ENUM_VEC = {
            sound_effect::BreathHit1,
            sound_effect::BreathHit2 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BreathHit)] =
            SoundEffectsSet(BREATH_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t TENDRILS_HIT_SOUNDS_ENUM_VEC = {
            sound_effect::TendrilHit1,
            sound_effect::TendrilHit2,
            sound_effect::TendrilHit3,
            sound_effect::TendrilHit4 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TendrilHit)] =
            SoundEffectsSet(TENDRILS_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t WHIP_HIT_SOUNDS_ENUM_VEC = { sound_effect::WhipHit };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::WhipHit)] =
            SoundEffectsSet(WHIP_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t WHIP_MISS_SOUNDS_ENUM_VEC = {
            sound_effect::WhipMiss1,
            sound_effect::WhipMiss2,
            sound_effect::WhipMiss3,
            sound_effect::WhipMiss4,
            sound_effect::WhipMiss5,
            sound_effect::WhipMiss6,
            sound_effect::WhipMiss7,
            sound_effect::WhipMiss8,
            sound_effect::WhipMiss9,
            sound_effect::WhipMiss10 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::WhipMiss)] =
            SoundEffectsSet(WHIP_MISS_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t MATERIAL_HIT_METAL_SOUNDS_ENUM_VEC = {
            sound_effect::MaterialHitMetal1,
            sound_effect::MaterialHitMetal2,
            sound_effect::MaterialHitMetal3,
            sound_effect::MaterialHitMetal4,
            sound_effect::MaterialHitMetal5 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MaterialHitMetal)] =
            SoundEffectsSet(MATERIAL_HIT_METAL_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t MATERIAL_HIT_MISC_SOUNDS_ENUM_VEC = {
            sound_effect::MaterialHitBone,
            sound_effect::MaterialHitCloth,
            sound_effect::MaterialHitFlesh,
            sound_effect::MaterialHitHide,
            sound_effect::MaterialHitHorn,
            sound_effect::MaterialHitLeather,
            sound_effect::MaterialHitLiquid,
            sound_effect::MaterialHitPlant,
            sound_effect::MaterialHitScale,
            sound_effect::MaterialHitStone,
            sound_effect::MaterialHitTooth,
            sound_effect::MaterialHitWood,
            sound_effect::MaterialHitMisc};

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MaterialHitMisc)] =
            SoundEffectsSet(MATERIAL_HIT_MISC_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t CLAW_HIT_SOUNDS_ENUM_VEC = { sound_effect::ClawTear };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ClawHit)] =
            SoundEffectsSet(CLAW_HIT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t WIND_SOUNDS_ENUM_VEC = {
            sound_effect::WindGust1,
            sound_effect::WindGust2,
            sound_effect::WindGust3 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Wind)] =
            SoundEffectsSet(WIND_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t SPELLSELECT_SOUNDS_ENUM_VEC = {
            sound_effect::SpellSelect1,
            sound_effect::SpellSelect2,
            sound_effect::SpellSelect3 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::SpellSelect)] =
            SoundEffectsSet(SPELLSELECT_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t SPELLBANDAGE_SOUNDS_ENUM_VEC = {
            sound_effect::SpellBandage1,
            sound_effect::SpellBandage2,
            sound_effect::SpellBandage3,
            sound_effect::SpellBandage4 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::SpellBandage)] =
            SoundEffectsSet(SPELLBANDAGE_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t DRUMBLIP_SOUNDS_ENUM_VEC = {
            sound_effect::DrumBlip1,
            sound_effect::DrumBlip2,
            sound_effect::DrumBlip3,
            sound_effect::DrumBlip4,
            sound_effect::DrumBlip5,
            sound_effect::DrumBlip6 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::DrumBlip)] =
            SoundEffectsSet(DRUMBLIP_SOUNDS_ENUM_VEC);

        const SoundEffectEnumVec_t GUITARSTRUM_SOUNDS_ENUM_VEC = {
            sound_effect::GuitarStrum1,
            sound_effect::GuitarStrum2 };

        soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::GuitarStrum)] =
            SoundEffectsSet(GUITARSTRUM_SOUNDS_ENUM_VEC);
    }


    const MusicSetSPtr_t SoundManager::MusicStart(const music::Enum WHICH,
                                                  const float       FADE_MULT,
                                                  const float       VOLUME)
    {
        return MusicStart(MusicEnumVec_t(1, WHICH),
                          false,
                          false,
                          FADE_MULT,
                          VOLUME,
                          music::IsLooped(WHICH));
    }


    const MusicSetSPtr_t SoundManager::MusicStart(const MusicEnumVec_t & WHICH_VEC,
                                                  const bool             WILL_RANDOMIZE,
                                                  const bool             WILL_START_AT_RANDOM,
                                                  const float            FADE_MULT,
                                                  const float            VOLUME,
                                                  const bool             WILL_LOOP)
    {
        //use the global volume if the invalid volume is specified
        const float VOLUME_TO_USE((VOLUME < 0.0f) ? musicVolume_ : VOLUME);

        MusicSetSPtr_t musicSetSPtr(new MusicSet(WHICH_VEC,
                                    WILL_RANDOMIZE,
                                    WILL_START_AT_RANDOM,
                                    FADE_MULT,
                                    VOLUME_TO_USE,
                                    WILL_LOOP));

        //After construction, the musicSetSPtr currently playing member is
        //already set to what should play first.
        const music::Enum WHICH_MUSIC_TO_START_PLAYING(musicSetSPtr->CurrentlyPlaying());

        //if there is only one song in the list, and it is already playing,
        //then simply set the volume to whatever is specified
        if (WHICH_VEC.size() == 1)
        {
            for (auto & nextSongsObj : songsVec_)
            {
                if ((nextSongsObj.set_sptr.get() != nullptr) &&
                    (nextSongsObj.set_sptr->CurrentlyPlaying() == WHICH_MUSIC_TO_START_PLAYING))
                {
                    if (nextSongsObj.operator_sptr.get() != nullptr)
                    {
                        nextSongsObj.operator_sptr->VolumeFadeTo(VOLUME_TO_USE, FADE_MULT);
                        nextSongsObj.operator_sptr->KillAfterFadeOut(false);
                        return nextSongsObj.set_sptr;
                    }
                }
            }
        }

        MusicSPtr_t musicSPtr;
        const MusicInfo MUSIC_INFO(MusicInfoAcquire(WHICH_MUSIC_TO_START_PLAYING, musicSPtr));

        MusicOperatorSPtr_t musicOperatorSPtr;
        MusicOperatorStart(musicOperatorSPtr, MUSIC_INFO, musicSPtr, FADE_MULT, VOLUME_TO_USE);

        songsVec_.push_back(Songs(musicSetSPtr, musicOperatorSPtr));

        return musicSetSPtr;
    }


    void SoundManager::MusicStop(const music::Enum WHICH, const float FADE_MULT)
    {
        for (auto & nextSongsObj : songsVec_)
        {
            if ((nextSongsObj.operator_sptr.get() != nullptr) &&
                ((nextSongsObj.operator_sptr->Info().Which() == WHICH) || (WHICH == music::All)))
            {
                nextSongsObj.operator_sptr->VolumeFadeOut(FADE_MULT, true);
            }
        }
    }


    void SoundManager::MusicStop(const MusicSetSPtr_t & MUSIC_SET_SPTR, const float FADE_MULT)
    {
        for (auto & nextSongsObj : songsVec_)
        {
            if ((nextSongsObj.set_sptr == MUSIC_SET_SPTR) &&
                (nextSongsObj.operator_sptr.get() != nullptr))
            {
                nextSongsObj.operator_sptr->VolumeFadeOut(FADE_MULT, true);
            }
        }
    }


    MusicOperatorSPtr_t SoundManager::MusicOperator(const music::Enum WHICH)
    {
        for (auto const & NEXT_SONGS_OBJ : songsVec_)
        {
            if ((NEXT_SONGS_OBJ.operator_sptr.get() != nullptr) &&
                (NEXT_SONGS_OBJ.operator_sptr->Info().Which() == WHICH))
            {
                return NEXT_SONGS_OBJ.operator_sptr;
            }
        }

        return MusicOperatorSPtr_t();
    }


    MusicOperatorSLst_t SoundManager::MusicOperators()
    {
        MusicOperatorSLst_t musicOperatorsList;

        for (auto const & NEXT_SONGS_OBJ : songsVec_)
        {
            if (NEXT_SONGS_OBJ.operator_sptr.get() != nullptr)
            {
                musicOperatorsList.push_back(NEXT_SONGS_OBJ.operator_sptr);
            }
        }

        return musicOperatorsList;
    }


    void SoundManager::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        SongsVec_t songsToRemove;

        for (auto & nextSongsObj : songsVec_)
        {
            if (nextSongsObj.operator_sptr.get() != nullptr)
            {
                const music_update_status::Enum UPDATE_STATUS(
                    nextSongsObj.operator_sptr->UpdateTime(ELAPSED_TIME_SECONDS));

                if (UPDATE_STATUS == music_update_status::FadedOutKill)
                {
                    songsToRemove.push_back(nextSongsObj);
                }
                else if (UPDATE_STATUS == music_update_status::Stopped)
                {
                    if (nextSongsObj.set_sptr->WillLoop() == false)
                    {
                        songsToRemove.push_back(nextSongsObj);
                    }
                    else
                    {
                        const music::Enum WHICH_MUSIC_TO_PLAY_NEXT(
                            nextSongsObj.set_sptr->Advance());

                        MusicSPtr_t musicSPtr;
                        const MusicInfo MUSIC_INFO(MusicInfoAcquire(WHICH_MUSIC_TO_PLAY_NEXT,
                                                                    musicSPtr));

                        //use the global volume if the invalid volume is specified
                        const float VOLUME_TO_USE((nextSongsObj.set_sptr->Volume() < 0.0f) ?
                            musicVolume_ : nextSongsObj.set_sptr->Volume());

                        MusicOperatorSPtr_t musicOperatorSPtr;
                        MusicOperatorStart(musicOperatorSPtr,
                                           MUSIC_INFO,
                                           musicSPtr,
                                           nextSongsObj.set_sptr->FadeInMult(),
                                           VOLUME_TO_USE);

                        nextSongsObj.operator_sptr = musicOperatorSPtr;
                    }
                }
            }
        }

        for (auto const & NEXT_SONGS_OBJ_TO_REMOVE_SPTR : songsToRemove)
        {
            songsVec_.erase(std::remove(songsVec_.begin(),
                                        songsVec_.end(),
                                        NEXT_SONGS_OBJ_TO_REMOVE_SPTR),
                            songsVec_.end());
        }

        SoundEffectsUpdate(ELAPSED_TIME_SECONDS);
    }


    void SoundManager::MusicVolumeSet(const float NEW_VOLUME)
    {
        musicVolume_ = NEW_VOLUME;

        for (auto & nextSongsObj : songsVec_)
        {
            if (nextSongsObj.operator_sptr.get() != nullptr)
            {
                nextSongsObj.operator_sptr->Volume(NEW_VOLUME);
            }
        }
    }


    void SoundManager::SoundEffectVolumeSet(const float V)
    {
        effectsVolume_ = V;

        for (auto & nextSoundEffectData : soundEffectsVec_)
        {
            nextSoundEffectData.sound.setVolume(effectsVolume_);
        }
    }


    void SoundManager::SoundEffectPlay(const sound_effect::Enum E,
                                       const float              PRE_DELAY_SEC)
    {
        soundEffectsToPlayVec_.push_back( std::make_pair(E, PRE_DELAY_SEC) );
    }


    void SoundManager::OpenMusic(const std::string & MUSIC_FILE_NAME,
                                 const std::string & MUSIC_DIR_NAME,
                                 MusicSPtr_t &       musicSPtr) const
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(musicDirectoryPath_) /
            bfs::path(MUSIC_DIR_NAME) / bfs::path(MUSIC_FILE_NAME)));

        sfml_util::OpenMusicSPtr(musicSPtr, PATH_OBJ.string());
    }


    void SoundManager::CacheMusicInfo_CombatIntro()
    {
        //configure the path
        namespace bfs = boost::filesystem;
        const bfs::path DIR_OBJ(bfs::system_complete(bfs::path(musicDirectoryPath_) /
            bfs::path(music::Directory(music::CombatIntro))));

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(DIR_OBJ),
            "sfml_util::SoundManager::CacheMusicInfo_CombatIntro()  directory \""
            << DIR_OBJ.string() << "\" does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_directory(DIR_OBJ),
            "sfml_util::SoundManager::CacheMusicInfo_CombatIntro()  directory \""
            << DIR_OBJ.string() << "\" exists but is not a directory.");

        combatIntroMusicInfoVec_.clear();

        bfs::directory_iterator end_iter; // default construction yields past-the-end
        for (bfs::directory_iterator iter(DIR_OBJ); iter != end_iter; ++iter)
        {
            const bfs::path NEXT_PATH_OBJ(iter->path());

            if (bfs::is_regular_file(NEXT_PATH_OBJ) == false)
            {
                continue;
            }

            auto const NEXT_FILENAME(NEXT_PATH_OBJ.leaf().string());

            if ((boost::algorithm::icontains(NEXT_FILENAME, ".txt")) ||
                (boost::algorithm::icontains(NEXT_FILENAME, ".DS_Store")))
            {
                continue;
            }

            std::vector<std::string> filenamePartsVec;
            appbase::stringhelp::SplitByChar(NEXT_FILENAME, filenamePartsVec, '_', true, true);
            if (filenamePartsVec.size() != 4)
            {
                continue;
            }

            const std::string NEXT_ARTIST_NAME(filenamePartsVec.at(1));
            const std::string NEXT_TRACK_NAME(filenamePartsVec.at(2));

            const std::string NEXT_LICENSE_NAME(boost::algorithm::erase_all_copy(
                filenamePartsVec.at(3), music::FileExt()));

            combatIntroMusicInfoVec_.push_back(MusicInfo(music::CombatIntro,
                                                         false,
                                                         NEXT_ARTIST_NAME,
                                                         NEXT_TRACK_NAME,
                                                         NEXT_LICENSE_NAME,
                                                         NEXT_FILENAME,
                                                         DIR_OBJ.string()));
        }
    }


    void SoundManager::MusicAcquire_CombatIntro(MusicInfo & musicInfo, MusicSPtr_t & musicSPtr)
    {
        const std::size_t INDEX(static_cast<std::size_t>(misc::random::Int(0,
            static_cast<int>(combatIntroMusicInfoVec_.size()) - 1)));

        MusicAcquire_CombatIntro(musicInfo, musicSPtr, INDEX);
    }


    void SoundManager::MusicAcquire_CombatIntro(MusicInfo &       musicInfo,
                                                MusicSPtr_t &     musicSPtr,
                                                const std::size_t INDEX)
    {
        OpenMusic(combatIntroMusicInfoVec_[INDEX].Filename(),
                  music::Directory(music::CombatIntro), musicSPtr);

        musicInfo = combatIntroMusicInfoVec_[INDEX];
    }


    const MusicInfo SoundManager::MusicInfoAcquire(const music::Enum WHICH,
                                                   MusicSPtr_t &     musicSPtr)
    {
        MusicInfo musicInfo(WHICH);

        if (WHICH == music::CombatIntro)
        {
            MusicAcquire_CombatIntro(musicInfo, musicSPtr);
        }
        else
        {
            OpenMusic(musicInfo.Filename(), musicInfo.DirName(), musicSPtr);
        }

        return musicInfo;
    }


    void SoundManager::MusicOperatorStart(MusicOperatorSPtr_t & musicOperatorSPtr,
                                          const MusicInfo &     MUSIC_INFO,
                                          const MusicSPtr_t &   MUSIC_SPTR,
                                          const float           FADE_MULT,
                                          const float           VOLUME)
    {
        musicOperatorSPtr.reset(new sfml_util::MusicOperator(MUSIC_INFO,
                                                             MUSIC_SPTR,
                                                             FADE_MULT,
                                                             VOLUME));
        musicOperatorSPtr->Volume(0.0f);
        musicOperatorSPtr->VolumeFadeTo(VOLUME, FADE_MULT);
        musicOperatorSPtr->Play();
    }


    bool SoundManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::SoundManager::Test() Starting Tests...");
        }

        static auto counter{ 0 };
        static auto playOrStop{ false };
        static auto const MUSIC_COUNT_MAX{ 50 };

        //test sound effects individually
        {
            static auto hasSFXPromptedStart{ false };
            if (false == hasSFXPromptedStart)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SFX Tests starting...");
                hasSFXPromptedStart = true;
            }

            static auto sfxIndex{ 0 };
            if (sfxIndex < sound_effect::Count)
            {
                auto const ENUM{ static_cast<sound_effect::Enum>(sfxIndex) };
                auto const ENUM_STR{ sound_effect::ToString(ENUM) };
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SFX Test \"" + ENUM_STR + "\"");
                SoundEffectPlay(ENUM);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                ++sfxIndex;
                return false;
            }

            static auto hasSFXPromptedEnd{ false };
            if (false == hasSFXPromptedEnd)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SFX Tests finished.  All Passed.");
                hasSFXPromptedEnd = true;
            }
        }

        //test sound effects through SoundEffectsSet interface
        {
            static auto hasStaticSFXPromptedStart{ false };
            if (false == hasStaticSFXPromptedStart)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SoundEffectsSet SFX Tests starting...");
                hasStaticSFXPromptedStart = true;
            }

            static std::size_t sfxSetIndex{ 0 };
            if (sfxSetIndex < soundEffectsSetVec_.size())
            {
                static std::size_t sfxSetInnerIndex{ 0 };

                M_ASSERT_OR_LOGANDTHROW_SS((soundEffectsSetVec_.at(sfxSetIndex).IsValid()),
                    "sfml_util::SoundManager::Test() While testing SoudEffectsSets #"
                    << sfxSetIndex << ", enum="
                    << sound_effect::ToString(static_cast<sound_effect::Enum>(sfxSetIndex))
                    << " found IsValid()==false.");

                if (false == TestSoundEffectsSet(soundEffectsSetVec_.at(sfxSetIndex),
                                                 sfxSetInnerIndex))
                {
                    ++sfxSetInnerIndex;
                    return false;
                }
                else
                {
                    std::ostringstream ss;
                    ss << sfxSetIndex;
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager SoundEffectsSet SFX Tested Set #" + ss.str());

                    sfxSetInnerIndex = 0;
                    ++sfxSetIndex;
                }

                return false;
            }

            static auto hasStaticSFXPromptedEnd{ false };
            if (false == hasStaticSFXPromptedEnd)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SoundEffectsSet SFX Tests finished.  All Passed.");
                hasStaticSFXPromptedEnd = true;
            }
        }

        //test regular music
        static auto musicIndex{ 0 };
        {
            if (musicIndex < music::Count)
            {
                std::ostringstream ss;
                ss << musicIndex;

                auto NEXT_ENUM(static_cast<music::Enum>(musicIndex));

                if (false == playOrStop)
                {
                    MusicStart(NEXT_ENUM);
                    playOrStop = true;
                    counter = 0;
                    return false;
                }

                if (counter < MUSIC_COUNT_MAX)
                {
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager Music Test #" + ss.str() + " Delay...");
                    ++counter;
                    return false;
                }

                if (playOrStop)
                {
                    MusicStop(NEXT_ENUM);
                    playOrStop = !playOrStop;
                    ++musicIndex;
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager Music Test #" + ss.str());
                    return false;
                }
            }
        }

        //test combat music
        {
            static std::size_t combatMusicIndex{ 0 };

            if (combatMusicIndex < CombatMusicCount())
            {
                std::ostringstream ss;
                ss << combatMusicIndex;

                static MusicInfo musicInfo(music::CombatIntro);
                static MusicSPtr_t musicSPtr;

                if (musicSPtr.get() == nullptr)
                    MusicAcquire_CombatIntro(musicInfo, musicSPtr, combatMusicIndex);

                if (false == playOrStop)
                {
                    musicSPtr->setVolume(100.0f);
                    musicSPtr->play();
                    playOrStop = true;
                    counter = 0;
                    return false;
                }

                if (counter < MUSIC_COUNT_MAX)
                {
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager Combat Music Test #" + ss.str() + " Delay...");
                    ++counter;
                    return false;
                }

                if (playOrStop)
                {
                    musicSPtr->stop();
                    playOrStop = !playOrStop;
                    ++combatMusicIndex;
                    musicSPtr.reset();
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager Music Test #" + ss.str());
                    return false;
                }
            }
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "sfml_util::SoundManager::Test() ALL TESTS PASSED");

        return true;
    }


    bool SoundManager::TestSoundEffectsSet(SoundEffectsSet & soundEffectsSet,
                                           const std::size_t INDEX)
    {
        if (INDEX < soundEffectsSet.Size())
        {
            soundEffectsSet.PlayAt(INDEX);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return false;
        }
        else
        {
            return true;
        }
    }


    void SoundManager::SoundEffectsUpdate(const float ELAPSED_TIME_SEC)
    {
        for (auto & nextSfxDelayPair : soundEffectsToPlayVec_)
        {
            nextSfxDelayPair.second -= ELAPSED_TIME_SEC;

            if (nextSfxDelayPair.second < 0.0f)
            {
                SoundEffectData & sfxData{ soundEffectsVec_[nextSfxDelayPair.first] };
                if (sfxData.sound.getStatus() == sf::SoundSource::Playing)
                {
                    //This restarts play from the beginning
                    sfxData.sound.play();
                }
                else
                {
                    LoadSound(nextSfxDelayPair.first, sfxData);
                    sfxData.sound.setVolume(SoundEffectVolume());
                    sfxData.sound.play();
                }
            }
        }

        soundEffectsToPlayVec_.erase(
            std::remove_if( soundEffectsToPlayVec_.begin(), soundEffectsToPlayVec_.end(),
                []
                (const std::pair<sound_effect::Enum, float> & PAIR)
                { return (PAIR.second < 0.0f); }),
            soundEffectsToPlayVec_.end() );
    }


    void SoundManager::ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX)
    {
        //eliminate buffers for sound effects that are done playing
        for (auto & nextSoundEffectData : soundEffectsVec_)
        {
            if (WILL_STOP_PLAYING_SFX &&
                (nextSoundEffectData.sound.getStatus() == sf::SoundSource::Playing))
            {
                nextSoundEffectData.sound.stop();
            }

            if (nextSoundEffectData.sound.getStatus() == sf::SoundSource::Stopped)
            {
                nextSoundEffectData.buffer = sf::SoundBuffer();
                nextSoundEffectData.sound.setBuffer(nextSoundEffectData.buffer);
            }
        }

        if (WILL_STOP_PLAYING_SFX)
        {
            soundEffectsToPlayVec_.clear();
        }
    }


    void SoundManager::LoadSound(const sound_effect::Enum ENUM,
                                 SoundEffectData &        soundEffectData) const
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(soundsDirectoryPath_) /
            bfs::path(sound_effect::Directory(ENUM)) /
            bfs::path(sound_effect::Filename(ENUM))));

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(PATH_OBJ),
            "sfml_util::SoundManager::LoadSound("
            << sound_effect::ToString(ENUM)
            << "), attempting path=\"" << PATH_OBJ.string()
            << "\", failed because that file does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_regular_file(PATH_OBJ),
            "sfml_util::SoundManager::LoadSound("
            << sound_effect::ToString(ENUM)
            << "), attempting path=\"" << PATH_OBJ.string()
            << "\", failed because that is not a regular file.");

        M_ASSERT_OR_LOGANDTHROW_SS(soundEffectData.buffer.loadFromFile(PATH_OBJ.string().c_str()),
            "sfml_util::SoundManager::LoadSound("
            << sound_effect::ToString(ENUM)
            << "), attempting path=\"" << PATH_OBJ.string()
            << "\", sf::SoundBuffer::loadFromFile() returned false.  See console output"
            << " for more information.");

        soundEffectData.sound.setBuffer(soundEffectData.buffer);
    }

}
