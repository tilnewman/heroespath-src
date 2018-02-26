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
#ifndef HEROESPATH_SFMLUTIL_SOUNDMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SOUNDMANAGER_HPP_INCLUDED
//
// sound-manager.hpp
//  Aound managing class that handles both sf::Sound and sf::Music instances.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/music-set.hpp"
#include "sfml-util/songs.hpp"
#include "sfml-util/sound-effects-set.hpp"
#include "sfml-util/sfx-set-enum.hpp"
#include "sfml-util/sfx-wrapper.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <utility>


namespace heroespath
{
namespace sfml_util
{

    using SfxDelayPair_t = std::pair<sound_effect::Enum, float>;
    using SfxDelayVec_t = std::vector<SfxDelayPair_t>;


    //A class that loads, stores, and distributes sounds
    class SoundManager
    {
        SoundManager(const SoundManager &) =delete;
        SoundManager & operator=(const SoundManager &) =delete;

    public:
        SoundManager();
        ~SoundManager();

        static SoundManager * Instance();
        static void Acquire();
        static void Release();

        static void SetSoundsDirectory(
            const std::string & SOUND_DIR_PATH,
            const std::string & MUSIC_DIR_PATH);

        bool Test();
        bool TestSfxSet(SfxSet &, const std::size_t INDEX);

        void LoadSoundSets();

        //throws range_error on invalid enum input.
        void MusicStart(
            const music::Enum WHICH,
            const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float VOLUME    = MusicOperator::VOLUME_USE_GLOBAL_);

        void MusicStart(
            const MusicEnumVec_t & WHICH_VEC,
            const bool             WILL_RANDOMIZE       = true,
            const bool             WILL_START_AT_RANDOM = true,
            const float            FADE_MULT            = MusicOperator::FADE_MULT_DEFAULT_IN_,
            const float            VOLUME               = MusicOperator::VOLUME_USE_GLOBAL_,
            const bool             WILL_LOOP            = true);

        void MusicStop(
            const music::Enum WHICH = sfml_util::music::All,
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

        //Note:  These functions don't actually change the volume of any sound objects in memory.
        //       See Loop::Start/StopMusic() functions for the actual volume handling code.
        //       All background music shares a volume, and all sound effects share a volume.
        inline float MusicVolume() const { return musicVolume_; }
        void MusicVolumeSet(const float);
        //
        inline float SoundEffectVolume() const  { return effectsVolume_; }
        void SoundEffectVolumeSet(const float V);

        void MusicVolumeFadeToCurrent(const music::Enum);

        const SfxSet & Getsound_effect_set(const sound_effect_set::Enum) const;

        void SoundEffectPlay(
            const sound_effect::Enum SFX_ENUM,
            const float PRE_DELAY_SEC = 0.0f);

        void SoundEffectPlayNow(const sound_effect::Enum);

        void PlaySfx_AckMinor();
        void PlaySfx_AckMajor();
        void PlaySfx_Reject();
        void PlaySfx_TickOn();
        void PlaySfx_TickOff();
        void PlaySfx_Keypress();
        void PlaySfx_MouseClick();

        void ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX = false);

    private:
        MusicUPtr_t OpenMusic(
            const std::string & MUSIC_FILE_NAME,
            const std::string & MUSIC_DIR_NAME) const;

        void CacheMusicInfo_CombatIntro();

        MusicOperator MakeAndStartMusicOperator(
            const music::Enum MUSIC_ENUM,
            const float       FADE_MULT,
            const float       VOLUME) const;

        void SongsUpdate(const float ELAPSED_TIME_SEC);

        void SoundEffectsUpdate(const float ELAPSED_TIME_SEC);

        SoundBufferUPtr_t LoadSfxBuffer(const sound_effect::Enum) const;

        inline bool IsSfxDelayPairReadyToPlay(const SfxDelayPair_t & SDP) const
        {
            return (SDP.second < 0.0f);
        }

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
        SfxDelayVec_t sfxToPlayPairsVec_;
        SfxWrapperVec_t sfxWrapperVec_;
    };

}
}

#endif //HEROESPATH_SFMLUTIL_SOUNDMANAGER_HPP_INCLUDED
