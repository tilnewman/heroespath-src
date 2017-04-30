//
// sound-manager.cpp
//
#include "sound-manager.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/static-sounds.hpp"
#include "sfml-util/random.hpp"
#include "sfml-util/music-info.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/loop-manager.hpp"

#include "stringutil/stringhelp.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <thread>


namespace sfml_util
{

    std::string        SoundManager::soundsDirectoryPath_("");
    std::string        SoundManager::musicDirectoryPath_("");
    SoundManagerSPtr_t SoundManager::instanceSPtr_;


    SoundManager::SoundManager()
    :
        musicVolume_                (0.0f),
        effectsVolume_              (0.0f),
        musicSVec_                  (),
        soundEffectBufferSVec_      (),
        promptSoundsSPtr_           (),
        switchSoundsSPtr_           (),
        tickOnSoundsSPtr_           (),
        tickOffSoundsSPtr_          (),
        thockSoundsSPtr_            (),
        coinSoundsSPtr_             (),
        gemSoundsSPtr_              (),
        meteorShardSoundsSPtr_      (),
        itemGiveSoundsSPtr_         (),
        itemDropSoundsSPtr_         (),
        blowpipeShootSoundsSPtr_    (),
        blowpipeHitSoundsSPtr_      (),
        arrowShootSoundsSPtr_       (),
        arrowHitSoundsSPtr_         (),
        projectileMissSoundsSPtr_   (),
        meleeMissSoundsSPtr_        (),
        fistHitSoundsSPtr_          (),
        breathHitSoundsSPtr_        (),
        tendrilHitSoundsSPtr_       (),
        whipHitSoundsSPtr_          (),
        whipMissSoundsSPtr_         (),
        materialHitMetalSoundsSPtr_ (),
        materialHitMiscSoundsSPtr_  (),
        clawHitSoundsSPtr_          (),
        allStaticSoundsSVec_        (),
        combatIntroMusicInfoVec_    (),
        songsVec_                   ()
    {
        musicSVec_.resize(music::Count);
        soundEffectBufferSVec_.resize(sound_effect::Count);
        CacheMusicInfo_CombatIntro();
    }


    SoundManager::~SoundManager()
    {}


    SoundManagerSPtr_t SoundManager::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset(new SoundManager);

