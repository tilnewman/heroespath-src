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


namespace sfml_util
{
    class SoundManager;
    using SoundManagerSPtr_t = std::shared_ptr<SoundManager>;

    using SoundBufferRefCountPair_t = std::pair<SoundBufferSPtr_t, std::size_t>;
    using SoundBufferRefCountVec_t = std::vector<SoundBufferRefCountPair_t>;


    struct SoundEffectData
    {
        sf::Sound sound;
        sf::SoundBuffer buffer;
    };

    using SoundEffectDataSPtr_t = std::shared_ptr<SoundEffectData>;
    using SoundEffectDataSVec_t = std::vector<SoundEffectDataSPtr_t>;


    //simple wrapper responsible for everything needed to describe and operate a set of song(s)
    struct Songs
    {
        Songs(const MusicSetSPtr_t &      SET_SPTR      = MusicSetSPtr_t(),
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
        virtual ~SoundManager();

        static SoundManagerSPtr_t Instance();
        static void SetSoundsDirectory(const std::string & SOUND_DIR_PATH,
                                       const std::string & MUSIC_DIR_PATH);

        bool Test();
        bool TestSoundEffectsSet(SoundEffectsSetSPtr_t &, const std::size_t INDEX);

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

        inline SoundEffectsSetPtr_t SoundEffectsSet_Prompt() const            { return promptSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_Switch() const            { return switchSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_TickOn() const            { return tickOnSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_TickOff() const           { return tickOffSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_Thock() const             { return thockSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_Coin() const              { return coinSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_Gem() const               { return gemSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_MeteorShard() const       { return meteorShardSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ItemGive() const          { return itemGiveSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ItemDrop() const          { return itemDropSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_BlowpipeShoot() const     { return blowpipeShootSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_BlowpipeHit() const       { return blowpipeHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ArrowShoot() const        { return arrowShootSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ArrowHit() const          { return arrowHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ProjectileMiss() const    { return projectileMissSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_MeleeMiss() const         { return meleeMissSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_FistHit() const           { return fistHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_BreathHit() const         { return breathHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_TendrilHit() const        { return tendrilHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_WhipHit() const           { return whipHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_WhipMiss() const          { return whipMissSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_MaterialHitMetal() const  { return materialHitMetalSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_MaterialHitMisc() const   { return materialHitMiscSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_ClawTear() const          { return clawHitSoundsSPtr_.get(); }
        inline SoundEffectsSetPtr_t SoundEffectsSet_Wind() const              { return windSoundsSPtr_.get(); }

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
                       SoundEffectDataSPtr_t &) const;

    private:
        static std::string soundsDirectoryPath_;
        static std::string musicDirectoryPath_;
        static SoundManagerSPtr_t instanceSPtr_;
        //
        float musicVolume_;
        float effectsVolume_;
        MusicSVec_t musicSVec_;
        SoundBufferRefCountVec_t soundEffectBufferPairVec_;
        SoundEffectsSetSPtr_t promptSoundsSPtr_;
        SoundEffectsSetSPtr_t switchSoundsSPtr_;
        SoundEffectsSetSPtr_t tickOnSoundsSPtr_;
        SoundEffectsSetSPtr_t tickOffSoundsSPtr_;
        SoundEffectsSetSPtr_t thockSoundsSPtr_;
        SoundEffectsSetSPtr_t coinSoundsSPtr_;
        SoundEffectsSetSPtr_t gemSoundsSPtr_;
        SoundEffectsSetSPtr_t meteorShardSoundsSPtr_;
        SoundEffectsSetSPtr_t itemGiveSoundsSPtr_;
        SoundEffectsSetSPtr_t itemDropSoundsSPtr_;
        SoundEffectsSetSPtr_t blowpipeShootSoundsSPtr_;
        SoundEffectsSetSPtr_t blowpipeHitSoundsSPtr_;
        SoundEffectsSetSPtr_t arrowShootSoundsSPtr_;
        SoundEffectsSetSPtr_t arrowHitSoundsSPtr_;
        SoundEffectsSetSPtr_t projectileMissSoundsSPtr_;
        SoundEffectsSetSPtr_t meleeMissSoundsSPtr_;
        SoundEffectsSetSPtr_t fistHitSoundsSPtr_;
        SoundEffectsSetSPtr_t breathHitSoundsSPtr_;
        SoundEffectsSetSPtr_t tendrilHitSoundsSPtr_;
        SoundEffectsSetSPtr_t whipHitSoundsSPtr_;
        SoundEffectsSetSPtr_t whipMissSoundsSPtr_;
        SoundEffectsSetSPtr_t materialHitMetalSoundsSPtr_;
        SoundEffectsSetSPtr_t materialHitMiscSoundsSPtr_;
        SoundEffectsSetSPtr_t clawHitSoundsSPtr_;
        SoundEffectsSetSPtr_t windSoundsSPtr_;
        SoundEffectsSetSVec_t soundEffectsSetSVec_;
        MusicInfoSVec_t combatIntroMusicInfoVec_;
        SongsVec_t songsVec_;
        SoundEffectEnumVec_t soundEffectsToPlayVec_;
        SoundEffectDataSVec_t soundEffectsSVec_;
    };

    using SoundManagerPtr_t  = SoundManager *;
    using SoundManagerSPtr_t = std::shared_ptr<SoundManager>;

}
#endif //SFMLUTIL_SOUNDMANAGER_INCLUDED
