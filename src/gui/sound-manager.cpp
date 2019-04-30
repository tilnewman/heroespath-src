// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sound-manager.cpp
//
#include "sound-manager.hpp"

#include "gui/music-info.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "stage/i-stage.hpp"

#include <chrono>
#include <thread>

namespace heroespath
{
namespace gui
{

    std::string SoundManager::soundsDirectoryPath_("");
    std::string SoundManager::musicDirectoryPath_("");
    std::unique_ptr<SoundManager> SoundManager::instanceUPtr_;

    SoundManager::SoundManager()
        : musicVolume_(0.0f)
        , effectsVolume_(0.0f)
        , sfxSetVec_()
        , combatIntroMusicInfoVec_()
        , songSetVec_()
        , delayedSfxVec_()
        , sfxWrapperVec_()
    {
        M_HP_LOG_DBG("Subsystem Construction: SoundManager");
        sfxWrapperVec_.resize(static_cast<std::size_t>(sound_effect::Count));
        sfxSetVec_.resize(static_cast<std::size_t>(sound_effect_set::Count));
    }

    SoundManager::~SoundManager() { M_HP_LOG_DBG("Subsystem Destruction: SoundManager"); }

    misc::NotNull<SoundManager *> SoundManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: SoundManager");
            Acquire();
        }

