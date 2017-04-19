//
// title-image-manager.cpp
//
#include "title-image-manager.hpp"

#include "sfml-util/loaders.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/creature/title.hpp"

#include <boost/filesystem.hpp>
#include <sstream>
#include <exception>


namespace sfml_util
{
namespace gui
{

    std::string             TitleImageManager::imagesDirectoryPath_;
    TitleImageManagerSPtr_t TitleImageManager::instanceSPtr_;


    TitleImageManager::TitleImageManager()
    :
        placeHlderFilePathObj_( boost::filesystem::system_complete(boost::filesystem::path(imagesDirectoryPath_) / PlaceholderFileName()) )
    {
        if (boost::filesystem::exists(placeHlderFilePathObj_) == false)
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::TitleImageManager::Constructor() could not locate the placeholder image at: \"" << placeHlderFilePathObj_.string() << "\".";
            throw std::runtime_error(ss.str());
        }
    }


    TitleImageManager::~TitleImageManager()
    {}


    TitleImageManagerSPtr_t TitleImageManager::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new TitleImageManager );

        return instanceSPtr_;
    }


    void TitleImageManager::SetTitleImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    const TextureSPtr_t TitleImageManager::Get(heroespath::creature::TitlePtr_t TITLE_PTR) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ( bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(TITLE_PTR->ImageFilename())) );
        bfs::path pathToUse(PATH_OBJ);

        if (bfs::exists(pathToUse) == false)
        {
            M_HP_LOG("sfml_util::gui::TitleImageManager::Get(title=\"" << TITLE_PTR->Name() << "\", title_filename=\"" << TITLE_PTR->ImageFilename() << "\") was unable to find the image for a title at: \"" << pathToUse.string() << "\".");
            pathToUse = placeHlderFilePathObj_;
        }

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, pathToUse.string());
        return textureSPtr;
    }

}
}
