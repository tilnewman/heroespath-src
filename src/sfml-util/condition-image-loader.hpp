// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CONDITION_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_GUI_CONDITION_IMAGE_LOADER_HPP_INCLUDED
//
// condition-image-loader.hpp
//
#include "creature/condition-enum.hpp"
#include "misc/config-file.hpp"
#include "sfml-util/enum-image-loader.hpp"

namespace heroespath
{
namespace gui
{

    // Responsible for loading condition images.
    class ConditionImageLoader : public EnumImageLoader<creature::Conditions>
    {
    public:
        ConditionImageLoader(const ConditionImageLoader &) = delete;
        ConditionImageLoader(ConditionImageLoader &&) = delete;
        ConditionImageLoader & operator=(const ConditionImageLoader &) = delete;
        ConditionImageLoader & operator=(ConditionImageLoader &&) = delete;

        ConditionImageLoader()
            : EnumImageLoader<creature::Conditions>(
                misc::ConfigFile::Instance()->GetMediaPath("media-images-conditions-dir"))
        {}
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CONDITION_IMAGE_LOADER_HPP_INCLUDED