        return instanceUPtr_;
    }

    void SoundManager::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<SoundManager>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: SoundManager");
        }
    }

    void SoundManager::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "gui::SoundManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    void SoundManager::Initialize()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "gui::SoundManager::Initialize() found instanceUPtr that was null.");

        instanceUPtr_->CacheMusicInfo_CombatIntro();
        instanceUPtr_->LoadSoundSets();
    }

    void SoundManager::SetSoundsDirectory(
        const std::string & SOUND_DIR_PATH, const std::string & MUSIC_DIR_PATH)
    {
        soundsDirectoryPath_ = SOUND_DIR_PATH;
        musicDirectoryPath_ = MUSIC_DIR_PATH;
    }

    void SoundManager::LoadSoundSets()
    {
        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Prompt)] = SfxSet(SfxEnumVec_t {
            sound_effect::PromptGeneric, sound_effect::PromptQuestion, sound_effect::PromptWarn });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Switch)] = SfxSet(
            SfxEnumVec_t { sound_effect::Switch1, sound_effect::Switch2, sound_effect::Switch3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TickOn)]
            = SfxSet(SfxEnumVec_t { sound_effect::TickOn1, sound_effect::TickOn2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TickOff)]
            = SfxSet(SfxEnumVec_t { sound_effect::TickOff1, sound_effect::TickOff2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Thock)]
            = SfxSet(SfxEnumVec_t { sound_effect::Thock1,
                                    sound_effect::Thock2,
                                    sound_effect::Thock3,
                                    sound_effect::Thock4,
                                    sound_effect::Thock5,
                                    sound_effect::Thock6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Coin)]
            = SfxSet(SfxEnumVec_t { sound_effect::Coins1,
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

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Gem)]
            = SfxSet(SfxEnumVec_t { sound_effect::Gems });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Shard)]
            = SfxSet(SfxEnumVec_t { sound_effect::Shards });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ItemGive)]
            = SfxSet(SfxEnumVec_t { sound_effect::ItemGive });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ItemDrop)]
            = SfxSet(SfxEnumVec_t { sound_effect::ItemDrop });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::BlowpipeShoot)]
            = SfxSet(SfxEnumVec_t { sound_effect::BlowpipeShoot1,
                                    sound_effect::BlowpipeShoot2,
                                    sound_effect::BlowpipeShoot3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::BlowpipeHit)] = SfxSet(SfxEnumVec_t {
            sound_effect::BlowpipeHit1, sound_effect::BlowpipeHit2, sound_effect::BlowpipeHit3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ArrowShoot)]
            = SfxSet(SfxEnumVec_t { sound_effect::ArrowShoot1,
                                    sound_effect::ArrowShoot2,
                                    sound_effect::ArrowShoot3,
                                    sound_effect::ArrowShoot4,
                                    sound_effect::ArrowShoot5,
                                    sound_effect::ArrowShoot6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ArrowHit)]
            = SfxSet(SfxEnumVec_t { sound_effect::ArrowHit });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ProjectileMiss)]
            = SfxSet(SfxEnumVec_t { sound_effect::ProjectileMiss1,
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

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MeleeMiss)] = SfxSet(SfxEnumVec_t {
            sound_effect::MeleeMiss1,  sound_effect::MeleeMiss2,  sound_effect::MeleeMiss3,
            sound_effect::MeleeMiss4,  sound_effect::MeleeMiss5,  sound_effect::MeleeMiss6,
            sound_effect::MeleeMiss7,  sound_effect::MeleeMiss8,  sound_effect::MeleeMiss9,
            sound_effect::MeleeMiss10, sound_effect::MeleeMiss11, sound_effect::MeleeMiss12,
            sound_effect::MeleeMiss13, sound_effect::MeleeMiss14, sound_effect::MeleeMiss15,
            sound_effect::MeleeMiss16, sound_effect::MeleeMiss17, sound_effect::MeleeMiss18,
            sound_effect::MeleeMiss19, sound_effect::MeleeMiss20, sound_effect::MeleeMiss21,
            sound_effect::MeleeMiss22, sound_effect::MeleeMiss23 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::FistHit)]
            = SfxSet(SfxEnumVec_t { sound_effect::FistHit1,
                                    sound_effect::FistHit2,
                                    sound_effect::FistHit3,
                                    sound_effect::FistHit4,
                                    sound_effect::FistHit5,
                                    sound_effect::FistHit6,
                                    sound_effect::FistHit7,
                                    sound_effect::FistHit8 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::TentacleHit)]
            = SfxSet(SfxEnumVec_t { sound_effect::TentacleHit1,
                                    sound_effect::TentacleHit2,
                                    sound_effect::TentacleHit3,
                                    sound_effect::TentacleHit4 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::WhipHit)]
            = SfxSet(SfxEnumVec_t { sound_effect::WhipHit });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::WhipMiss)]
            = SfxSet(SfxEnumVec_t { sound_effect::WhipMiss1,
                                    sound_effect::WhipMiss2,
                                    sound_effect::WhipMiss3,
                                    sound_effect::WhipMiss4,
                                    sound_effect::WhipMiss5,
                                    sound_effect::WhipMiss6,
                                    sound_effect::WhipMiss7,
                                    sound_effect::WhipMiss8,
                                    sound_effect::WhipMiss9,
                                    sound_effect::WhipMiss10 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MaterialHitMetal)]
            = SfxSet(SfxEnumVec_t { sound_effect::MaterialHitMetal1,
                                    sound_effect::MaterialHitMetal2,
                                    sound_effect::MaterialHitMetal3,
                                    sound_effect::MaterialHitMetal4,
                                    sound_effect::MaterialHitMetal5 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::MaterialHitMisc)]
            = SfxSet(SfxEnumVec_t { sound_effect::MaterialHitBone,
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

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::ClawHit)]
            = SfxSet(SfxEnumVec_t { sound_effect::ClawTear });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::Wind)] = SfxSet(SfxEnumVec_t {
            sound_effect::WindGust1, sound_effect::WindGust2, sound_effect::WindGust3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::SpellSelect)] = SfxSet(SfxEnumVec_t {
            sound_effect::SpellSelect1, sound_effect::SpellSelect2, sound_effect::SpellSelect3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::SpellBandage)]
            = SfxSet(SfxEnumVec_t { sound_effect::SpellBandage1,
                                    sound_effect::SpellBandage2,
                                    sound_effect::SpellBandage3,
                                    sound_effect::SpellBandage4 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::DrumBlip)]
            = SfxSet(SfxEnumVec_t { sound_effect::DrumBlip1,
                                    sound_effect::DrumBlip2,
                                    sound_effect::DrumBlip3,
                                    sound_effect::DrumBlip4,
                                    sound_effect::DrumBlip5,
                                    sound_effect::DrumBlip6 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::GuitarStrum)]
            = SfxSet(SfxEnumVec_t { sound_effect::GuitarStrum1, sound_effect::GuitarStrum2 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::CombatWin)] = SfxSet(SfxEnumVec_t {
            sound_effect::CombatWin1, sound_effect::CombatWin2, sound_effect::CombatWin3 });

        sfxSetVec_[static_cast<std::size_t>(sound_effect_set::CombatLose)] = SfxSet(SfxEnumVec_t {
            sound_effect::CombatLose1, sound_effect::CombatLose2, sound_effect::CombatLose3 });
    }

    void
        SoundManager::MusicStart(const music::Enum WHICH, const float FADE_MULT, const float VOLUME)
    {
        MusicStart(
            MusicEnumVec_t(1, WHICH), false, false, FADE_MULT, VOLUME, music::IsLooped(WHICH));
    }

    void SoundManager::MusicStart(
        const MusicEnumVec_t & WHICH_VEC,
        const bool WILL_RANDOMIZE,
        const bool WILL_START_AT_RANDOM,
        const float FADE_MULT,
        const float VOLUME,
        const bool WILL_LOOP)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (WHICH_VEC.empty() == false),
            "SoundManager::MusicStart(vector version) given an empty vector.");

        // use the global volume if an invalid volume is specified
        const float VOLUME_TO_USE((VOLUME < 0.0f) ? musicVolume_ : VOLUME);

        // Create the MusicSet object here, before we know if it is needed, because its
        // constructor is where the CurrentlyPlaying() song must be selected from among WHICH_VEC.
        MusicSet musicSet(
            WHICH_VEC, WILL_RANDOMIZE, WILL_START_AT_RANDOM, FADE_MULT, VOLUME_TO_USE, WILL_LOOP);

        const music::Enum WHICH_MUSIC_TO_START_PLAYING(musicSet.CurrentlyPlaying());

        // if there is only one song in the vector, and it is the music that should start,
        // and it is already playing, then simply set the volume to whatever is specified
        if (WHICH_VEC.size() == 1)
        {
            for (auto & songSet : songSetVec_)
            {
                if (songSet.IsValid()
                    && (songSet.set.CurrentlyPlaying() == WHICH_MUSIC_TO_START_PLAYING))
                {
                    songSet.op.VolumeFadeTo(VOLUME_TO_USE, FADE_MULT);
                    songSet.op.WillKillAfterFadeOut(false);
                    return;
                }
            }
        }

        songSetVec_.emplace_back(SongSet(
            musicSet,
            MakeAndStartMusicOperator(WHICH_MUSIC_TO_START_PLAYING, FADE_MULT, VOLUME_TO_USE)));
    }

    void SoundManager::MusicStop(const music::Enum WHICH, const float FADE_MULT)
    {
        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid()
                && (((songSet.set.Count() == 1) && (songSet.op.Info().Which() == WHICH))
                    || (WHICH == music::All)))
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
                const auto CONTAINS_ALL { [&]() {
                    for (const auto MUSIC : MUSIC_ENUMS)
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

        for (const auto & SONG_SET : songSetVec_)
        {
            if (SONG_SET.IsValid())
            {
                musicInfos.emplace_back(SONG_SET.op.Info());
            }
        }

        return musicInfos;
    }

    void SoundManager::MusicVolume(
        const MusicEnumVec_t & MUSIC_ENUMS, const float NEW_VOLUME, const float FADE_MULT)
    {
        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                const auto CONTAINS_ALL { [&]() {
                    for (const auto MUSIC : MUSIC_ENUMS)
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
        SongsUpdate(ELAPSED_TIME_SECONDS);
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
            // the Volume() function pre-checks if IsValid(), so this is safe
            sfxWrapper.Volume(effectsVolume_);
        }
    }

    void SoundManager::MusicVolumeFadeToCurrent(const music::Enum MUSIC_ENUM)
    {
        for (auto & songSet : songSetVec_)
        {
            if ((songSet.IsValid()) && (songSet.op.Info().Which() == MUSIC_ENUM))
            {
                const float CURRENT_VOLUME(songSet.op.Volume());
                const float INTENDED_VOLUME(gui::SoundManager::Instance()->MusicVolume());

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
                            ((CURRENT_VOLUME < INTENDED_VOLUME)
                                 ? gui::MusicOperator::FADE_MULT_DEFAULT_IN_
                                 : gui::MusicOperator::FADE_MULT_DEFAULT_OUT_));
                    }
                }
            }
        }
    }

    const SfxSet & SoundManager::GetSoundEffectSet(const sound_effect_set::Enum ENUM) const
    {
        const auto INDEX { static_cast<std::size_t>(ENUM) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < sfxSetVec_.size()),
            "SoundManager::GetSoundEffectSet(enum="
                << ENUM << ", index=" << INDEX
                << ") index was out of range with sfxSetVec_.size()=" << sfxSetVec_.size());

        return sfxSetVec_[INDEX];
    }

    void SoundManager::SoundEffectPlay(
        const sound_effect::Enum SFX_ENUM, const bool WILL_LOOP, const float VOLUME_RATIO)
    {
        if (IsSfxEnumValid(SFX_ENUM))
        {
            PreLoadSfx(SFX_ENUM, WILL_LOOP, VOLUME_RATIO);

            // This restarts play from the beginning, which is the
            // desired behavior even if it was already playing,
            // because I decided that repeated plays of the same
            // sfx will restart if not allowed to finish.
            GetSfxWrapper(SFX_ENUM).Play();
        }
    }

    void SoundManager::SoundEffectPlay(
        const sound_effect::Enum SFX_ENUM,
        const float PRE_DELAY_SEC,
        const bool WILL_LOOP,
        const float VOLUME_RATIO)
    {
        if (IsSfxEnumValid(SFX_ENUM))
        {
            delayedSfxVec_.emplace_back(
                DelayedSfx(SFX_ENUM, PRE_DELAY_SEC, WILL_LOOP, VOLUME_RATIO));
        }
    }

    void SoundManager::SoundEffectStop(const sound_effect::Enum SFX_ENUM)
    {
        if (IsSfxEnumValid(SFX_ENUM))
        {
            auto & sfxWrapper { GetSfxWrapper(SFX_ENUM) };
            if (sfxWrapper.IsValid())
            {
                sfxWrapper.Stop();
            }
        }
    }

    void SoundManager::PlaySfx_AckMinor()
    {
        GetSoundEffectSet(gui::sound_effect_set::Switch).PlayRandom();
    }

    void SoundManager::PlaySfx_AckMajor()
    {
        GetSoundEffectSet(gui::sound_effect_set::Thock).PlayRandom();
    }

    void SoundManager::PlaySfx_Reject() { SoundEffectPlay(gui::sound_effect::PromptWarn); }

    void SoundManager::PlaySfx_TickOn()
    {
        GetSoundEffectSet(gui::sound_effect_set::TickOn).PlayRandom();
    }

    void SoundManager::PlaySfx_TickOff()
    {
        GetSoundEffectSet(gui::sound_effect_set::TickOff).PlayRandom();
    }

    void SoundManager::PlaySfx_Keypress() { PlaySfx_AckMinor(); }

    void SoundManager::PlaySfx_MouseClick() { PlaySfx_AckMinor(); }

    void SoundManager::ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX)
    {
        // eliminate buffers for sound effects that are done playing
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
            delayedSfxVec_.clear();
        }
    }

    void SoundManager::PreLoadSfx(
        const sound_effect::Enum SFX_ENUM, const bool WILL_LOOP, const float VOLUME_RATIO)
    {
        auto & sfxWrapper { GetSfxWrapper(SFX_ENUM) };

        if (sfxWrapper.IsValid() == false)
        {
            sfxWrapper = SfxWrapper(
                SFX_ENUM,
                std::make_unique<sf::Sound>(),
                LoadSfxBuffer(SFX_ENUM),
                WILL_LOOP,
                VOLUME_RATIO);
        }
        else
        {
            sfxWrapper.WillLoop(WILL_LOOP);
            sfxWrapper.VolumeRatio(VOLUME_RATIO);
        }

        sfxWrapper.Volume(SoundEffectVolume());
    }

    void SoundManager::PreLoadSfx(const SfxEnumVec_t & SFX_VEC)
    {
        for (const auto SFX_ENUM : SFX_VEC)
        {
            PreLoadSfx(SFX_ENUM);
        }
    }

    SfxWrapper & SoundManager::GetSfxWrapper(const sound_effect::Enum SFX_ENUM)
    {
        const auto INDEX { static_cast<std::size_t>(SFX_ENUM) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < sfxWrapperVec_.size()),
            "SoundManager::GetSfxWrapper(" << SFX_ENUM << ") but that index was out of range.");

        return sfxWrapperVec_[INDEX];
    }

    MusicUPtr_t SoundManager::OpenMusic(
        const std::string & MUSIC_FILE_NAME, const std::string & MUSIC_DIR_NAME) const
    {
        const auto PATH_STR_COMPLETE { misc::filesystem::CombinePaths(
            musicDirectoryPath_, MUSIC_DIR_NAME, MUSIC_FILE_NAME) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsFile(PATH_STR_COMPLETE)),
            "SoundManager::OpenMusic(\""
                << PATH_STR_COMPLETE
                << "\") failed because that file either does not exist or is not a regular file.");

        auto musicUPtr { std::make_unique<sf::Music>() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (musicUPtr->openFromFile(PATH_STR_COMPLETE)),
            "SoundManager::OpenMusic(\"" << PATH_STR_COMPLETE
                                         << "\"), sf::Music::OpenFromFile() returned false.  "
                                         << "See console output for more information.");

        return musicUPtr;
    }

    void SoundManager::CacheMusicInfo_CombatIntro()
    {
        const auto DIR_PATH_STR { misc::filesystem::CombinePaths(
            musicDirectoryPath_, music::Directory(music::CombatIntro)) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsDirectory(DIR_PATH_STR)),
            "gui::SoundManager::CacheMusicInfo_CombatIntro() but the directory does not "
            "exist: "
                << DIR_PATH_STR);

        const auto MUSIC_FILE_PATHS { misc::filesystem::FindFiles(
            false, DIR_PATH_STR, "", music::FileExt()) };

        combatIntroMusicInfoVec_.clear();

        for (const auto & MUSIC_FILE_PATH_STR : MUSIC_FILE_PATHS)
        {
            const auto FILE_NAME_STR { misc::filesystem::Filename(MUSIC_FILE_PATH_STR) };

            const std::vector<std::string> FILE_NAME_PARTS_VEC { misc::SplitByChars(
                FILE_NAME_STR, misc::SplitHow('_')) };
            ;
            if (FILE_NAME_PARTS_VEC.size() != 4)
            {
                continue;
            }

            const auto NEXT_ARTIST_NAME { FILE_NAME_PARTS_VEC.at(1) };
            const auto NEXT_TRACK_NAME { FILE_NAME_PARTS_VEC.at(2) };

            const auto NEXT_LICENSE_NAME { boost::algorithm::erase_all_copy(
                FILE_NAME_PARTS_VEC.at(3), music::FileExt()) };

            combatIntroMusicInfoVec_.emplace_back(MusicInfo(
                music::CombatIntro,
                false,
                NEXT_ARTIST_NAME,
                NEXT_TRACK_NAME,
                NEXT_LICENSE_NAME,
                FILE_NAME_STR,
                DIR_PATH_STR));
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (combatIntroMusicInfoVec_.empty() == false),
            "gui::SoundManager::CacheMusicInfo_CombatIntro() failed to load any music info "
            "from the directory: "
                << DIR_PATH_STR);
    }

    MusicOperator SoundManager::MakeAndStartMusicOperator(
        const music::Enum MUSIC_ENUM, const float FADE_MULT, const float VOLUME) const
    {
        MusicUPtr_t musicUPtr;
        MusicInfo musicInfo(MUSIC_ENUM);

        if (musicInfo.Which() == music::CombatIntro)
        {
            musicInfo = misc::RandomSelect(combatIntroMusicInfoVec_);
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

    bool SoundManager::Test(stage::IStagePtr_t iStagePtr)
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            iStagePtr->TestingStrAppend("gui::SoundManager::Test() Starting Tests...");
        }

        static auto counter { 0 };
        static auto playOrStop { false };
        static const auto MUSIC_COUNT_MAX { 200 };

        // test sound effects individually
        {
            static auto hasSFXPromptedStart { false };
            if (false == hasSFXPromptedStart)
            {
                iStagePtr->TestingStrIncrement("SoundManager SFX Tests starting...");
                hasSFXPromptedStart = true;
            }

            static EnumUnderlying_t sfxIndex { 0 };
            if (sfxIndex < sound_effect::Count)
            {
                const auto ENUM { static_cast<sound_effect::Enum>(sfxIndex) };
                const auto ENUM_STR { sound_effect::ToString(ENUM) };

                iStagePtr->TestingStrIncrement("SoundManager SFX Test \"" + ENUM_STR + "\"");

                SoundEffectPlay(ENUM);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                ++sfxIndex;
                return false;
            }

            static auto hasSFXPromptedEnd { false };
            if (false == hasSFXPromptedEnd)
            {
                iStagePtr->TestingStrIncrement("SoundManager SFX Tests finished.  All Passed.");
                hasSFXPromptedEnd = true;
                ClearSoundEffectsCache();
            }
        }

        // test sound effects through SfxSet interface
        {
            static auto hasStaticSFXPromptedStart { false };
            if (false == hasStaticSFXPromptedStart)
            {
                iStagePtr->TestingStrIncrement("SoundManager SfxSet SFX Tests starting...");

                hasStaticSFXPromptedStart = true;
            }

            static EnumUnderlying_t sfxSetIndex { 0 };
            if (sfxSetIndex < sfxSetVec_.size())
            {
                static std::size_t sfxSetInnerIndex { 0 };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (sfxSetVec_.at(sfxSetIndex).IsValid()),
                    "gui::SoundManager::Test() While testing SoudEffectsSets #"
                        << sfxSetIndex << ", enum="
                        << sound_effect::ToString(static_cast<sound_effect::Enum>(sfxSetIndex))
                        << " found IsValid()==false.");

                if (false == TestSfxSet(sfxSetVec_.at(sfxSetIndex), sfxSetInnerIndex))
                {
                    ++sfxSetInnerIndex;
                    return false;
                }
                else
                {
                    std::ostringstream ss;
                    ss << sfxSetIndex;
                    iStagePtr->TestingStrIncrement(
                        "SoundManager SfxSet SFX Tested Set #" + ss.str());

                    sfxSetInnerIndex = 0;
                    ++sfxSetIndex;
                }

                return false;
            }

            static auto hasStaticSFXPromptedEnd { false };
            if (false == hasStaticSFXPromptedEnd)
            {
                iStagePtr->TestingStrIncrement(
                    "SoundManager SfxSet SFX Tests finished.  All Passed.");

                hasStaticSFXPromptedEnd = true;
                ClearSoundEffectsCache();
            }
        }

        // test regular music
        static EnumUnderlying_t musicIndex { 0 };
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
                    iStagePtr->TestingStrIncrement(
                        "SoundManager Music Test #" + ss.str() + " Delay...");

                    ++counter;
                    return false;
                }

                if (playOrStop)
                {
                    MusicStop(NEXT_ENUM);
                    playOrStop = false;
                    ++musicIndex;
                    iStagePtr->TestingStrIncrement("SoundManager Music Test #" + ss.str());
                    return false;
                }
            }
        }

        iStagePtr->TestingStrAppend("gui::SoundManager::Test() ALL TESTS PASSED");
        return true;
    }

    bool SoundManager::TestSfxSet(SfxSet & soundEffectsSet, const std::size_t INDEX)
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

    void SoundManager::SongsUpdate(const float ELAPSED_TIME_SECONDS)
    {
        auto willCleanup { false };

        for (auto & songSet : songSetVec_)
        {
            if (songSet.IsValid())
            {
                const auto UPDATE_STATUS { songSet.op.UpdateTime(ELAPSED_TIME_SECONDS) };

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
                        const float VOLUME_TO_USE(
                            (songSet.set.Volume() < 0.0f) ? musicVolume_ : songSet.set.Volume());

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
            songSetVec_.erase(
                std::remove_if(
                    songSetVec_.begin(),
                    songSetVec_.end(),
                    [](const auto & SONG_SET) { return (SONG_SET.IsValid() == false); }),
                songSetVec_.end());
        }
    }

    void SoundManager::SoundEffectsUpdate(const float ELAPSED_TIME_SEC)
    {
        auto willCleanup { false };

        for (auto & delayedSfx : delayedSfxVec_)
        {
            delayedSfx.delay_sec_remaining -= ELAPSED_TIME_SEC;

            if (delayedSfx.IsTimeToPlay())
            {
                willCleanup = true;
                SoundEffectPlay(delayedSfx.sfx_enum, delayedSfx.will_loop, delayedSfx.volume_ratio);
            }
        }

        if (willCleanup)
        {
            delayedSfxVec_.erase(
                std::remove_if(
                    std::begin(delayedSfxVec_),
                    std::end(delayedSfxVec_),
                    [](const auto & DELAYED_SFX) { return DELAYED_SFX.IsTimeToPlay(); }),
                std::end(delayedSfxVec_));
        }
    }

    SoundBufferUPtr_t SoundManager::LoadSfxBuffer(const sound_effect::Enum ENUM) const
    {
        const auto SOUND_FILE_PATH_STR { misc::filesystem::CombinePaths(
            soundsDirectoryPath_, sound_effect::Directory(ENUM), sound_effect::Filename(ENUM)) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (misc::filesystem::ExistsAndIsFile(SOUND_FILE_PATH_STR)),
            "gui::SoundManager::LoadSound(" << sound_effect::ToString(ENUM)
                                            << "), attempting path=\"" << SOUND_FILE_PATH_STR
                                            << "\", failed because that file does not exist.");

        auto bufferUPtr { std::make_unique<sf::SoundBuffer>() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            bufferUPtr->loadFromFile(SOUND_FILE_PATH_STR),
            "gui::SoundManager::LoadSound("
                << sound_effect::ToString(ENUM) << "), attempting path=\"" << SOUND_FILE_PATH_STR
                << "\", sf::SoundBuffer::loadFromFile() returned false.  See console output"
                << " for more information.");

        return bufferUPtr;
    }

} // namespace gui
} // namespace heroespath
