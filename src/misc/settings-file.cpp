// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// settings-file.cpp
//
#include "settings-file.hpp"

#include "gui/display.hpp"
#include "gui/sound-manager.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/video-mode.hpp"

namespace heroespath
{
namespace misc
{

    const std::string SettingsFile::KEY_MUSIC_VOLUME_("volume-music");
    const std::string SettingsFile::KEY_SOUND_FX_VOLUME_("volume-sound_fx");
    const std::string SettingsFile::KEY_DISPLAY_WIDTH_("display-width");
    const std::string SettingsFile::KEY_DISPLAY_HEIGHT_("display-height");
    const std::string SettingsFile::KEY_DISPLAY_BIT_DEPTH_("display-bit_depth");
    const std::string SettingsFile::KEY_DISPLAY_WILL_VERTICAL_SYNC_("display-will_vertical_sync");
    const std::string SettingsFile::KEY_DISPLAY_FRAMERATE_LIMIT_("display-frame_rate_limit");
    const std::string SettingsFile::KEY_DISPLAY_ANTIALIAS_LEVEL_("display-antialias_level");

    std::unique_ptr<SettingsFile> SettingsFile::instanceUPtr_;

    SettingsFile::SettingsFile()
        : KeyValueFile("settings.txt")
    {
        M_HP_LOG_DBG("Subsystem Construction: SettingsFile");
    }

    SettingsFile::~SettingsFile() { M_HP_LOG_DBG("Subsystem Destruction: SettingsFile"); }

    misc::NotNull<SettingsFile *> SettingsFile::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: SettingsFile");
            Acquire();
        }

