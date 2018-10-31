// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
//
// enum-image-loader.hpp
//
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "sfml-util/image-options.hpp"
#include "sfml-util/image-util.hpp"
#include "sfml-util/loaders.hpp"

#include <boost/type_index.hpp>

#include <string>

namespace heroespath
{
namespace sfml_util
{

    namespace EnumImageLoaderHelpers
    {
        void TestLog(const std::string & MESSAGE);
    }

    // Responsible for implementing loading and returned an image based on enum values.
    template <typename EnumWrapper_t>
    class EnumImageLoader
    {
    public:
        EnumImageLoader(const EnumImageLoader &) = delete;
        EnumImageLoader(EnumImageLoader &&) = delete;
        EnumImageLoader & operator=(const EnumImageLoader &) = delete;
        EnumImageLoader & operator=(EnumImageLoader &&) = delete;

        explicit EnumImageLoader(const std::string & IMAGE_DIRECTORY_PATH)
            : imageDirectoryPath_(misc::filesystem::CleanPath(IMAGE_DIRECTORY_PATH))
        {}

        float MaxDimmension() const { return StandardImageDimmension(); }

        void Load(sf::Texture & texture, const typename EnumWrapper_t::Enum ENUM_VALUE) const
        {
            sfml_util::Loaders::Texture(texture, Path(ENUM_VALUE));
        }

        const std::string Path(const typename EnumWrapper_t::Enum ENUM_VALUE) const
        {
            return misc::filesystem::CombinePathsThenClean(
                imageDirectoryPath_, EnumWrapper_t::ImageFilename(ENUM_VALUE));
        }

        bool Test() const
        {
            auto makeLogPrefix { []() {
                return "sfml_util::EnumImageLoader<"
                    + boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "> ";
            } };

            static auto hasInitialPrompt { false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                EnumImageLoaderHelpers::TestLog(makeLogPrefix() + "Starting Tests...");
            }

            static misc::EnumUnderlying_t imageIndex { 0 };
            if (imageIndex < EnumWrapper_t::Count)
            {
                const auto ENUM_VALUE { static_cast<typename EnumWrapper_t::Enum>(imageIndex++) };

                sf::Texture texture;
                Load(texture, ENUM_VALUE);

                const auto MAX_DIMMENSION_U { static_cast<unsigned>(MaxDimmension()) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    ((texture.getSize().x == MAX_DIMMENSION_U)
                     || (texture.getSize().y == MAX_DIMMENSION_U)),
                    makeLogPrefix()
                        << "::Test() The call to Get(enum=" << EnumWrapper_t::ToString(ENUM_VALUE)
                        << ") returned an image that was not of size=" << MAX_DIMMENSION_U << "x"
                        << MAX_DIMMENSION_U << ".  The actual size=" << texture.getSize().x << "x"
                        << texture.getSize().y << ".");

                EnumImageLoaderHelpers::TestLog(Path(ENUM_VALUE));
                return false;
            }

            EnumImageLoaderHelpers::TestLog(makeLogPrefix() + "  All Test Passed");
            return true;
        }

    private:
        std::string imageDirectoryPath_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
