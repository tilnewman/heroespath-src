#ifndef SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
//
// spell-image-manager.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include "heroespath/spell/spell-enum.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <memory>


namespace sfml_util
{
namespace gui
{

    //types required by the singleton implementation
    class SpellImageManager;
    using SpellImageManagerSPtr_t = std::shared_ptr<SpellImageManager>;


    //Loads images and delivers sfml_util::TextureSPtr_ts to them on demand.
    class SpellImageManager
    {
        //prevent copy construction
        SpellImageManager(const SpellImageManager &);

        //prevent copy assignment
        SpellImageManager & operator=(const SpellImageManager &);

        //prevent non-singleton construction
        SpellImageManager();

    public:
        virtual ~SpellImageManager();

        static SpellImageManagerSPtr_t Instance();
        inline static void SetImagesDirectory(const std::string & S)    { spellImagesDirectory_ = S; }
        inline static float Dimmension()                                { return 256.0f; }
        static bool Test();

        sfml_util::TextureSPtr_t Get(const heroespath::spell::Spells::Enum) const;

    private:
        const std::string MakeFilename(const heroespath::spell::Spells::Enum) const;
        const boost::filesystem::path MakeFilepath(const heroespath::spell::Spells::Enum) const;
    private:
        static SpellImageManagerSPtr_t instance_;
        static std::string spellImagesDirectory_;
        static const std::string filenameExtension_;
    };
}
}

#endif //SFMLUTIL_GUI_SPELLIMAGEMANAGER_HPP_INCLUDED