        return misc::NotNull<SettingsFile *>(instanceUPtr_.get());
    }

    void SettingsFile::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<SettingsFile>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: SettingsFile");
        }
    }

    void SettingsFile::Release() { instanceUPtr_.reset(); }

    void SettingsFile::AcquireAndSave()
    {
        SetOrAppendAs(KEY_MUSIC_VOLUME_, gui::SoundManager::Instance()->MusicVolume());

        SetOrAppendAs(KEY_SOUND_FX_VOLUME_, gui::SoundManager::Instance()->SoundEffectVolume());

        SetOrAppendAs(
            KEY_DISPLAY_WIDTH_, static_cast<int>(gui::Display::Instance()->GetWinWidthu()));

        SetOrAppendAs(
            KEY_DISPLAY_HEIGHT_, static_cast<int>(gui::Display::Instance()->GetWinHeightu()));

        const auto BIT_DEPTH { gui::Display::Instance()->WinColorDepth() };

        // for some reason SFML reports 32 bit depth as 0 sometimes...
        if (BIT_DEPTH == 0)
        {
            SetOrAppendAs(KEY_DISPLAY_BIT_DEPTH_, 32);
        }
        else
        {
            SetOrAppendAs(KEY_DISPLAY_BIT_DEPTH_, BIT_DEPTH);
        }

        SetOrAppendAs(KEY_DISPLAY_WILL_VERTICAL_SYNC_, gui::Display::Instance()->GetVerticalSync());

        SetOrAppendAs(
            KEY_DISPLAY_FRAMERATE_LIMIT_,
            static_cast<int>(gui::Display::Instance()->GetFrameRateLimit()));

        SetOrAppendAs(
            KEY_DISPLAY_ANTIALIAS_LEVEL_,
            static_cast<int>(gui::Display::Instance()->AntialiasLevel()));

        if (DeleteReCreateSave() == false)
        {
            M_HP_LOG_ERR(
                "KeyValueFile::DeleteReCreateSave() returned false.  So...something went wrong "
                "with the open or write?"
                + M_HP_VAR_STR(Path()));
        }
    }

    void SettingsFile::LoadAndRestore()
    {
        if (Exists() == false)
        {
            M_HP_LOG(
                "Settings file does not exist.  No settings will be restored from previous run.  "
                "Defaults will be set."
                + M_HP_VAR_STR(Path()));

            SetDefaults();
            return;
        }

        if (ClearAndLoad() == false)
        {
            M_HP_LOG_ERR(
                "Settings file exists but KeyValueFile::ClearAndLoad() returned false, so "
                "something went wrong reading the file...  Defaults will be set."
                + M_HP_VAR_STR(Path()));

            SetDefaults();
            return;
        }

        const auto MUSIC_VOLUME { ValueAs(KEY_MUSIC_VOLUME_, -1.0f) };
        if (!(MUSIC_VOLUME < 0.0f))
        {
            M_HP_LOG("Settings file restoring previous music volume." + M_HP_VAR_STR(MUSIC_VOLUME));
            gui::SoundManager::Instance()->MusicVolumeSet(MUSIC_VOLUME);
        }

        const float SOUND_FX_VOLUME { ValueAs(KEY_SOUND_FX_VOLUME_, -1.0f) };
        if (!(SOUND_FX_VOLUME < 0.0f))
        {
            M_HP_LOG(
                "Settings file restoring previous sound fx volume."
                + M_HP_VAR_STR(SOUND_FX_VOLUME));

            gui::SoundManager::Instance()->SoundEffectVolumeSet(SOUND_FX_VOLUME);
        }

        const unsigned DIMMENSION_TOO_BIG { 10000 };
        const unsigned OTHER_TOO_BIG { 33 };

        const auto WIDTH { ValueAs<unsigned>(KEY_DISPLAY_WIDTH_, DIMMENSION_TOO_BIG) };
        const auto HEIGHT { ValueAs<unsigned>(KEY_DISPLAY_HEIGHT_, DIMMENSION_TOO_BIG) };
        const auto BIT_DEPTH { ValueAs<unsigned>(KEY_DISPLAY_BIT_DEPTH_, OTHER_TOO_BIG) };

        const auto ANTIALIAS_LEVEL { ValueAs<unsigned>(
            KEY_DISPLAY_ANTIALIAS_LEVEL_, OTHER_TOO_BIG) };

        const auto ARE_VALUES_INVALID { (WIDTH >= DIMMENSION_TOO_BIG)
                                        || (HEIGHT >= DIMMENSION_TOO_BIG)
                                        || (BIT_DEPTH >= OTHER_TOO_BIG)
                                        || (ANTIALIAS_LEVEL >= OTHER_TOO_BIG) };

        if (ARE_VALUES_INVALID)
        {
            M_HP_LOG_ERR(
                "Settings file display options were invalid, and will not be restored."
                + M_HP_VAR_STR(WIDTH) + M_HP_VAR_STR(HEIGHT) + M_HP_VAR_STR(BIT_DEPTH)
                + M_HP_VAR_STR(ANTIALIAS_LEVEL));
        }
        else
        {
            const auto ARE_VALUES_DIFFERENT_FROM_CURRENT {
                (gui::Display::Instance()->GetWinWidthu() != WIDTH)
                || (gui::Display::Instance()->GetWinHeightu() != HEIGHT)
                || (gui::Display::Instance()->GetCurrentVideoMode().bitsPerPixel != BIT_DEPTH)
                || (gui::Display::Instance()->AntialiasLevel() != ANTIALIAS_LEVEL)
            };

            if (ARE_VALUES_DIFFERENT_FROM_CURRENT)
            {
                const sf::VideoMode VIDEO_MODE(WIDTH, HEIGHT, BIT_DEPTH);

                M_HP_LOG(
                    "Settings file attempting to restore previous video mode..."
                    + M_HP_VAR_STR(VIDEO_MODE) + M_HP_VAR_STR(ANTIALIAS_LEVEL));

                gui::Display::Instance()->ChangeVideoMode(VIDEO_MODE, ANTIALIAS_LEVEL);
            }
            else
            {
                M_HP_LOG("Settings file display options were identical to what has already "
                         "been established, and will be ignored.");
            }
        }

        const unsigned FRAME_RATE_LIMIT_TOO_BIG { 1000 };

        const auto FRAME_RATE_LIMIT(
            ValueAs(KEY_DISPLAY_FRAMERATE_LIMIT_, FRAME_RATE_LIMIT_TOO_BIG));

        if (FRAME_RATE_LIMIT < FRAME_RATE_LIMIT_TOO_BIG)
        {
            M_HP_LOG(
                "Settings file restoring previous frame rate limit of "
                << FRAME_RATE_LIMIT << ". (zero disables)" + M_HP_VAR_STR(FRAME_RATE_LIMIT));

            gui::Display::Instance()->SetFrameRateLimit(FRAME_RATE_LIMIT);
        }

        if (ContainsKey(KEY_DISPLAY_WILL_VERTICAL_SYNC_))
        {
            const bool WILL_VERTICAL_SYNC { ValueOrDefault<bool>(KEY_DISPLAY_WILL_VERTICAL_SYNC_) };

            M_HP_LOG(
                "Settings file restoring display vertical sync of "
                << WILL_VERTICAL_SYNC << ". (false disables)" + M_HP_VAR_STR(WILL_VERTICAL_SYNC));

            gui::Display::Instance()->SetVerticalSync(WILL_VERTICAL_SYNC);
        }
    }

    void SettingsFile::SetDefaults()
    {
        const auto MUSIC_VOLUME_PERCENT { 33.3f };
        const auto SOUND_FX_VOLUME_PERCENT { 50.0f };

        M_HP_LOG(
            "Settings file ended up skipping the file's contents and setting the following "
            "defaults:"
            + M_HP_VAR_STR(MUSIC_VOLUME_PERCENT) + M_HP_VAR_STR(SOUND_FX_VOLUME_PERCENT));

        gui::SoundManager::Instance()->MusicVolumeSet(MUSIC_VOLUME_PERCENT);
        gui::SoundManager::Instance()->SoundEffectVolumeSet(SOUND_FX_VOLUME_PERCENT);
    }

} // namespace misc
} // namespace heroespath
