// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SPELL_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SPELL_IMAGE_LOADER_HPP_INCLUDED
//
// spell-image-loader.hpp
//
#include "misc/config-file.hpp"
#include "sfml-util/enum-image-loader.hpp"
#include "spell/spell-enum.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Responsible for loading Spell images.
    class SpellImageLoader : public EnumImageLoader<spell::Spells>
    {
    public:
        SpellImageLoader(const SpellImageLoader &) = delete;
        SpellImageLoader(SpellImageLoader &&) = delete;
        SpellImageLoader & operator=(const SpellImageLoader &) = delete;
        SpellImageLoader & operator=(SpellImageLoader &&) = delete;

        SpellImageLoader()
            : EnumImageLoader<spell::Spells>(
                  misc::ConfigFile::Instance()->GetMediaPath("media-images-spells-dir"))
        {}
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SPELL_IMAGE_LOADER_HPP_INCLUDED
