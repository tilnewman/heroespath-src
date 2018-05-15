// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
//
// enum-image-loader.hpp
//
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "sfml-util/gui/image-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util.hpp"

#include <boost/type_index.hpp>

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for implementing loading and returned an image based on enum values.
        template <typename EnumWrapper_t>
        class EnumImageLoader
        {
        public:
            EnumImageLoader(const EnumImageLoader &) = delete;
            EnumImageLoader(EnumImageLoader &&) = delete;
            EnumImageLoader & operator=(const EnumImageLoader &) = delete;
            EnumImageLoader & operator=(EnumImageLoader &&) = delete;

            EnumImageLoader(const std::string & IMAGE_DIRECTORY_PATH)
                : imageDirectoryPath_("")
            {
                namespace bfs = boost::filesystem;

                auto const PATH{
                    bfs::system_complete(bfs::path(IMAGE_DIRECTORY_PATH)).normalize()
                };

                imageDirectoryPath_ = PATH.string();
            }

            float MaxDimmension() const { return image::StandardDimmension(); }

            void
                Get(sf::Texture & texture,
                    const typename EnumWrapper_t::Enum ENUM_VALUE_VALUE,
                    const image::Flip WILL_FLIP_HORIZ = image::Flip::No) const
            {
                namespace bfs = boost::filesystem;

                auto const PATH{ bfs::path(imageDirectoryPath_)
                                 / bfs::path(EnumWrapper_t::ImageFilename(ENUM_VALUE_VALUE)) };

                sfml_util::LoadTexture(texture, PATH.string());

                if (WILL_FLIP_HORIZ == image::Flip::Yes)
                {
                    sfml_util::FlipHoriz(texture);
                }
            }

            bool Test() const
            {
                auto makeLogPrefix{ []() {
                    return "sfml_util::gui::EnumImageLoader<"
                        + boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "> ";
                } };

                static auto hasInitialPrompt{ false };
                if (false == hasInitialPrompt)
                {
                    hasInitialPrompt = true;

                    game::LoopManager::Instance()->TestingStrAppend(
                        makeLogPrefix() + "Starting Tests...");
                }

                static auto willFlip{ false };
                static misc::EnumUnderlying_t imageIndex{ 0 };
                if (imageIndex < EnumWrapper_t::Count)
                {
                    auto const ENUM_VALUE{ static_cast<typename EnumWrapper_t::Enum>(imageIndex) };

                    sf::Texture texture;
                    Get(texture, ENUM_VALUE, ((willFlip) ? image::Flip::Yes : image::Flip::No));

                    auto const MAX_DIMMENSION_U{ static_cast<unsigned>(MaxDimmension()) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        ((texture.getSize().x == MAX_DIMMENSION_U)
                         || (texture.getSize().y == MAX_DIMMENSION_U)),
                        makeLogPrefix()
                            << "::Test() The call to Get(enum="
                            << EnumWrapper_t::ToString(ENUM_VALUE)
                            << ", will_flip=" << std::boolalpha << willFlip
                            << ") returned an image that was not of size=" << MAX_DIMMENSION_U
                            << "x" << MAX_DIMMENSION_U << ".  The actual size="
                            << texture.getSize().x << "x" << texture.getSize().y << ".");

                    game::LoopManager::Instance()->TestingImageSet(texture);

                    if (willFlip)
                    {
                        ++imageIndex;
                    }

                    willFlip = !willFlip;
                    return false;
                }

                game::LoopManager::Instance()->TestingStrAppend(
                    makeLogPrefix() + "  All Test Passed");

                return true;
            }

        private:
            std::string imageDirectoryPath_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_ENUM_IMAGE_LOADER_BASE_HPP_INCLUDED