        return instanceSPtr_;
    }


    void SoundManager::SetSoundsDirectory(const std::string & SOUND_DIR_PATH,
        const std::string & MUSIC_DIR_PATH)
    {
        soundsDirectoryPath_ = SOUND_DIR_PATH;
        musicDirectoryPath_ = MUSIC_DIR_PATH;
    }


    void SoundManager::LoadStaticSoundSets()
    {
        const SoundEffectEnumVec_t PROMPT_SOUNDS_ENUM_VEC = { sound_effect::PromptGeneric,
                                                              sound_effect::PromptQuestion,
                                                              sound_effect::PromptWarn };
        promptSoundsSPtr_.reset(new StaticSounds(PROMPT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t SWITCH_SOUNDS_ENUM_VEC = { sound_effect::Switch1,
                                                              sound_effect::Switch2,
                                                              sound_effect::Switch3,
                                                              sound_effect::Switch4 };
        switchSoundsSPtr_.reset(new StaticSounds(SWITCH_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t TICKON_SOUNDS_ENUM_VEC = { sound_effect::TickOn1,
                                                               sound_effect::TickOn2 };
        tickOnSoundsSPtr_.reset(new StaticSounds(TICKON_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t TICKOFF_SOUNDS_ENUM_VEC = { sound_effect::TickOff1,
                                                                sound_effect::TickOff2 };
        tickOffSoundsSPtr_.reset(new StaticSounds(TICKOFF_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t THOCK_SOUNDS_ENUM_VEC = { sound_effect::Thock1,
                                                              sound_effect::Thock2,
                                                              sound_effect::Thock3,
                                                              sound_effect::Thock4,
                                                              sound_effect::Thock5,
                                                              sound_effect::Thock6 };
        thockSoundsSPtr_.reset(new StaticSounds(THOCK_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t COIN_SOUNDS_ENUM_VEC = { sound_effect::Coins1,
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
        coinSoundsSPtr_.reset(new StaticSounds(COIN_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t GEM_SOUNDS_ENUM_VEC = { sound_effect::Gems };
        gemSoundsSPtr_.reset(new StaticSounds(GEM_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t METEORSHARD_SOUNDS_ENUM_VEC = { sound_effect::MeteorShards };
        meteorShardSoundsSPtr_.reset(new StaticSounds(METEORSHARD_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t ITEMGIVE_SOUNDS_ENUM_VEC = { sound_effect::ItemGive };
        itemGiveSoundsSPtr_.reset(new StaticSounds(ITEMGIVE_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t ITEMDROP_SOUNDS_ENUM_VEC = { sound_effect::ItemDrop };
        itemDropSoundsSPtr_.reset(new StaticSounds(ITEMDROP_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t BLOWPIPE_SHOOT_SOUNDS_ENUM_VEC = { sound_effect::BlowpipeShoot1,
                                                                      sound_effect::BlowpipeShoot2,
                                                                      sound_effect::BlowpipeShoot3 };
        blowpipeShootSoundsSPtr_.reset(new StaticSounds(BLOWPIPE_SHOOT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t BLOWPIPE_HIT_SOUNDS_ENUM_VEC = { sound_effect::BlowpipeHit1,
                                                                    sound_effect::BlowpipeHit2,
                                                                    sound_effect::BlowpipeHit3 };
        blowpipeHitSoundsSPtr_.reset(new StaticSounds(BLOWPIPE_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t ARROW_SHOOT_SOUNDS_ENUM_VEC = { sound_effect::ArrowShoot1,
                                                                   sound_effect::ArrowShoot2,
                                                                   sound_effect::ArrowShoot3,
                                                                   sound_effect::ArrowShoot4,
                                                                   sound_effect::ArrowShoot5,
                                                                   sound_effect::ArrowShoot6 };
        arrowShootSoundsSPtr_.reset(new StaticSounds(ARROW_SHOOT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t ARROW_HIT_SOUNDS_ENUM_VEC = { sound_effect::ArrowHit };
        arrowHitSoundsSPtr_.reset(new StaticSounds(ARROW_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t PROJECTILE_MISS_SOUNDS_ENUM_VEC = { sound_effect::ProjectileMiss1,
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
        projectileMissSoundsSPtr_.reset(new StaticSounds(ARROW_SHOOT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t MELEE_MISS_SOUNDS_ENUM_VEC = { sound_effect::MeleeMiss1,
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
                                                                  sound_effect::MeleeMiss23,
                                                                  sound_effect::MeleeMiss24,
                                                                  sound_effect::MeleeMiss25,
                                                                  sound_effect::MeleeMiss26,
                                                                  sound_effect::MeleeMiss27,
                                                                  sound_effect::MeleeMiss28,
                                                                  sound_effect::MeleeMiss29,
                                                                  sound_effect::MeleeMiss30,
                                                                  sound_effect::MeleeMiss31,
                                                                  sound_effect::MeleeMiss32,
                                                                  sound_effect::MeleeMiss33,
                                                                  sound_effect::MeleeMiss34,
                                                                  sound_effect::MeleeMiss35,
                                                                  sound_effect::MeleeMiss36,
                                                                  sound_effect::MeleeMiss37,
                                                                  sound_effect::MeleeMiss38,
                                                                  sound_effect::MeleeMiss39,
                                                                  sound_effect::MeleeMiss40,
                                                                  sound_effect::MeleeMiss41 };
        meleeMissSoundsSPtr_.reset(new StaticSounds(MELEE_MISS_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t FIST_HIT_SOUNDS_ENUM_VEC = { sound_effect::FistHit1,
                                                                sound_effect::FistHit2,
                                                                sound_effect::FistHit3,
                                                                sound_effect::FistHit4,
                                                                sound_effect::FistHit5,
                                                                sound_effect::FistHit6,
                                                                sound_effect::FistHit7,
                                                                sound_effect::FistHit8 };

        fistHitSoundsSPtr_.reset(new StaticSounds(FIST_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t BREATH_HIT_SOUNDS_ENUM_VEC = { sound_effect::BreathHit1,
                                                                  sound_effect::BreathHit2 };
        breathHitSoundsSPtr_.reset(new StaticSounds(BREATH_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t TENDRILS_HIT_SOUNDS_ENUM_VEC = { sound_effect::TendrilHit1,
                                                                    sound_effect::TendrilHit2,
                                                                    sound_effect::TendrilHit3,
                                                                    sound_effect::TendrilHit4 };
        tendrilHitSoundsSPtr_.reset(new StaticSounds(TENDRILS_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t WHIP_HIT_SOUNDS_ENUM_VEC = { sound_effect::WhipHit };
        whipHitSoundsSPtr_.reset(new StaticSounds(WHIP_HIT_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t WHIP_MISS_SOUNDS_ENUM_VEC = { sound_effect::WhipMiss1,
                                                                 sound_effect::WhipMiss2,
                                                                 sound_effect::WhipMiss3,
                                                                 sound_effect::WhipMiss4,
                                                                 sound_effect::WhipMiss5,
                                                                 sound_effect::WhipMiss6,
                                                                 sound_effect::WhipMiss7,
                                                                 sound_effect::WhipMiss8,
                                                                 sound_effect::WhipMiss9,
                                                                 sound_effect::WhipMiss10 };
        whipMissSoundsSPtr_.reset(new StaticSounds(WHIP_MISS_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t MATERIAL_HIT_METAL_SOUNDS_ENUM_VEC = { sound_effect::MaterialHitMetal1,
                                                                          sound_effect::MaterialHitMetal2,
                                                                          sound_effect::MaterialHitMetal3,
                                                                          sound_effect::MaterialHitMetal4,
                                                                          sound_effect::MaterialHitMetal5 };
        materialHitMetalSoundsSPtr_.reset(new StaticSounds(MATERIAL_HIT_METAL_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t MATERIAL_HIT_MISC_SOUNDS_ENUM_VEC = { sound_effect::MaterialHitBone,
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
        materialHitMiscSoundsSPtr_.reset(new StaticSounds(MATERIAL_HIT_MISC_SOUNDS_ENUM_VEC));

        const SoundEffectEnumVec_t CLAW_HIT_SOUNDS_ENUM_VEC = { sound_effect::ClawTear };
        clawHitSoundsSPtr_.reset(new StaticSounds(CLAW_HIT_SOUNDS_ENUM_VEC));

        allStaticSoundsSVec_ = { promptSoundsSPtr_,
                                 switchSoundsSPtr_,
                                 tickOnSoundsSPtr_,
                                 tickOffSoundsSPtr_,
                                 thockSoundsSPtr_,
                                 coinSoundsSPtr_,
                                 gemSoundsSPtr_,
                                 meteorShardSoundsSPtr_,
                                 itemGiveSoundsSPtr_,
                                 itemDropSoundsSPtr_,
                                 blowpipeShootSoundsSPtr_,
                                 blowpipeHitSoundsSPtr_,
                                 arrowShootSoundsSPtr_,
                                 arrowHitSoundsSPtr_,
                                 projectileMissSoundsSPtr_,
                                 meleeMissSoundsSPtr_,
                                 fistHitSoundsSPtr_,
                                 breathHitSoundsSPtr_,
                                 tendrilHitSoundsSPtr_,
                                 whipHitSoundsSPtr_,
                                 whipMissSoundsSPtr_,
                                 materialHitMetalSoundsSPtr_,
                                 materialHitMiscSoundsSPtr_,
                                 clawHitSoundsSPtr_ };
    }


    const MusicSetSPtr_t SoundManager::MusicStart(const music::Enum WHICH,
                                                  const float       FADE_MULT,
                                                  const float       VOLUME)
    {
        return MusicStart(MusicEnumVec_t(1, WHICH), false, false, FADE_MULT, VOLUME, music::IsLooped(WHICH));
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

        MusicSetSPtr_t musicSetSPtr(new MusicSet(WHICH_VEC, WILL_RANDOMIZE, WILL_START_AT_RANDOM, FADE_MULT, VOLUME_TO_USE, WILL_LOOP));

        //After construction, the musicSetSPtr currently playing member is already set to what should play first.
        const music::Enum WHICH_MUSIC_TO_START_PLAYING(musicSetSPtr->CurrentlyPlaying());

        //if there is only one song in the list, and it is already playing, then simply set the volume to whatever is specified
        if (WHICH_VEC.size() == 1)
        {
            for (auto & nextSongsObj : songsVec_)
            {
                if ((nextSongsObj.set_sptr.get() != nullptr) && (nextSongsObj.set_sptr->CurrentlyPlaying() == WHICH_MUSIC_TO_START_PLAYING))
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
            if (nextSongsObj.operator_sptr.get() != nullptr)
                if ((nextSongsObj.operator_sptr->Info().Which() == WHICH) || (WHICH == music::All))
                    nextSongsObj.operator_sptr->VolumeFadeOut(FADE_MULT, true);
    }


    void SoundManager::MusicStop(const MusicSetSPtr_t & MUSIC_SET_SPTR, const float FADE_MULT)
    {
        for (auto & nextSongsObj : songsVec_)
            if (nextSongsObj.set_sptr == MUSIC_SET_SPTR)
                if (nextSongsObj.operator_sptr.get() != nullptr)
                    nextSongsObj.operator_sptr->VolumeFadeOut(FADE_MULT, true);
    }


    MusicOperatorSPtr_t SoundManager::MusicOperator(const music::Enum WHICH)
    {
        for (auto const & NEXT_SONGS_OBJ : songsVec_)
            if (NEXT_SONGS_OBJ.operator_sptr.get() != nullptr)
                if (NEXT_SONGS_OBJ.operator_sptr->Info().Which() == WHICH)
                    return NEXT_SONGS_OBJ.operator_sptr;

        return MusicOperatorSPtr_t();
    }


    MusicOperatorSLst_t SoundManager::MusicOperators()
    {
        MusicOperatorSLst_t musicOperatorsList;

        for (auto const & NEXT_SONGS_OBJ : songsVec_)
            if (NEXT_SONGS_OBJ.operator_sptr.get() != nullptr)
                musicOperatorsList.push_back(NEXT_SONGS_OBJ.operator_sptr);

        return musicOperatorsList;
    }


    void SoundManager::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        SongsVec_t songsToRemove;

        for (auto & nextSongsObj : songsVec_)
        {
            if (nextSongsObj.operator_sptr.get() != nullptr)
            {
                const music_update_status::Enum UPDATE_STATUS(nextSongsObj.operator_sptr->UpdateTime(ELAPSED_TIME_SECONDS));

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
                        const music::Enum WHICH_MUSIC_TO_PLAY_NEXT(nextSongsObj.set_sptr->Advance());

                        MusicSPtr_t musicSPtr;
                        const MusicInfo MUSIC_INFO(MusicInfoAcquire(WHICH_MUSIC_TO_PLAY_NEXT, musicSPtr));

                        //use the global volume if the invalid volume is specified
                        const float VOLUME_TO_USE((nextSongsObj.set_sptr->Volume() < 0.0f) ? musicVolume_ : nextSongsObj.set_sptr->Volume());

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
            songsVec_.erase(std::remove(songsVec_.begin(), songsVec_.end(), NEXT_SONGS_OBJ_TO_REMOVE_SPTR), songsVec_.end());
    }


    void SoundManager::MusicVolumeSet(const float NEW_VOLUME)
    {
        musicVolume_ = NEW_VOLUME;

        for (auto & nextSongsObj : songsVec_)
            if (nextSongsObj.operator_sptr.get() != nullptr)
                nextSongsObj.operator_sptr->Volume(NEW_VOLUME);
    }


    void SoundManager::SoundEffectVolumeSet(const float V)
    {
        effectsVolume_ = V;

        for (auto & nextStaticSoundsSPtr : allStaticSoundsSVec_)
            nextStaticSoundsSPtr->Volume(V);
    }


    SoundSPtr_t SoundManager::SoundEffectAcquire(const sound_effect::Enum E)
    {
        SoundBufferSPtr_t bufferSPtr(soundEffectBufferSVec_.at(E));

        if (bufferSPtr.get() == nullptr)
            LoadSound(sound_effect::Filename(E), sound_effect::Directory(E), bufferSPtr);

        SoundSPtr_t soundSPtr(new sf::Sound(*bufferSPtr));
        soundSPtr->setVolume(SoundEffectVolume());
        return soundSPtr;
    }


    void SoundManager::SoundEffectRelease(const sound_effect::Enum E)
    {
        soundEffectBufferSVec_.at(E).reset();
    }


    void SoundManager::OpenMusic(const std::string & MUSIC_FILE_NAME,
        const std::string & MUSIC_DIR_NAME,
        MusicSPtr_t &       musicSPtr) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(musicDirectoryPath_) / bfs::path(MUSIC_DIR_NAME) / bfs::path(MUSIC_FILE_NAME)));
        sfml_util::OpenMusicSPtr(musicSPtr, PATH_OBJ.string());
    }


    void SoundManager::LoadSound(const std::string & SOUND_FILE_NAME,
        const std::string & SOUND_DIR_NAME,
        SoundBufferSPtr_t & soundBufferSPtr) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(soundsDirectoryPath_) / bfs::path(SOUND_DIR_NAME) / bfs::path(SOUND_FILE_NAME)));
        sfml_util::LoadSoundBufferSPtr(soundBufferSPtr, PATH_OBJ.string());
    }


    void SoundManager::CacheMusicInfo_CombatIntro()
    {
        namespace bfs = boost::filesystem;

        //configure the path
        const bfs::path   DIR_OBJ(bfs::system_complete(bfs::path(musicDirectoryPath_) / bfs::path(music::Directory(music::CombatIntro))));
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::exists(DIR_OBJ), "sfml_util::SoundManager::CacheMusicInfo_CombatIntro()  directory \"" << DIR_OBJ.string() << "\" does not exist.");
        M_ASSERT_OR_LOGANDTHROW_SS(bfs::is_directory(DIR_OBJ), "sfml_util::SoundManager::CacheMusicInfo_CombatIntro()  directory \"" << DIR_OBJ.string() << "\" exists but is not a directory.");

        combatIntroMusicInfoVec_.clear();

        bfs::directory_iterator end_iter; // default construction yields past-the-end
        for (bfs::directory_iterator iter(DIR_OBJ); iter != end_iter; ++iter)
        {
            const bfs::path NEXT_PATH_OBJ(iter->path());

            if (bfs::is_regular_file(NEXT_PATH_OBJ) == false)
                continue;

            const std::string NEXT_FILENAME(NEXT_PATH_OBJ.leaf().string());

            std::vector<std::string> filenamePartsVec;
            appbase::stringhelp::SplitByChar(NEXT_FILENAME, filenamePartsVec, '_', true, true);
            if (filenamePartsVec.size() != 4)
            {
                M_HP_LOG("sfml_util::SoundManager::CacheMusicInfo_CombatIntro() failed to split filename \"" << NEXT_FILENAME << "\" by '_' into the required 4 parts.  Probably an incorrectly named combat intro music file.");
                continue;
            }

            const std::string NEXT_ARTIST_NAME(filenamePartsVec.at(1));
            const std::string NEXT_TRACK_NAME(filenamePartsVec.at(2));
            const std::string NEXT_LICENSE_NAME(boost::algorithm::erase_all_copy(filenamePartsVec.at(3), music::FileExt(music::CombatIntro)));

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
        const std::size_t INDEX(sfml_util::rand::Int(0, combatIntroMusicInfoVec_.size() - 1));
        MusicAcquire_CombatIntro(musicInfo, musicSPtr, INDEX);
    }


    void SoundManager::MusicAcquire_CombatIntro(MusicInfo & musicInfo, MusicSPtr_t & musicSPtr, const std::size_t INDEX)
    {
        OpenMusic(combatIntroMusicInfoVec_[INDEX].Filename(), music::Directory(music::CombatIntro), musicSPtr);
        musicInfo = combatIntroMusicInfoVec_[INDEX];
    }


    const MusicInfo SoundManager::MusicInfoAcquire(const music::Enum WHICH, MusicSPtr_t & musicSPtr)
    {
        MusicInfo musicInfo(WHICH);

        if (WHICH == music::CombatIntro)
            MusicAcquire_CombatIntro(musicInfo, musicSPtr);
        else
            OpenMusic(musicInfo.Filename(), musicInfo.DirName(), musicSPtr);

        return musicInfo;
    }


    void SoundManager::MusicOperatorStart(MusicOperatorSPtr_t & musicOperatorSPtr,
                                          const MusicInfo &     MUSIC_INFO,
                                          const MusicSPtr_t &   MUSIC_SPTR,
                                          const float           FADE_MULT,
                                          const float           VOLUME)
    {
        musicOperatorSPtr.reset(new sfml_util::MusicOperator(MUSIC_INFO, MUSIC_SPTR, FADE_MULT, VOLUME));
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
            heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::SoundManager::Test() Starting Tests...");
        }

        static auto counter{ 0 };
        static auto playOrStop{ false };
        static auto const MUSIC_COUNT_MAX{ 50 };

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
                    playOrStop = !playOrStop;
                    counter = 0;
                    return false;
                }

                if (counter < MUSIC_COUNT_MAX)
                {
                    heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager Music Test #" + ss.str() + " Delay...");
                    ++counter;
                    return false;
                }

                if (playOrStop)
                {
                    MusicStop(NEXT_ENUM);
                    playOrStop = !playOrStop;
                    ++musicIndex;
                    heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager Music Test #" + ss.str());
                    return false;
                }
            }
        }

        //test combat music
        static std::size_t combatMusicIndex{ 0 };
        {
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
                    playOrStop = !playOrStop;
                    counter = 0;
                    return false;
                }

                if (counter < MUSIC_COUNT_MAX)
                {
                    heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager Combat Music Test #" + ss.str() + " Delay...");
                    ++counter;
                    return false;
                }

                if (playOrStop)
                {
                    musicSPtr->stop();
                    playOrStop = !playOrStop;
                    ++combatMusicIndex;
                    musicSPtr.reset();
                    heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager Music Test #" + ss.str());
                    return false;
                }
            }
        }

        //test sound effects
        {
            static auto hasSFXPromptedStart{ false };
            if (false == hasSFXPromptedStart)
            {
                heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager SFX Tests starting...");
                hasSFXPromptedStart = true;
            }

            static auto sfxIndex{ 0 };
            if (sfxIndex < sound_effect::Count)
            {
                auto const ENUM{ static_cast<sound_effect::Enum>(sfxIndex) };
                auto const ENUM_STR{ sound_effect::ToString(ENUM) };
                heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager SFX Test \"" + ENUM_STR + "\"");

                SoundSPtr_t soundSPtr{ SoundEffectAcquire(ENUM) };
                M_ASSERT_OR_LOGANDTHROW_SS((soundSPtr.get() != nullptr), "sfml_util::SoundManager::Test()  SFX Testing block found SoundEffectAcquire(\"" << ENUM_STR << "\") returned a nullptr.");

                soundSPtr->setVolume(100.0f);
                soundSPtr->play();
                soundSPtr->play();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                soundSPtr->stop();

                ++sfxIndex;
                return false;
            }

            static auto hasSFXPromptedEnd{ false };
            if (false == hasSFXPromptedEnd)
            {
                heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager SFX Tests finished.  All Passed.");
                hasSFXPromptedEnd = true;
            }
        }

        //test sound effects through static sounds interface
        {
            static auto hasStaticSFXPromptedStart{ false };
            if (false == hasStaticSFXPromptedStart)
            {
                heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager StaticSet SFX Tests starting...");
                hasStaticSFXPromptedStart = true;
            }

            static std::size_t staticSfxIndex{ 0 };
            static std::size_t staticSfxInnerIndex{ 0 };
            if (staticSfxIndex < allStaticSoundsSVec_.size())
            {
                if (0 == staticSfxInnerIndex)
                {
                    allStaticSoundsSVec_.at(staticSfxIndex)->LoadAll();
                }

                if (false == TestStaticSounds(allStaticSoundsSVec_.at(staticSfxIndex), staticSfxInnerIndex))
                {
                    ++staticSfxInnerIndex;
                    return false;
                }
                else
                {
                    allStaticSoundsSVec_.at(staticSfxIndex)->ReleaseAll();

                    std::ostringstream ss;
                    ss << staticSfxIndex;
                    heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager StaticSet SFX Tested Set #" + ss.str());

                    staticSfxInnerIndex = 0;
                    ++staticSfxIndex;
                }

                return false;
            }

            static auto hasStaticSFXPromptedEnd{ false };
            if (false == hasStaticSFXPromptedEnd)
            {
                heroespath::LoopManager::Instance()->TestingStrIncrement("SoundManager StaticSet SFX Tests finished.  All Passed.");
                hasStaticSFXPromptedEnd = true;
            }
        }

        heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::SoundManager::Test() ALL TESTS PASSED");
        return true;
    }


    bool SoundManager::TestStaticSounds(StaticSoundsSPtr_t & staticSoundsSPtr, const std::size_t INDEX)
    {
        if (INDEX < staticSoundsSPtr->Size())
        {
            staticSoundsSPtr->PlayAt(INDEX);
            return false;
        }
        else
        {
            return true;
        }
    }

}
