//
// combat-image-manager.hpp
//
#include "combat-image-manager.hpp"

#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

#include "heroespath/loop-manager.hpp"
#include "heroespath/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>


namespace sfml_util
{
namespace gui
{

    CombatImageManagerSPtr_t CombatImageManager::instance_           (nullptr);
    std::string              CombatImageManager::imagesDirectoryPath_("");


    CombatImageManager::CombatImageManager()
    {}


    CombatImageManager::~CombatImageManager()
    {}


    CombatImageManagerSPtr_t CombatImageManager::Instance()
    {
        if (instance_.get() == nullptr)
            instance_.reset( new CombatImageManager );

        return instance_;
    }


    bool CombatImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CombatImageManager::Test()  Starting Tests...");
        }

        static auto willFlip{ true };
        static auto imageIndex{ 0 };
        if (imageIndex < CombatImageType::Count)
        {
            auto const ENUM{ static_cast<CombatImageType::Enum>(imageIndex) };
            auto const FILENAME{ CombatImageType::Filename(ENUM) };
            auto textureSPtr{ Get(ENUM) };
            M_ASSERT_OR_LOGANDTHROW_SS((textureSPtr.get() != nullptr), "sfml_util::gui::CombatImageManager::Test()  Get(\"" << FILENAME << "\") returned a nullptr.");

            willFlip = ! willFlip;

            if (willFlip)
            {
                sfml_util::FlipHoriz( * textureSPtr);
                heroespath::LoopManager::Instance()->TestingImageSet(textureSPtr);
                heroespath::LoopManager::Instance()->TestingStrAppend("CombatImageManager Tested " + FILENAME);
            }
            else
            {
                heroespath::LoopManager::Instance()->TestingImageSet(textureSPtr);
                heroespath::LoopManager::Instance()->TestingStrAppend("CombatImageManager Tested " + FILENAME);
                return false;
            }

            ++imageIndex;
            return false;
        }

        heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::CombatImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    TextureSPtr_t CombatImageManager::Get(const CombatImageType::Enum E, const bool WILL_FLIP_HORIZ) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(CombatImageType::Filename(E))));

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, PATH_OBJ.string());

        if (WILL_FLIP_HORIZ)
            sfml_util::FlipHoriz( * textureSPtr);

        return textureSPtr;
    }


}
}
