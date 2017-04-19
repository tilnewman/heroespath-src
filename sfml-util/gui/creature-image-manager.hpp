#ifndef APPBASE_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
#define APPBASE_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
//
// creature-image-manager.hpp
//  Code that manages loading and lifetime of creature images.
//
#include "sfml-util/sfml-graphics.hpp"

#include "heroespath/creature/role-enum.hpp"
#include "heroespath/creature/race-enum.hpp"
#include "heroespath/creature/sex-enum.hpp"
#include "heroespath/creature/wolfen-class-enum.hpp"
#include "heroespath/creature/dragon-class-enum.hpp"

#include <memory>
#include <map>
#include <tuple>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    class CreatureImageManager;
    using CreatureImageManagerSPtr_t = std::shared_ptr<CreatureImageManager>;


    //A class that loads, stores, and distributes creature images.
    class CreatureImageManager
    {
        //prevent copy construction
        CreatureImageManager(const CreatureImageManager &);

        //prevent copy assignment
        CreatureImageManager & operator=(const CreatureImageManager &);

        //prevent non-singleton construction
        CreatureImageManager();

    public:
        virtual ~CreatureImageManager();

        static CreatureImageManagerSPtr_t Instance();
        static void SetCreatureImageDirectory(const std::string & PATH);

        static bool Test();

        inline static float DimmensionMax() { return 256.0f; }

        const std::string GetFilename(const heroespath::creature::race::Enum         RACE,
                                      const heroespath::creature::role::Enum         ROLE,
                                      const heroespath::creature::sex::Enum          SEX,
                                      const bool                                     WILL_PICK_RANDOM = true,
                                      const heroespath::creature::wolfen_class::Enum WOLFEN_CLASS     = heroespath::creature::wolfen_class::Pup,
                                      const heroespath::creature::dragon_class::Enum DRAGON_CLASS     = heroespath::creature::dragon_class::Hatchling) const;

        const TextureSPtr_t GetImage(const heroespath::creature::race::Enum         RACE,
                                     const heroespath::creature::role::Enum         ROLE,
                                     const heroespath::creature::sex::Enum          SEX              = heroespath::creature::sex::Male,
                                     const bool                                     WILL_PICK_RANDOM = true,
                                     const bool                                     WILL_FACE_RIGHT  = false,
                                     const heroespath::creature::wolfen_class::Enum WOLFEN_CLASS     = heroespath::creature::wolfen_class::Pup,
                                     const heroespath::creature::dragon_class::Enum DRAGON_CLASS     = heroespath::creature::dragon_class::Hatchling) const;

        const TextureSPtr_t GetImage(const std::string & FILENAME,
                                     const bool          WILL_FACE_RIGHT = false) const;

        void GetFilenames(std::vector<std::string> &                     outputVec,
                          const heroespath::creature::race::Enum         RACE,
                          const heroespath::creature::role::Enum         ROLE,
                          const heroespath::creature::sex::Enum          SEX,
                          const heroespath::creature::wolfen_class::Enum WOLFEN_CLASS = heroespath::creature::wolfen_class::Pup,
                          const heroespath::creature::dragon_class::Enum DRAGON_CLASS = heroespath::creature::dragon_class::Hatchling) const;

    private:
        //assumes there is no matching entry in textureMap_
        TextureSPtr_t LoadImage(const std::string & IMAGE_FILE_NAME,
                                const bool          WILL_FACE_RIGHT = false) const;

    private:
        static std::string imagesDirectoryPath_;
        static CreatureImageManagerSPtr_t instanceSPtr_;
    };

}
}
#endif //APPBASE_SFMLUTIL_GUI_CREATUREIMAGEMANAGER_INCLUDED
