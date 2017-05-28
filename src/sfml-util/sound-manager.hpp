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
#ifndef SFMLUTIL_SOUNDMANAGER_INCLUDED
#define SFMLUTIL_SOUNDMANAGER_INCLUDED
//
// sound-manager.hpp
//  Aound managing class that handles both sf::Sound and sf::Music instances.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/music-set.hpp"
#include "sfml-util/sound-effects-set.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <utility>


namespace sfml_util
{

    struct SoundEffectData
    {
        explicit SoundEffectData(const sf::Sound &       SOUND  = sf::Sound(),
                                 const sf::SoundBuffer & BUFFER = sf::SoundBuffer())
        :
            sound(SOUND),
            buffer(BUFFER)
        {}

        sf::Sound sound;
        sf::SoundBuffer buffer;
    };

    using SoundEffectDataVec_t = std::vector<SoundEffectData>;


    //simple wrapper responsible for everything needed to describe and operate a set of song(s)
    struct Songs
    {
        explicit  Songs(const MusicSetSPtr_t &      SET_SPTR      = MusicSetSPtr_t(),
                        const MusicOperatorSPtr_t & OPERATOR_SPTR = MusicOperatorSPtr_t())
        :
            set_sptr(SET_SPTR),
            operator_sptr(OPERATOR_SPTR)
        {}

        MusicSetSPtr_t set_sptr;
        MusicOperatorSPtr_t operator_sptr;
    };

    using SongsVec_t = std::vector<Songs>;

    inline bool operator==(const Songs & L, const Songs & R)
    {
        return std::tie(L.set_sptr, L.operator_sptr) == std::tie(R.set_sptr, R.operator_sptr);
    }

    inline bool operator!=(const Songs & L, const Songs & R)
    {
        return ! (L == R);
    }


    //A class that loads, stores, and distributes sounds
    class SoundManager
    {
        //prevent copy construction
        SoundManager(const SoundManager &) =delete;

        //prevent copy assignment
        SoundManager & operator=(const SoundManager &) =delete;

        //prevent non-singleton construction
        SoundManager();

    public:
        enum class SfxSet
        {
            Prompt = 0,
            Switch,
            TickOn,
            TickOff,
            Thock,
            Coin,
            Gem,
            MeteorShard,
            ItemGive,
            ItemDrop,
            BlowpipeShoot,
            BlowpipeHit,
            ArrowShoot,
            ArrowHit,
            ProjectileMiss,
            MeleeMiss,
            FistHit,
            BreathHit,
            TendrilHit,
            WhipHit,
            WhipMiss,
            MaterialHitMetal,
            MaterialHitMisc,
            ClawHit,
            Wind,
            Count
        };

    public:
        static SoundManager * Instance();
        static void Acquire();
        static void Release();

        static void SetSoundsDirectory(const std::string & SOUND_DIR_PATH,
                                       const std::string & MUSIC_DIR_PATH);

        bool Test();
        bool TestSoundEffectsSet(SoundEffectsSet &, const std::size_t INDEX);

        void LoadSoundSets();

        //throws range_error on invalid enum input.
        const MusicSetSPtr_t MusicStart(const music::Enum WHICH,
                                        const float       FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_IN_,
                                        const float       VOLUME    = MusicOperator::VOLUME_USE_GLOBAL_);

        const MusicSetSPtr_t MusicStart(const MusicEnumVec_t & WHICH_VEC,
                                        const bool             WILL_RANDOMIZE       = true,
                                        const bool             WILL_START_AT_RANDOM = true,
                                        const float            FADE_MULT            = MusicOperator::FADE_MULT_DEFAULT_IN_,
                                        const float            VOLUME               = MusicOperator::VOLUME_USE_GLOBAL_,
                                        const bool             WILL_LOOP            = true);

        void MusicStop(const music::Enum WHICH, const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_OUT_);

        void MusicStop(const MusicSetSPtr_t & MUSIC_SET_SPTR, const float FADE_MULT = MusicOperator::FADE_MULT_DEFAULT_OUT_);

        MusicOperatorSPtr_t MusicOperator(const music::Enum);

