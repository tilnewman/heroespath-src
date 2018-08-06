// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_LIST_ELEMENT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LIST_ELEMENT_HPP_INCLUDED
//
// list-element.hpp
//
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/list-no-element.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/image-loaders.hpp"
#include "sfml-util/image-options.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <boost/type_traits.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for keeping everything required of an element listed in a ListBox,
        // essentially a collection of: (1) an observer pointer to a game resource, (2) an image,
        // (3) some text, and (4) a bool indicating if this element is valid.
        //
        // Not designed to work if Element_t is an owning pointer.  Designed to work with observer
        // pointers such as item::ItemPtr_t/creature::CreaturePtr_t/etc or value types.
        template <typename Element_t>
        class ListElement : public sf::Drawable
        {
        public:
            using ElementOpt_t = boost::optional<Element_t>;

            ListElement(const ListElement<Element_t> & LE)
                : elementOpt_(LE.elementOpt_)
                , cachedTextureOpt_(LE.cachedTextureOpt_)
                , sprite_(LE.sprite_)
                , textRegionUPtr_()
                , isValid_(LE.isValid_)
            {
                if (LE.textRegionUPtr_)
                {
                    std::ostringstream ss;
                    ss << LE.textRegionUPtr_->GetEntityName() << "CopyMadeBy_ListElement<"
                       << boost::typeindex::type_id<Element_t>().pretty_name()
                       << ">::CopyConstructor(MakingCopyOf_" + LE.ToString() << ")_";

                    textRegionUPtr_
                        = std::make_unique<TextRegion>(ss.str(), LE.textRegionUPtr_->GetTextInfo());
                }
            }

            ListElement & operator=(const ListElement<Element_t> & LE)
            {
                if (LE.textRegionUPtr_)
                {
                    std::ostringstream ss;
                    ss << LE.textRegionUPtr_->GetEntityName() << "CopyMadeBy_" << ToString()
                       << "'s::CopyAssignmentOperator(MakingCopyOf_" << LE.ToString() << ")_";

                    textRegionUPtr_
                        = std::make_unique<TextRegion>(ss.str(), LE.textRegionUPtr_->GetTextInfo());
                }

                cachedTextureOpt_ = LE.cachedTextureOpt_;
                sprite_ = LE.sprite_;
                isValid_ = LE.isValid_;
            }

            ListElement(ListElement<Element_t> &&) = default;
            ListElement & operator=(ListElement<Element_t> &&) = default;

            ListElement(
                const ElementOpt_t & ELEMENT_OPT,
                const TextInfoOpt_t & TEXT_INFO_OPT,
                const CachedTextureOpt_t & CACHED_TEXTURE_OPT,
                const bool IS_VALID = true)
                : elementOpt_()
                , cachedTextureOpt_(CACHED_TEXTURE_OPT)
                , sprite_()
                , textRegionUPtr_()
                , isValid_(IS_VALID)
            {
                if constexpr (!std::is_same<Element_t, NoElement_t>::value)
                {
                    elementOpt_ = ELEMENT_OPT;
                }

                if (cachedTextureOpt_)
                {
                    sprite_.setTexture(cachedTextureOpt_->Get(), true);
                }

                if (TEXT_INFO_OPT)
                {
                    std::ostringstream ss;
                    ss << "TextRegionMadeBy_" << ToString() << "'s_Constructor_ThatSays_\""
                       << TEXT_INFO_OPT->text << "\"";

                    textRegionUPtr_ = std::make_unique<TextRegion>(ss.str(), TEXT_INFO_OPT.value());
                }

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (HasText() || HasImage()),
                    ToString()
                        + "'s constructor completed both without text and without an image.");
            }

            // image automatically loaded and cached from the ELEMENT provided
            ListElement(
                const Element_t & ELEMENT,
                const TextInfoOpt_t & TEXT_INFO_OPT,
                const sfml_util::ImageOptions & OPTIONS = sfml_util::ImageOptions(),
                const bool IS_VALID = true)
                : ListElement(ELEMENT, TEXT_INFO_OPT, LoadAndCacheImage(ELEMENT, OPTIONS), IS_VALID)
            {}

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override
            {
                if (textRegionUPtr_)
                {
                    target.draw(*textRegionUPtr_, states);
                }

                if (cachedTextureOpt_)
                {
                    target.draw(sprite_, states);
                }
            }

            // moves both the TextRegion and the image
            void Move(const float HORIZ, const float VERT) const
            {
                const sf::Vector2f MOVE_V(HORIZ, VERT);

                if (textRegionUPtr_)
                {
                    textRegionUPtr_->MoveEntityPos(MOVE_V.x, MOVE_V.y);
                }

                sprite_.move(MOVE_V);
            }

            bool IsValid() const { return isValid_; }

            void IsValid(const bool IS_VALID) { isValid_ = IS_VALID; }

            void IsValidFlip() { IsValid(!isValid_); }

            bool HasElement() const
            {
                if constexpr (std::is_same<Element_t, NoElement_t>::value)
                {
                    return false;
                }
                else
                {
                    return !!elementOpt_;
                }
            }

            const ElementOpt_t & ElementOpt() const { return elementOpt_; }

            const Element_t & Element() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (elementOpt_),
                    ToString() << "::Element() but the elementOpt_ is not initialized.");

                return elementOpt_.value();
            }

            bool HasImage() const { return !!cachedTextureOpt_; }

            bool HasCachedTexture() const { return HasImage(); }

            const CachedTextureOpt_t & CachedTextureOpt() const { return cachedTextureOpt_; }

            const CachedTexture & CachedTexture() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (cachedTextureOpt_),
                    ToString()
                        << "::CachedTexture() but the cachedTextureOpt_ is not initialized.");

                return cachedTextureOpt_.value();
            }

            bool HasText() const { return !!textRegionUPtr_; }

            bool HasTextRegion() const { return HasText(); }

            const TextRegionUPtr_t & TextRegionUPtr() const { return textRegionUPtr_; }

            // returns the plain text of the TextRegion, returns an empty string if there is no
            // TextRegion
            const std::string Text() const
            {
                if (textRegionUPtr_)
                {
                    return textRegionUPtr_->GetText();
                }
                else
                {
                    return "";
                }
            }

            // returns the TextRegion.GetEntityRegion() if there is one
            const sf::FloatRect RegionText() const
            {
                if (textRegionUPtr_)
                {
                    return textRegionUPtr_->GetEntityRegion();
                }
                else
                {
                    return sf::FloatRect(-1.0f, -1.0f, -1.0f, -1.0f);
                }
            }

            // returns TextRegion.GetTextInfo(), returns a default constructed TextInfo if there is
            // no TextRegion
            const TextInfo TextInfo() const
            {
                if (textRegionUPtr_)
                {
                    return textRegionUPtr_->GetTextInfo();
                }
                else
                {
                    return TextInfo();
                }
            }

            // returns TextRegion.GetEntityPos(), returns all negative values if there is no
            // TextRegion
            const sf::Vector2f TextPos() const
            {
                if (textRegionUPtr_)
                {
                    return textRegionUPtr_->GetEntityPos();
                }
                else
                {
                    return { -1.0f, -1.0f };
                }
            }

            void TextPos(const sf::Vector2f & NEW_POS) const
            {
                if (textRegionUPtr_)
                {
                    textRegionUPtr_->SetEntityPos(NEW_POS.x, NEW_POS.y);
                }
            }

            // returns the image region (sprite's global bounds), returns all negative values if
            // there is no image
            const sf::FloatRect RegionImage() const
            {
                if (cachedTextureOpt_)
                {
                    return sprite_.getGlobalBounds();
                }
                else
                {
                    return sf::FloatRect(-1.0f, -1.0f, -1.0f, -1.0f);
                }
            }

            void ImageFitCenterColor(const sf::FloatRect & RECT, const sf::Color & COLOR) const
            {
                sfml_util::FitAndCenterTo(sprite_, RECT);
                sprite_.setColor(COLOR);
            }

            friend bool
                operator==(const ListElement<Element_t> & L, const ListElement<Element_t> & R);

            friend bool
                operator<(const ListElement<Element_t> & L, const ListElement<Element_t> & R);

            const std::string ToString() const
            {
                std::ostringstream ss;
                ss << "ListElement<" << boost::typeindex::type_id<Element_t>().pretty_name() << ">("
                   << ((HasElement()) ? "HasElement" : "NoElement") << ")("
                   << ((cachedTextureOpt_) ? cachedTextureOpt_->Path() : "NoImage") << ")("
                   << ((textRegionUPtr_) ? "\"" + textRegionUPtr_->GetText() + "\"" : "NoText")
                   << ")";

                return ss.str();
            }

        private:
            ElementOpt_t elementOpt_;
            CachedTextureOpt_t cachedTextureOpt_;
            mutable sf::Sprite sprite_;
            mutable TextRegionUPtr_t textRegionUPtr_;
            bool isValid_;
        };

        template <typename Element_t>
        using ListElementOpt_t = boost::optional<ListElement<Element_t>>;

        template <typename Element_t>
        using ListElementPtr_t = misc::NotNull<ListElement<Element_t> *>;

        template <typename Element_t>
        using ListElementPtrOpt_t = boost::optional<ListElementPtr_t<Element_t>>;

        template <typename Element_t>
        using ListElementUPtr_t = std::unique_ptr<ListElement<Element_t>>;

        template <typename Element_t>
        using ListElementUVec_t = std::vector<ListElementUPtr_t<Element_t>>;

        template <typename Element_t>
        bool operator==(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            // sprite_ and textRegion's details are intentionally excluded

            if (L.textRegionUPtr_ && R.textRegionUPtr_)
            {
                return std::tie(
                           L.elementOpt_,
                           L.cachedTextureOpt_,
                           L.textRegionUPtr_->GetText(),
                           L.isValid_)
                    == std::tie(
                           R.elementOpt_,
                           R.cachedTextureOpt_,
                           R.textRegionUPtr_->GetText(),
                           R.isValid_);
            }
            else
            {
                return std::tie(L.elementOpt_, L.cachedTextureOpt_, L.isValid_)
                    == std::tie(R.elementOpt_, R.cachedTextureOpt_, R.isValid_);
            }
        }

        template <typename Element_t>
        inline bool operator!=(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            return !(L == R);
        }

        template <typename Element_t>
        bool operator<(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            // sprite_ and textRegio's details are intentionally excluded

            if (L.textRegionUPtr_ && R.textRegionUPtr_)
            {
                return std::tie(
                           L.elementOpt_,
                           L.cachedTextureOpt_,
                           L.textRegionUPtr_->GetText(),
                           L.isValid_)
                    < std::tie(
                           R.elementOpt_,
                           R.cachedTextureOpt_,
                           R.textRegionUPtr_->GetText(),
                           R.isValid_);
            }
            else
            {
                return std::tie(L.elementOpt_, L.cachedTextureOpt_, L.isValid_)
                    < std::tie(R.elementOpt_, R.cachedTextureOpt_, R.isValid_);
            }
        }

        template <typename Element_t>
        bool operator>(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            return (R < L);
        }

        template <typename Element_t>
        bool operator<=(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            return !(L > R);
        }

        template <typename Element_t>
        bool operator>=(const ListElement<Element_t> & L, const ListElement<Element_t> & R)
        {
            return !(L < R);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LIST_ELEMENT_HPP_INCLUDED
