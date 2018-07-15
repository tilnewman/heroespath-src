// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfx-wrapper.cpp
//
#include "sfx-wrapper.hpp"

#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace sfml_util
{

    SfxWrapper::SfxWrapper(
        const sound_effect::Enum ENUM,
        SoundUPtr_t soundUPtr,
        SoundBufferUPtr_t BUFFER_UPTR,
        const bool WILL_LOOP,
        const float VOLUME_RATIO)
        : which_(ENUM)
        , soundUPtr_(std::move(soundUPtr))
        , bufferUPtr_(std::move(BUFFER_UPTR))
        , volumeRatio_(VOLUME_RATIO)
    {
        if (IsValid())
        {
            soundUPtr_->setBuffer(*bufferUPtr_);
            soundUPtr_->setLoop(WILL_LOOP);
        }
    }

    SfxWrapper::~SfxWrapper()
    {
        if (soundUPtr_)
        {
            soundUPtr_->stop();
        }
    }

    bool SfxWrapper::IsValid() const
    {
        return (
            (which_ != sound_effect::Count) && (which_ != sound_effect::None) && (soundUPtr_)
            && (bufferUPtr_));
    }

    void SfxWrapper::Reset()
    {
        if (soundUPtr_)
        {
            soundUPtr_->stop();
        }

        which_ = sound_effect::Count;
        soundUPtr_.reset();
        bufferUPtr_.reset();
    }

} // namespace sfml_util
} // namespace heroespath
