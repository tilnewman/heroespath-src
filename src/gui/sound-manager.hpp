// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SOUNDMANAGER_HPP_INCLUDED
#define HEROESPATH_GUI_SOUNDMANAGER_HPP_INCLUDED
//
// sound-manager.hpp
//  Sound managing class that handles both sf::Sound and sf::Music instances.
//
#include "gui/music-enum.hpp"
#include "gui/music-operator.hpp"
#include "gui/music-set.hpp"
#include "gui/sfx-set-enum.hpp"
#include "gui/sfx-wrapper.hpp"
#include "gui/songs.hpp"
#include "gui/sound-effects-enum.hpp"
#include "gui/sound-effects-set.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
} // namespace stage

namespace gui
{

    // Responsible for storing all the information needed to play an sfx at some time in the future.
    struct DelayedSfx
    {
        explicit DelayedSfx(
            const sound_effect::Enum SFX_ENUM = sound_effect::Count,
            const float DELAY_SEC = 0.0f,
            const bool WILL_LOOP = false,
            const float VOLUME_RATIO = 1.0f)
            : sfx_enum(SFX_ENUM)
            , delay_sec_remaining(DELAY_SEC)
            , will_loop(WILL_LOOP)
            , volume_ratio(VOLUME_RATIO)
        {}

        bool IsTimeToPlay() const { return delay_sec_remaining < 0.0f; }

        sound_effect::Enum sfx_enum;
        float delay_sec_remaining;
        bool will_loop;
        float volume_ratio;
    };

    using DelayedSfxVec_t = std::vector<DelayedSfx>;

    // A class that loads, stores, and distributes sounds
    class SoundManager
    {
    public:
        SoundManager(const SoundManager &) = delete;
        SoundManager(SoundManager &&) = delete;
        SoundManager & operator=(const SoundManager &) = delete;
        SoundManager & operator=(SoundManager &&) = delete;

        SoundManager();
        ~SoundManager();

        static misc::NotNull<SoundManager *> Instance();
        static void Acquire();
        static void Release();
        static void Initialize();

        static void SetSoundsDirectory(
            const std::string & SOUND_DIR_PATH, const std::string & MUSIC_DIR_PATH);

        void LoadSoundSets();

        // throws range_error on invalid enum input.
        void MusicStart(
            const music::Enum WHICH,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float VOLUME = MusicOperator::VOLUME_USE_GLOBAL_);

        void MusicStart(
            const MusicEnumVec_t & WHICH_VEC,
            const bool WILL_RANDOMIZE = true,
            const bool WILL_START_AT_RANDOM = true,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float VOLUME = MusicOperator::VOLUME_USE_GLOBAL_,
            const bool WILL_LOOP = true);

        void MusicStop(
            const music::Enum WHICH = gui::music::All,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_OUT_);

        void MusicStop(
            const MusicEnumVec_t & MUSIC_ENUMS,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_OUT_);

        const MusicInfoVec_t MusicInfoSet() const;

        void MusicVolume(
            const MusicEnumVec_t & MUSIC_ENUMS,
            const float NEW_VOLUME,
            const float FADE_MULT = MusicOperator::FADE_MULT_IMMEDIATE_);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        // Note:  These functions don't actually change the volume of any sound objects in memory.
        //       See Loop::Start/StopMusic() functions for the actual volume handling code.
        //       All background music shares a volume, and all sound effects share a volume.
        float MusicVolume() const { return musicVolume_; }
        void MusicVolumeSet(const float);
        //
        float SoundEffectVolume() const { return effectsVolume_; }
        void SoundEffectVolumeSet(const float V);

        void MusicVolumeFadeToCurrent(const music::Enum);

        const SfxSet & GetSoundEffectSet(const sound_effect_set::Enum) const;

        void SoundEffectPlay(
            const sound_effect::Enum,
            const bool WILL_LOOP = false,
            const float VOLUME_RATIO = 1.0f);

        void SoundEffectPlay(
            const sound_effect::Enum SFX_ENUM,
            const float PRE_DELAY_SEC,
            const bool WILL_LOOP = false,
            const float VOLUME_RATIO = 1.0f);

        void SoundEffectStop(const sound_effect::Enum);

        void PlaySfx_AckMinor();
        void PlaySfx_AckMajor();
        void PlaySfx_Reject();
        void PlaySfx_TickOn();
        void PlaySfx_TickOff();
        void PlaySfx_Keypress();
        void PlaySfx_MouseClick();

        void ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX = false);

        void PreLoadSfx(
            const sound_effect::Enum,
            const bool WILL_LOOP = false,
            const float VOLUME_RATIO = 1.0f);

        void PreLoadSfx(const SfxEnumVec_t &);

    private:
        bool IsSfxEnumValid(const sound_effect::Enum SFX_ENUM) const
        {
            return (
                (SFX_ENUM != sound_effect::None) && (SFX_ENUM != sound_effect::Count)
                && (SFX_ENUM != sound_effect::Random));
        }

        SfxWrapper & GetSfxWrapper(const sound_effect::Enum SFX_ENUM);

        MusicUPtr_t OpenMusic(
            const std::string & MUSIC_FILE_NAME, const std::string & MUSIC_DIR_NAME) const;

        void CacheMusicInfo_CombatIntro();

        MusicOperator MakeAndStartMusicOperator(
            const music::Enum MUSIC_ENUM, const float FADE_MULT, const float VOLUME) const;

        void SongsUpdate(const float ELAPSED_TIME_SEC);

        void SoundEffectsUpdate(const float ELAPSED_TIME_SEC);

        SoundBufferUPtr_t LoadSfxBuffer(const sound_effect::Enum) const;

    private:
        static std::string soundsDirectoryPath_;
        static std::string musicDirectoryPath_;
        static std::unique_ptr<SoundManager> instanceUPtr_;
        //
        float musicVolume_;
        float effectsVolume_;
        SfxSetVec_t sfxSetVec_;
        MusicInfoVec_t combatIntroMusicInfoVec_;
        SongSetVec_t songSetVec_;
        DelayedSfxVec_t delayedSfxVec_;
        SfxWrapperVec_t sfxWrapperVec_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SOUNDMANAGER_HPP_INCLUDED
