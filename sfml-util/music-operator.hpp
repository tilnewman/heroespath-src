#ifndef APPBASE_SFMLUTIL_MUSIC_OPERATOR_INCLUDED
#define APPBASE_SFMLUTIL_MUSIC_OPERATOR_INCLUDED
//
// music-operator.hpp
//  Code that performs all needed operations on a piece of music.
//
#include "sfml-util/sfml-audio.hpp"
#include "sfml-util/music-info.hpp"

#include <memory>
#include <string>
#include <list>


//forward declaration
namespace sf
{
    class Music;
}

namespace sfml_util
{

    using MusicSPtr_t = std::shared_ptr<sf::Music>;


    struct music_update_status
    {
        enum Enum
        {
            Stopped = 0,
            Playing,
            FadingOut,
            FadedOut,
            FadedOutKill,
            FadingIn,
            FadedIn,
            Count
        };

        static const std::string ToString(const music_update_status::Enum);
    };


    //responsible ofr controlling a piece of music
    class MusicOperator
    {
    public:
        MusicOperator(const MusicInfo &              MUSIC_INFO,
                      const sfml_util::MusicSPtr_t & MUSIC_SPTR,
                      const float                    FADE_IN_MULT  = FADE_MULT_IMMEDIATE_,
                      const float                    TARGET_VOLUME = VOLUME_USE_GLOBAL_);

        virtual ~MusicOperator();

        inline const MusicInfo Info() const         { return info_; }

        inline void Play()                          { musicSPtr_->play(); }
        inline void Pause()                         { musicSPtr_->pause(); }
        inline void Stop()                          { musicSPtr_->stop(); }
        inline sf::Music::Status Status() const     { return musicSPtr_->getStatus(); }

        inline bool IsLooped() const                { return musicSPtr_->getLoop(); }
        inline void IsLooped(const bool B)          { musicSPtr_->setLoop(B); }

        inline bool IsFadingIn() const              { return (fadeInMult_ > 0.0f); }
        inline bool IsFadingOut() const             { return (fadeOutMult_ > 0.0f); }

        inline float FadingInMult() const           { return fadeInMult_; }
        inline void FadingInMult(const float F)     { fadeInMult_ = F; }

        inline float FadingOutMult() const          { return fadeOutMult_; }
        inline void FadingOutMult(const float F)    { fadeOutMult_ = F; }

        inline bool KillAfterFadeOut() const        { return killAfterFadeOut_; }
        inline void KillAfterFadeOut(const bool B)  { killAfterFadeOut_ = B; }

        inline float Volume() const                 { return musicSPtr_->getVolume(); }
        inline void Volume(const float F)           { musicSPtr_->setVolume(F); }
        inline float VolumeTarget() const           { return targetVolume_; }
        inline void VolumeTarget(const float F)     { targetVolume_ = F; }

        void VolumeFadeTo(const float TARGET_VOL, const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeToGlobal(const float FADE_MULT = FADE_MULT_IMMEDIATE_);
        void VolumeFadeOut(const float FADE_MULT = FADE_MULT_DEFAULT_OUT_, const bool WILL_KILL_AFTER = true);

        //returns true if this MusicOperator object should be removed from the list and destroyed
        music_update_status::Enum UpdateTime(const float ELAPSED_TIME_SECONDS);

        friend bool operator==(const MusicOperator & L, const MusicOperator & R);

    public:
        static const float VOLUME_USE_GLOBAL_;
        static const float FADE_MULT_IMMEDIATE_;
        static const float FADE_MULT_DEFAULT_IN_;
        static const float FADE_MULT_DEFAULT_OUT_;
    private:
        MusicInfo info_;
        float     targetVolume_;
        float     fadeInMult_;
        float     fadeOutMult_;
        bool      killAfterFadeOut_;
        sfml_util::MusicSPtr_t musicSPtr_;
    };

    using MusicOperatorSPtr_t = std::shared_ptr<MusicOperator>;
    using MusicOperatorSLst_t = std::list<MusicOperatorSPtr_t>;

    bool operator==(const MusicOperator & L, const MusicOperator & R);
    inline bool operator!=(const MusicOperator & L, const MusicOperator & R) { return ! (L == R); }

}
#endif //APPBASE_SFMLUTIL_MUSIC_OPERATOR_INCLUDED
