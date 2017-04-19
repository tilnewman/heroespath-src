#ifndef APPBASE_SFMLUTIL_SOUNDMANAGER_INCLUDED
#define APPBASE_SFMLUTIL_SOUNDMANAGER_INCLUDED
//
// sound-manager.hpp
//  Aound managing class that handles both sf::Sound and sf::Music instances.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/music-operator.hpp"
#include "sfml-util/music-set.hpp"

#include <memory>
#include <string>
#include <vector>
#include <tuple>


namespace sfml_util
{

    class StaticSounds;
    using StaticSoundsSPtr_t = std::shared_ptr<StaticSounds>;
    using StaticSoundsSVec_t = std::vector<StaticSoundsSPtr_t>;

    class SoundManager;
    using SoundManagerSPtr_t = std::shared_ptr<SoundManager>;


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
        SoundManager(const SoundManager &);

        //prevent copy assignment
        SoundManager & operator=(const SoundManager &);

        //prevent non-singleton construction
        SoundManager();

    public:
        virtual ~SoundManager();

        static SoundManagerSPtr_t Instance();
        static void SetSoundsDirectory(const std::string & SOUND_DIR_PATH,
                                       const std::string & MUSIC_DIR_PATH);

        bool Test();
        bool TestStaticSounds(StaticSoundsSPtr_t &, const std::size_t INDEX);

        void LoadStaticSoundSets();

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

        //These functions throws range_error on invalid enum input.
        SoundSPtr_t SoundEffectAcquire(const sound_effect::Enum);
        void SoundEffectRelease(const sound_effect::Enum);

        inline const StaticSoundsSPtr_t StaticSounds_Prompt() const            { return promptSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_Switch() const            { return switchSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_TickOn() const            { return tickOnSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_TickOff() const           { return tickOffSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_Thock() const             { return thockSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_Coin() const              { return coinSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_Gem() const               { return gemSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_MeteorShard() const       { return meteorShardSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ItemGive() const          { return itemGiveSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ItemDrop() const          { return itemDropSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_BlowpipeShoot() const     { return blowpipeShootSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_BlowpipeHit() const       { return blowpipeHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ArrowShoot() const        { return arrowShootSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ArrowHit() const          { return arrowHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ProjectileMiss() const    { return projectileMissSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_MeleeMiss() const         { return meleeMissSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_FistHit() const           { return fistHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_BreathHit() const         { return breathHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_TendrilHit() const        { return tendrilHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_WhipHit() const           { return whipHitSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_WhipMiss() const          { return whipMissSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_MaterialHitMetal() const  { return materialHitMetalSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_MaterialHitMisc() const   { return materialHitMiscSoundsSPtr_; }
        inline const StaticSoundsSPtr_t StaticSounds_ClawTear() const          { return clawHitSoundsSPtr_; }

    private:
        void OpenMusic(const std::string & MUSIC_FILE_NAME,
                       const std::string & MUSIC_DIR_NAME,
                       MusicSPtr_t &       musicSPtr) const;

        void LoadSound(const std::string & SOUND_FILE_NAME,
                       const std::string & SOUND_DIR_NAME,
                       SoundBufferSPtr_t & soundBufferSPtr) const;

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

    private:
        static std::string soundsDirectoryPath_;
        static std::string musicDirectoryPath_;
        static SoundManagerSPtr_t instanceSPtr_;
        //
        float              musicVolume_;
        float              effectsVolume_;
        MusicSVec_t        musicSVec_;
        SoundBufferSVec_t  soundEffectBufferSVec_;
        StaticSoundsSPtr_t promptSoundsSPtr_;
        StaticSoundsSPtr_t switchSoundsSPtr_;
        StaticSoundsSPtr_t tickOnSoundsSPtr_;
        StaticSoundsSPtr_t tickOffSoundsSPtr_;
        StaticSoundsSPtr_t thockSoundsSPtr_;
        StaticSoundsSPtr_t coinSoundsSPtr_;
        StaticSoundsSPtr_t gemSoundsSPtr_;
        StaticSoundsSPtr_t meteorShardSoundsSPtr_;
        StaticSoundsSPtr_t itemGiveSoundsSPtr_;
        StaticSoundsSPtr_t itemDropSoundsSPtr_;
        StaticSoundsSPtr_t blowpipeShootSoundsSPtr_;
        StaticSoundsSPtr_t blowpipeHitSoundsSPtr_;
        StaticSoundsSPtr_t arrowShootSoundsSPtr_;
        StaticSoundsSPtr_t arrowHitSoundsSPtr_;
        StaticSoundsSPtr_t projectileMissSoundsSPtr_;
        StaticSoundsSPtr_t meleeMissSoundsSPtr_;
        StaticSoundsSPtr_t fistHitSoundsSPtr_;
        StaticSoundsSPtr_t breathHitSoundsSPtr_;
        StaticSoundsSPtr_t tendrilHitSoundsSPtr_;
        StaticSoundsSPtr_t whipHitSoundsSPtr_;
        StaticSoundsSPtr_t whipMissSoundsSPtr_;
        StaticSoundsSPtr_t materialHitMetalSoundsSPtr_;
        StaticSoundsSPtr_t materialHitMiscSoundsSPtr_;
        StaticSoundsSPtr_t clawHitSoundsSPtr_;
        StaticSoundsSVec_t allStaticSoundsSVec_;
        MusicInfoSVec_t    combatIntroMusicInfoVec_;
        SongsVec_t         songsVec_;
    };

    using SoundManagerPtr_t  = SoundManager *;
    using SoundManagerSPtr_t = std::shared_ptr<SoundManager>;

}
#endif //APPBASE_SFMLUTIL_SOUNDMANAGER_INCLUDED
