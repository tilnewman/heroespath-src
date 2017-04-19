#ifndef SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
//
// combat-image-manager.hpp
//
#include <sfml-util/gui/combat-image-enum.hpp>

#include <string>
#include <memory>


namespace sf
{
    class Texture;
}
namespace sfml_util
{
    using TextureSPtr_t = std::shared_ptr<sf::Texture>;

namespace gui
{

    //types required by the singleton implementation
    class CombatImageManager;
    using CombatImageManagerSPtr_t = std::shared_ptr<CombatImageManager>;


    //singleton responsible for loading and dispensing combat image TextureSPtr_t
    class CombatImageManager
    {
        //prevent copy assignment
        CombatImageManager & operator=(const CombatImageManager &);

        //prevent copy construction
        CombatImageManager(const CombatImageManager &);

        //prevent non-singleton construction
        CombatImageManager();

    public:
        virtual ~CombatImageManager();

        static CombatImageManagerSPtr_t Instance();

        inline static void SetImagesDirectory(const std::string & S) { imagesDirectoryPath_ = S; }

        bool Test();

        TextureSPtr_t Get(const CombatImageType::Enum, const bool WILL_FLIP_HORIZ = false) const;

    private:
        static CombatImageManagerSPtr_t instance_;
        static std::string imagesDirectoryPath_;
    };

}
}

#endif //SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