        MusicOperatorSLst_t MusicOperators();

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        //Note:  These functions don't actually change the volume of any sound objects in memory.
        //       See Loop::Start/StopMusic() functions for the actual volume handling code.
        //       All background music shares a volume, and all sound effects share a volume.
        inline float MusicVolume() const        { return musicVolume_; }
        void MusicVolumeSet(const float);
        //
        inline float SoundEffectVolume() const  { return effectsVolume_; }
        void SoundEffectVolumeSet(const float V);

        inline const SoundEffectsSet & SoundEffectsSet_Prompt() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Prompt)]; }

        inline const SoundEffectsSet & SoundEffectsSet_Switch() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Switch)]; }

        inline const SoundEffectsSet & SoundEffectsSet_TickOn() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TickOn)]; }

        inline const SoundEffectsSet & SoundEffectsSet_TickOff() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TickOff)]; }

        inline const SoundEffectsSet & SoundEffectsSet_Thock() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Thock)]; }

        inline const SoundEffectsSet & SoundEffectsSet_Coin() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Coin)]; }

        inline const SoundEffectsSet & SoundEffectsSet_Gem() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Gem)]; }

        inline const SoundEffectsSet & SoundEffectsSet_MeteorShard() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MeteorShard)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ItemGive() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ItemGive)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ItemDrop() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ItemDrop)]; }

        inline const SoundEffectsSet & SoundEffectsSet_BlowpipeShoot() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BlowpipeShoot)]; }

        inline const SoundEffectsSet & SoundEffectsSet_BlowpipeHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BlowpipeHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ArrowShoot() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ArrowShoot)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ArrowHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ArrowHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ProjectileMiss() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ProjectileMiss)]; }

        inline const SoundEffectsSet & SoundEffectsSet_MeleeMiss() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MeleeMiss)]; }

        inline const SoundEffectsSet & SoundEffectsSet_FistHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::FistHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_BreathHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::BreathHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_TendrilHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::TendrilHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_WhipHit() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::WhipHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_WhipMiss() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::WhipMiss)]; }

        inline const SoundEffectsSet & SoundEffectsSet_MaterialHitMetal() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MaterialHitMetal)]; }

        inline const SoundEffectsSet & SoundEffectsSet_MaterialHitMisc() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::MaterialHitMisc)]; }

        inline const SoundEffectsSet & SoundEffectsSet_ClawTear() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::ClawHit)]; }

        inline const SoundEffectsSet & SoundEffectsSet_Wind() const
            { return soundEffectsSetVec_[static_cast<std::size_t>(SfxSet::Wind)]; }

        void SoundEffectPlay(const sound_effect::Enum);
        void ClearSoundEffectsCache(const bool WILL_STOP_PLAYING_SFX = false);

    private:
        void OpenMusic(const std::string & MUSIC_FILE_NAME,
                       const std::string & MUSIC_DIR_NAME,
                       MusicSPtr_t &       musicSPtr) const;

        void CacheMusicInfo_CombatIntro();

        void MusicAcquire_CombatIntro(MusicInfo &, MusicSPtr_t &);

        void MusicAcquire_CombatIntro(MusicInfo &, MusicSPtr_t &, const std::size_t INDEX);

        inline std::size_t CombatMusicCount() const { return combatIntroMusicInfoVec_.size(); }

        const MusicInfo MusicInfoAcquire(const music::Enum WHICH, MusicSPtr_t & musicSPtr);

        void MusicOperatorStart(MusicOperatorSPtr_t & musicOperatorSPtr,
                                const MusicInfo &     MUSIC_INFO,
                                const MusicSPtr_t &   MUSIC_SPTR,
                                const float           FADE_MULT,
                                const float           VOLUME);

        void SoundEffectsUpdate();

        void LoadSound(const sound_effect::Enum,
                       SoundEffectData &) const;

    private:
        static std::string soundsDirectoryPath_;
        static std::string musicDirectoryPath_;
        static std::unique_ptr<SoundManager> instanceUPtr_;
        //
        float musicVolume_;
        float effectsVolume_;
        SoundEffectsSetVec_t soundEffectsSetVec_;
        MusicInfoVec_t combatIntroMusicInfoVec_;
        SongsVec_t songsVec_;
        SoundEffectEnumVec_t soundEffectsToPlayVec_;
        SoundEffectDataVec_t soundEffectsVec_;
    };

}
#endif //SFMLUTIL_SOUNDMANAGER_INCLUDED
