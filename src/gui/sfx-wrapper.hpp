// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SFXWRAPPER_HPP_INCLUDED
#define HEROESPATH_GUI_SFXWRAPPER_HPP_INCLUDED
//
// sfx-wrapper.hpp
//
#include "gui/sfml-audio.hpp"
#include "gui/sound-effects-enum.hpp"

#include <memory>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for storing all sfml objects relating to a single sound effect,
    // and presenting an interface for controlling it.
    class SfxWrapper
    {
    public:
        explicit SfxWrapper(
            const sound_effect::Enum ENUM = sound_effect::Count,
            SoundUPtr_t soundUPtr = SoundUPtr_t(),
            SoundBufferUPtr_t BUFFER_UPTR = SoundBufferUPtr_t(),
            const bool WILL_LOOP = false,
            const float VOLUME_RATIO = 1.0f);

        ~SfxWrapper();

        // the unique_ptr members will implicitly delete copy operations
        // default move operations will suffice
        SfxWrapper(SfxWrapper &&) = default;
        SfxWrapper & operator=(SfxWrapper &&) = default;

        bool IsValid() const;

        void Reset();

        sound_effect::Enum Which() const { return which_; }

        void Play()
        {
            if (IsValid())
            {
                soundUPtr_->play();
            }
        }

        void Stop()
        {
            if (IsValid())
            {
                soundUPtr_->stop();
            }
        }

        float Volume() const { return ((IsValid()) ? soundUPtr_->getVolume() : 0.0f); }

        void Volume(const float V)
        {
            if (IsValid())
            {
                soundUPtr_->setVolume(
                    ((V < 0.0f) ? 0.0f : ((V > 100.0f) ? 100.0f : V * volumeRatio_)));
            }
        }

        float VolumeRatio() const { return volumeRatio_; }

        void VolumeRatio(const float NEW_VOLUME_RATIO)
        {
            volumeRatio_ = NEW_VOLUME_RATIO;
            Volume(Volume());
        }

        bool WillLoop() const
        {
            if (IsValid())
            {
                return soundUPtr_->getLoop();
            }
            else
            {
                return false;
            }
        }

        void WillLoop(const bool NEW_WILL_LOOP)
        {
            if (IsValid())
            {
                soundUPtr_->setLoop(NEW_WILL_LOOP);
            }
        }

        sf::SoundSource::Status Status() const
        {
            return ((IsValid()) ? soundUPtr_->getStatus() : sf::SoundSource::Stopped);
        }

        friend bool operator==(const SfxWrapper & L, const SfxWrapper & R);
        friend bool operator<(const SfxWrapper & L, const SfxWrapper & R);

    private:
        sound_effect::Enum which_;
        SoundUPtr_t soundUPtr_;
        SoundBufferUPtr_t bufferUPtr_;
        float volumeRatio_;
    };

    inline bool operator==(const SfxWrapper & L, const SfxWrapper & R)
    {
        // intentionally omit volumeRatio_
        return std::tie(L.which_, L.soundUPtr_, L.bufferUPtr_)
            == std::tie(R.which_, R.soundUPtr_, R.bufferUPtr_);
    }

    inline bool operator!=(const SfxWrapper & L, const SfxWrapper & R) { return !(L == R); }

    inline bool operator<(const SfxWrapper & L, const SfxWrapper & R)
    {
        // intentionally omit volumeRatio_
        return std::tie(L.which_, L.soundUPtr_, L.bufferUPtr_)
            < std::tie(R.which_, R.soundUPtr_, R.bufferUPtr_);
    }

    using SfxWrapperVec_t = std::vector<SfxWrapper>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SFXWRAPPER_HPP_INCLUDED
