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

#include "log/log-macros.hpp"
#include "game/loop-manager.hpp"

#include "stringutil/stringhelp.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <thread>


namespace heroespath
{
namespace sfml_util
{

    std::string SoundManager::soundsDirectoryPath_("");
    std::string SoundManager::musicDirectoryPath_("");
    std::unique_ptr<SoundManager> SoundManager::instanceUPtr_{ nullptr };


    SoundManager::SoundManager()
    :
        musicVolume_            (0.0f),
        effectsVolume_          (0.0f),
        sfxSetVec_              (),
        combatIntroMusicInfoVec_(),
        songSetVec_             (),
        sfxToPlayPairsVec_      (),
        sfxWrapperVec_          ()
    {
        M_HP_LOG_DBG("Singleton Construction: SoundManager");
        CacheMusicInfo_CombatIntro();
        sfxWrapperVec_.resize(static_cast<std::size_t>(sound_effect::Count));
        sfxSetVec_.resize(static_cast<std::size_t>(sound_effect_set::Count));
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
            instanceUPtr_ = std::make_unique<SoundManager>();
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
        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Prompt)] = SfxSet(SfxEnumVec_t
            { sound_effect::PromptGeneric,
              sound_effect::PromptQuestion,
              sound_effect::PromptWarn });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Switch)] = SfxSet(SfxEnumVec_t
            { sound_effect::Switch1,
              sound_effect::Switch2,
              sound_effect::Switch3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TickOn)] = SfxSet(SfxEnumVec_t
            { sound_effect::TickOn1,
              sound_effect::TickOn2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TickOff)] = SfxSet(SfxEnumVec_t
            { sound_effect::TickOff1,
              sound_effect::TickOff2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Thock)] = SfxSet(SfxEnumVec_t
            { sound_effect::Thock1,
              sound_effect::Thock2,
              sound_effect::Thock3,
              sound_effect::Thock4,
              sound_effect::Thock5,
              sound_effect::Thock6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Coin)] = SfxSet(SfxEnumVec_t
            { sound_effect::Coins1,
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
              sound_effect::Coins16 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Gem)] = SfxSet(SfxEnumVec_t
            { sound_effect::Gems });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MeteorShard)] = SfxSet(SfxEnumVec_t
            { sound_effect::MeteorShards });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ItemGive)] = SfxSet(SfxEnumVec_t
            { sound_effect::ItemGive });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ItemDrop)] = SfxSet(SfxEnumVec_t
            { sound_effect::ItemDrop });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::BlowpipeShoot)] = SfxSet(SfxEnumVec_t
            { sound_effect::BlowpipeShoot1,
              sound_effect::BlowpipeShoot2,
              sound_effect::BlowpipeShoot3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::BlowpipeHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::BlowpipeHit1,
              sound_effect::BlowpipeHit2,
              sound_effect::BlowpipeHit3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ArrowShoot)] = SfxSet(SfxEnumVec_t
            { sound_effect::ArrowShoot1,
              sound_effect::ArrowShoot2,
              sound_effect::ArrowShoot3,
              sound_effect::ArrowShoot4,
              sound_effect::ArrowShoot5,
              sound_effect::ArrowShoot6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ArrowHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::ArrowHit });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ProjectileMiss)] =
            SfxSet(SfxEnumVec_t
            { sound_effect::ProjectileMiss1,
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
              sound_effect::ProjectileMiss13 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MeleeMiss)] = SfxSet(SfxEnumVec_t
            { sound_effect::MeleeMiss1,
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
              sound_effect::MeleeMiss23 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::FistHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::FistHit1,
              sound_effect::FistHit2,
              sound_effect::FistHit3,
              sound_effect::FistHit4,
              sound_effect::FistHit5,
              sound_effect::FistHit6,
              sound_effect::FistHit7,
              sound_effect::FistHit8 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TendrilHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::TendrilHit1,
              sound_effect::TendrilHit2,
              sound_effect::TendrilHit3,
              sound_effect::TendrilHit4 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::WhipHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::WhipHit });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::WhipMiss)] = SfxSet(SfxEnumVec_t
            { sound_effect::WhipMiss1,
              sound_effect::WhipMiss2,
              sound_effect::WhipMiss3,
              sound_effect::WhipMiss4,
              sound_effect::WhipMiss5,
              sound_effect::WhipMiss6,
              sound_effect::WhipMiss7,
              sound_effect::WhipMiss8,
              sound_effect::WhipMiss9,
              sound_effect::WhipMiss10 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MaterialHitMetal)] =
            SfxSet(SfxEnumVec_t
            { sound_effect::MaterialHitMetal1,
              sound_effect::MaterialHitMetal2,
              sound_effect::MaterialHitMetal3,
              sound_effect::MaterialHitMetal4,
              sound_effect::MaterialHitMetal5 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MaterialHitMisc)] =
            SfxSet(SfxEnumVec_t
            { sound_effect::MaterialHitBone,
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
              sound_effect::MaterialHitMisc });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ClawHit)] = SfxSet(SfxEnumVec_t
            { sound_effect::ClawTear });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Wind)] = SfxSet(SfxEnumVec_t
            { sound_effect::WindGust1,
              sound_effect::WindGust2,
              sound_effect::WindGust3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::SpellSelect)] = SfxSet(SfxEnumVec_t
            { sound_effect::SpellSelect1,
              sound_effect::SpellSelect2,
              sound_effect::SpellSelect3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::SpellBandage)] = SfxSet(SfxEnumVec_t
            { sound_effect::SpellBandage1,
              sound_effect::SpellBandage2,
              sound_effect::SpellBandage3,
              sound_effect::SpellBandage4 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::DrumBlip)] = SfxSet(SfxEnumVec_t
            { sound_effect::DrumBlip1,
              sound_effect::DrumBlip2,
              sound_effect::DrumBlip3,
              sound_effect::DrumBlip4,
              sound_effect::DrumBlip5,
              sound_effect::DrumBlip6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::GuitarStrum)] = SfxSet(SfxEnumVec_t
            { sound_effect::GuitarStrum1,
              sound_effect::GuitarStrum2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::CombatWin)] = SfxSet(SfxEnumVec_t
            { sound_effect::CombatWin1,
              sound_effect::CombatWin2,
              sound_effect::CombatWin3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::CombatLose)] = SfxSet(SfxEnumVec_t
            { sound_effect::CombatLose1,
              sound_effect::CombatLose2,
              sound_effect::CombatLose3 });
    }


    void SoundManager::MusicStart(const music::Enum WHICH,
                                  const float       FADE_MULT,
                                  const float       VOLUME)
    {
        MusicStart(MusicEnumVec_t(1, WHICH),
                   false,
                   false,
                   FADE_MULT,
                   VOLUME,
                   music::IsLooped(WHICH));
    }


    void SoundManager::MusicStart(const MusicEnumVec_t & WHICH_VEC,
                                  const bool             WILL_RANDOMIZE,
                                  const bool             WILL_START_AT_RANDOM,
                                  const float            FADE_MULT,
                                  const float            VOLUME,
                                  const bool             WILL_LOOP)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((WHICH_VEC.empty() == false),
            "SoundManager::MusicStart(vector version) given an empty vector.");

        //use the global volume if an invalid volume is specified
        const float VOLUME_TO_USE((VOLUME < 0.0f) ? musicVolume_ : VOLUME);

        //Create the MusicSet object here, before we know if it is needed, because its
        //constructor is where the CurrentlyPlaying() song must be selected from among WHICH_VEC.
        MusicSet musicSet(WHICH_VEC,
                          WILL_RANDOMIZE,
                          WILL_START_AT_RANDOM,
                          FADE_MULT,
                          VOLUME_TO_USE,
                          WILL_LOOP);

        const music::Enum WHICH_MUSIC_TO_START_PLAYING(musicSet.CurrentlyPlaying());

        //if there is only one song in the vector, and it is the music that should start,
        //and it is already playing, then simply set the volume to whatever is specified
        if (WHICH_VEC.size() == 1)
        {
            for (auto & songSet : songSetVec_)
            {
                if (songSet.IsValid() &&
                    (songSet.set.CurrentlyPlaying() == WHICH_MUSIC_TO_START_PLAYING))
                {
                    songSet.op.VolumeFadeTo(VOLUME_TO_USE, FADE_MULT);
                    songSet.op.WillKillAfterFadeOut(false);
                    return;
                }
            }
        }

        songSetVec_.push_back( SongSet(
            musicSet,
            MakeAndStartMusicOperator(WHICH_MUSIC_TO_START_PLAYING, FADE_MULT, VOLUME_TO_USE)) );
    }


    void SoundManager::MusicStop(const music::Enum WHICH, const float FADE_MULT)
    {
        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid() &&
                (((songSet.set.Count() == 1) && (songSet.op.Info().Which() == WHICH)) ||
                (WHICH == music::All)))
            {
                songSet.op.VolumeFadeOut(FADE_MULT, true);
            }
        }
    }


    void SoundManager::MusicStop(const MusicEnumVec_t & MUSIC_ENUMS, const float FADE_MULT)
    {
        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                auto const CONTAINS_ALL{
                    [&]()
                {
                    for (auto const MUSIC : MUSIC_ENUMS)
                    {
                        if (songSet.set.Contains(MUSIC) == false)
                        {
                            return false;
                        }
                    }

                    return true;
                }() };

                if (CONTAINS_ALL)
                {
                    songSet.op.VolumeFadeOut(FADE_MULT, true);
                }
            }
        }
    }


    const MusicInfoVec_t SoundManager::MusicInfoSet() const
    {
        MusicInfoVec_t musicInfos;

        for (auto const & SONG_SET : songSetVec_)
        {
            if (SONG_SET.IsValid())
            {
                musicInfos.push_back(SONG_SET.op.Info());
            }
        }

        return musicInfos;
    }


    void SoundManager::MusicVolume(
        const MusicEnumVec_t & MUSIC_ENUMS,
        const float NEW_VOLUME,
        const float FADE_MULT)
    {
        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                auto const CONTAINS_ALL{
                    [&]()
                    {
                        for (auto const MUSIC : MUSIC_ENUMS)
                        {
                            if (songSet.set.Contains(MUSIC) == false)
                            {
                                return false;
                            }
                        }

                        return true;
                    }() };

                if (CONTAINS_ALL)
                {
                    songSet.op.VolumeFadeTo(NEW_VOLUME, FADE_MULT);
                }
            }
        }
    }


    void SoundManager::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        auto willCleanup{ false };

        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                auto const UPDATE_STATUS{ songSet.op.UpdateTime(ELAPSED_TIME_SECONDS) };

                if (UPDATE_STATUS == music_update_status::FadedOutKill)
                {
                    songSet.op.Stop();
                    songSet = SongSet();
                    willCleanup = true;
                }
                else if (UPDATE_STATUS == music_update_status::Stopped)
                {
                    songSet.op.Stop();

                    if (songSet.set.WillLoop())
                    {
                        const float VOLUME_TO_USE((songSet.set.Volume() < 0.0f) ?
                            musicVolume_ : songSet.set.Volume());

                        songSet.op = MakeAndStartMusicOperator(
                            songSet.set.Advance(), songSet.set.FadeInMult(), VOLUME_TO_USE);
                    }
                    else
                    {
                        songSet = SongSet();
                        willCleanup = true;
                    }
                }
            }
        }

        if (willCleanup)
        {
            songSetVec_.erase(std::remove_if(
                songSetVec_.begin(),
                songSetVec_.end(),
                [](const auto & SONG_SET)
                {
                    return (SONG_SET.IsValid() == false);
                }), songSetVec_.end());
        }

        SoundEffectsUpdate(ELAPSED_TIME_SECONDS);
    }


    void SoundManager::MusicVolumeSet(const float NEW_VOLUME)
    {
        musicVolume_ = NEW_VOLUME;

        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                songSet.op.Volume(NEW_VOLUME);
            }
        }
    }


    void SoundManager::SoundEffectVolumeSet(const float V)
    {
        effectsVolume_ = V;

        for (auto & sfxWrapper : sfxWrapperVec_)
        {
            //the Volume() function pre-checks if IsValid(), so this is safe
            sfxWrapper.Volume(effectsVolume_);
        }
    }


    void SoundManager::MusicVolumeFadeToCurrent(const music::Enum MUSIC_ENUM)
    {
        for (auto & songSet : songSetVec_)
        {
            if ((songSet.IsValid()) &&
                (songSet.op.Info().Which() == MUSIC_ENUM))
            {
                const float CURRENT_VOLUME(songSet.op.Volume());
                const float INTENDED_VOLUME(sfml_util::SoundManager::Instance()->MusicVolume());

                if (misc::IsRealClose(CURRENT_VOLUME, INTENDED_VOLUME) == false)
                {
                    if (misc::IsRealClose(INTENDED_VOLUME, 0.0f))
                    {
                        songSet.op.VolumeFadeOut();
                    }
                    else
                    {
                        songSet.op.VolumeFadeTo(
                            INTENDED_VOLUME,
                            ((CURRENT_VOLUME < INTENDED_VOLUME) ?
                                sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_ :
                                sfml_util::MusicOperator::FADE_MULT_DEFAULT_OUT_));
                    }
                }
            }
        }
    }

    const SfxSet & SoundManager::Getsound_effect_set(const sound_effect_set::Enum E) const
    {
        auto const INDEX{ static_cast<std::size_t>(E) };

        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < sfxSetVec_.size()),
            "SoundManager::Getsound_effect_set(enum=" << E << ", index=" << INDEX
                << ") index was out of range with sfxSetVec_.size()=" << sfxSetVec_.size());

        return sfxSetVec_[INDEX];
    }


    void SoundManager::SoundEffectPlay(const sound_effect::Enum E,
                                       const float PRE_DELAY_SEC)
    {
        if ((E != sound_effect::None) &&
            (E != sound_effect::Count &&
            (E != sound_effect::Random)))
        {
            sfxToPlayPairsVec_.push_back(std::make_pair(E, PRE_DELAY_SEC));
        }
    }


    void SoundManager::SoundEffectPlayNow(const sound_effect::Enum SFX_ENUM)
    {
        auto & sfxWrapper{ sfxWrapperVec_[SFX_ENUM] };

        if (sfxWrapper.IsValid() == false)
        {
            sfxWrapper = SfxWrapper(
                SFX_ENUM,
                std::make_unique<sf::Sound>(),
                LoadSfxBuffer(SFX_ENUM));

            sfxWrapper.Volume(SoundEffectVolume());
        }

        //This restarts play from the beginning, which is the
        //desired behavior even if it was already playing,
        //because I decided that repeated plays of the same
        //sfx will restart if not allowed to finish.
        sfxWrapper.Play();
    }


    void SoundManager::PlaySfx_AckMinor()
    {
        Getsound_effect_set(sfml_util::sound_effect_set::Switch).PlayRandom();
    }


    void SoundManager::PlaySfx_AckMajor()
    {
        Getsound_effect_set(sfml_util::sound_effect_set::Thock).PlayRandom();
    }


    void SoundManager::PlaySfx_Reject()
    {
        SoundEffectPlay(sfml_util::sound_effect::PromptWarn);
    }


    void SoundManager::PlaySfx_TickOn()
    {
        Getsound_effect_set(sfml_util::sound_effect_set::TickOn).PlayRandom();
    }


    void SoundManager::PlaySfx_TickOff()
    {
        Getsound_effect_set(sfml_util::sound_effect_set::TickOff).PlayRandom();
    }


    void SoundManager::PlaySfx_Keypress()
    {
        PlaySfx_AckMinor();
    }


    void SoundManager::PlaySfx_MouseClick()
    {
        PlaySfx_AckMinor();
    }


    void SoundManager::ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX)
    {
        //eliminate buffers for sound effects that are done playing
        for (auto & sfxWrapper : sfxWrapperVec_)
        {
            if (sfxWrapper.IsValid())
            {
                if (WILL_STOP_PLAYING_SFX)
                {
                    sfxWrapper.Stop();
                }

                if (sfxWrapper.Status() != sf::SoundSource::Playing)
                {
                    sfxWrapper.Reset();
                }
            }
        }

        if (WILL_STOP_PLAYING_SFX)
        {
            sfxToPlayPairsVec_.clear();
        }
    }


    MusicUPtr_t SoundManager::OpenMusic(const std::string & MUSIC_FILE_NAME,
                                        const std::string & MUSIC_DIR_NAME) const
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(musicDirectoryPath_) /
                                 bfs::path(MUSIC_DIR_NAME) /
                                 bfs::path(MUSIC_FILE_NAME)));

        return sfml_util::LoadMusic(PATH_OBJ.string());
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


    MusicOperator SoundManager::MakeAndStartMusicOperator(
        const music::Enum MUSIC_ENUM, const float FADE_MULT, const float VOLUME) const
    {
        MusicUPtr_t musicUPtr;
        MusicInfo musicInfo(MUSIC_ENUM);

        if (musicInfo.Which() == music::CombatIntro)
        {
            auto const INDEX{ static_cast<std::size_t>(
                misc::random::Int(0, static_cast<int>(combatIntroMusicInfoVec_.size()) - 1)) };

            musicInfo = combatIntroMusicInfoVec_[INDEX];

            musicUPtr = OpenMusic(musicInfo.Filename(), music::Directory(music::CombatIntro));
        }
        else
        {
            musicInfo.AutoSetup();
            musicUPtr = OpenMusic(musicInfo.Filename(), musicInfo.DirName());
        }

        MusicOperator musicOperator(musicInfo, std::move(musicUPtr), FADE_MULT, VOLUME);

        musicOperator.Volume(0.0f);
        musicOperator.VolumeFadeTo(VOLUME, FADE_MULT);
        musicOperator.Play();

        return musicOperator;
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
        static auto const MUSIC_COUNT_MAX{ 200 };

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
                ClearSoundEffectsCache();
            }
        }

        //test sound effects through SfxSet interface
        {
            static auto hasStaticSFXPromptedStart{ false };
            if (false == hasStaticSFXPromptedStart)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SfxSet SFX Tests starting...");

                hasStaticSFXPromptedStart = true;
            }

            static std::size_t sfxSetIndex{ 0 };
            if (sfxSetIndex < sfxSetVec_.size())
            {
                static std::size_t sfxSetInnerIndex{ 0 };

                M_ASSERT_OR_LOGANDTHROW_SS((sfxSetVec_.at(sfxSetIndex).IsValid()),
                    "sfml_util::SoundManager::Test() While testing SoudEffectsSets #"
                    << sfxSetIndex << ", enum="
                    << sound_effect::ToString(static_cast<sound_effect::Enum>(sfxSetIndex))
                    << " found IsValid()==false.");

                if (false == TestSfxSet(
                    sfxSetVec_.at(sfxSetIndex), sfxSetInnerIndex))
                {
                    ++sfxSetInnerIndex;
                    return false;
                }
                else
                {
                    std::ostringstream ss;
                    ss << sfxSetIndex;
                    game::LoopManager::Instance()->TestingStrIncrement(
                        "SoundManager SfxSet SFX Tested Set #" + ss.str());

                    sfxSetInnerIndex = 0;
                    ++sfxSetIndex;
                }

                return false;
            }

            static auto hasStaticSFXPromptedEnd{ false };
            if (false == hasStaticSFXPromptedEnd)
            {
                game::LoopManager::Instance()->TestingStrIncrement(
                    "SoundManager SfxSet SFX Tests finished.  All Passed.");

                hasStaticSFXPromptedEnd = true;
                ClearSoundEffectsCache();
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
                    playOrStop = false;
                    ++musicIndex;

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


    bool SoundManager::TestSfxSet(SfxSet & soundEffectsSet,
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
        auto willCleanup{ false };

        for (auto & sfxDelayPair : sfxToPlayPairsVec_)
        {
            sfxDelayPair.second -= ELAPSED_TIME_SEC;

            if (IsSfxDelayPairReadyToPlay(sfxDelayPair))
            {
                willCleanup = true;
                SoundEffectPlayNow(sfxDelayPair.first);
            }
        }

        if (willCleanup)
        {
            sfxToPlayPairsVec_.erase(
                std::remove_if(
                    sfxToPlayPairsVec_.begin(),
                    sfxToPlayPairsVec_.end(),
                    [this](const auto & SFX_DELAY_PAIR)
                    {
                        return this->IsSfxDelayPairReadyToPlay(SFX_DELAY_PAIR);
                    }),
                sfxToPlayPairsVec_.end() );
        }
    }


    SoundBufferUPtr_t SoundManager::LoadSfxBuffer(const sound_effect::Enum ENUM) const
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

        auto bufferUPtr{ std::make_unique<sf::SoundBuffer>() };

        M_ASSERT_OR_LOGANDTHROW_SS(bufferUPtr->loadFromFile(PATH_OBJ.string().c_str()),
            "sfml_util::SoundManager::LoadSound("
            << sound_effect::ToString(ENUM)
            << "), attempting path=\"" << PATH_OBJ.string()
            << "\", sf::SoundBuffer::loadFromFile() returned false.  See console output"
            << " for more information.");

        return bufferUPtr;
    }

}
}
