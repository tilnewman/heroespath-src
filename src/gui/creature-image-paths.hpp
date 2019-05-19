// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CREATURE_IMAGE_PATHS_HPP_INCLUDED
#define HEROESPATH_GUI_CREATURE_IMAGE_PATHS_HPP_INCLUDED
//
// creature-image-paths.hpp
//
#include "creature/dragon-class-enum.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "gui/content-images.hpp"
#include "misc/not-null.hpp"

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

namespace gui
{

    class CreatureImagePaths
    {
    public:
        CreatureImagePaths() = delete;

        static void SetupFilesystemPaths();
        static const std::string PathFromFilename(const std::string & FILENAME);
        static const std::string FilenameRandom(const creature::Creature &);

        static const std::vector<std::string> Filenames(
            const creature::race::Enum RACE,
            const creature::role::Enum ROLE,
            const creature::sex::Enum SEX,
            const creature::wolfen_class::Enum WOLFEN_CLASS = creature::wolfen_class::Pup,
            const creature::dragon_class::Enum DRAGON_CLASS = creature::dragon_class::Hatchling);

        static bool WillHorizFlipToFaceRight(const creature::Creature &);

    private:
        static std::string imageDirectoryPath_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CREATURE_IMAGE_PATHS_HPP_INCLUDED
