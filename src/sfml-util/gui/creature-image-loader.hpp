// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_CREATURE_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_CREATURE_IMAGE_LOADER_HPP_INCLUDED
//
// creature-image-loader.hpp
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/image-util.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace sf
{
class Texture;
}

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace sfml_util
{
    namespace gui
    {

        // Responsible for loading Creature images.
        class CreatureImageLoader
        {
        public:
            CreatureImageLoader(const CreatureImageLoader &) = delete;
            CreatureImageLoader(CreatureImageLoader &&) = delete;
            CreatureImageLoader & operator=(const CreatureImageLoader &) = delete;
            CreatureImageLoader & operator=(CreatureImageLoader &&) = delete;

            CreatureImageLoader();

            bool Test() const;

            static float MaxDimmension() { return StandardImageDimmension(); }

            const std::string Path(const creature::CreaturePtr_t) const;
            const std::string Path(const std::string & FILENAME) const;

            void Load(sf::Texture & texture, const creature::CreaturePtr_t) const;

            void Load(
                sf::Texture & texture,
                const std::string & IMAGE_FILE_NAME,
                const bool WILL_HORIZ_FLIP_TO_FACE_RIGHT = false) const;

            const std::string FilenameRandom(const creature::CreaturePtr_t) const;

            const std::vector<std::string> Filenames(
                const creature::race::Enum RACE,
                const creature::role::Enum ROLE,
                const creature::sex::Enum SEX,
                const creature::wolfen_class::Enum WOLFEN_CLASS = creature::wolfen_class::Pup,
                const creature::dragon_class::Enum DRAGON_CLASS
                = creature::dragon_class::Hatchling) const;

            void EnsureFileExists(const std::string & FILENAME) const;

            bool WillHorizFlipToFaceRight(const creature::CreaturePtr_t CREATURE_PTR) const;

        private:
            std::string imageDirectoryPath_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_CREATURE_IMAGE_LOADER_HPP_INCLUDED
