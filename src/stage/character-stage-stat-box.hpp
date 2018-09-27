// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CHARACTERSTAGE_STAT_BOX_HPP_INCLUDED
#define HEROESPATH_CHARACTERSTAGE_STAT_BOX_HPP_INCLUDED
//
// character-stage-stat-box.hpp
//
#include "creature/stat-set.hpp"
#include "creature/trait-enum.hpp"
#include "creature/trait.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/VertexArray.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class BoxEntity;
        using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
        using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;

    } // namespace gui
} // namespace sfml_util
namespace stage
{

    // Responsible for contents, positions, and sizes of the Character Stage's Stats Box.  These are
    // actually called Traits...
    class StatBox : public sf::Drawable
    {
    public:
        StatBox(
            sfml_util::IStagePtr_t owningStagePtr,
            const float WIDTH,
            const float HEIGHT_RATIO,
            const sf::Color & TEXT_COLOR);

        virtual ~StatBox();

        StatBox(const StatBox &) = delete;
        StatBox(StatBox &&) = delete;
        StatBox & operator=(const StatBox &) = delete;
        StatBox & operator=(StatBox &&) = delete;

        void Reset();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void Focus();

        const sf::FloatRect RegionOuter() const;
        const sf::FloatRect RegionInner() const;

        void SetVerticalPos(const float POS_TOP);

        static bool IsValid(const creature::Traits::Enum TRAIT);
        static bool IsValid(const std::size_t INDEX);
        static std::size_t StatCount() { return StatInvalid(); }
        static std::size_t IndexInvalid() { return StatCount(); }
        static creature::Traits::Enum StatInvalid() { return creature::Traits::StatCount; }

        // returns IndexInvalid() if TRAIT is invalid
        std::size_t Index(const creature::Traits::Enum TRAIT) const;

        // returns StatInvalid() if INDEX is invalid
        creature::Traits::Enum StatEnum(const std::size_t INDEX) const;

        // returns StatInvalid() if TRAIT is Strength or invalid
        creature::Traits::Enum Prevoius(const creature::Traits::Enum TRAIT) const;

        // returns StatInvalid() if TRAIT is Intelligence or invalid
        creature::Traits::Enum Next(const creature::Traits::Enum TRAIT) const;

        float RowHeight() const { return fullRegions_.front().height; }
        float NumberPosLeft() const { return labelRegions_.at(0).left; }

        // returns StatInvalid() if no stat is held down
        creature::Traits::Enum HeldDown() const { return heldDown_; }

        void HeldDown(const creature::Traits::Enum TRAIT_HELD_DOWN) { heldDown_ = TRAIT_HELD_DOWN; }

        const creature::StatSet CurrentSet() const { return currentSet_; }

        // does nothing if TRAIT is invalid
        void AppendMod(
            const creature::Traits::Enum TRAIT,
            const creature::Trait_t VALUE,
            const std::string & MODIFIER_NAME);

        void ClearMods();

        void SwapBaseValue(const creature::Traits::Enum A, const creature::Traits::Enum B);

        void SetBaseValue(const creature::Traits::Enum TRAIT, const creature::Trait_t VALUE);

        bool DoAllHaveValues() const { return doAllStatsHaveValues_; }

        bool WillShowModValues() const { return willShowModValues_; }

        void WillShowModValues(const bool WILL_SHOW_MOD_VALUES)
        {
            willShowModValues_ = WILL_SHOW_MOD_VALUES;
            Sync();
        }

        float TextScale() const { return textScale_; }

    private:
        // returns an empty string if TRAIT is invalid
        const std::string StatName(const creature::Traits::Enum TRAIT) const;

        // returns a negative value if TRAIT is invalid
        float NextModTextPositionLeft(const creature::Traits::Enum TRAIT) const;

        void Sync();

        sfml_util::IStagePtr_t owningStagePtr_;
        sfml_util::gui::BoxEntityUPtr_t boxUPtr_;
        std::vector<sf::FloatRect> fullRegions_;
        std::vector<sf::FloatRect> labelRegions_;
        std::vector<sf::FloatRect> numberRegions_;
        std::vector<sfml_util::Text> textLabels_;
        std::vector<sfml_util::Text> textValues_;
        sf::VertexArray vertexArray_;
        creature::Traits::Enum heldDown_;
        creature::StatSet baseSet_;
        creature::StatSet modSet_;
        creature::StatSet currentSet_;
        std::vector<sfml_util::gui::TextRegionUVec_t> modTextRegionVecs_;
        bool willShowModValues_;
        bool doAllStatsHaveValues_;
        float textScale_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_STAT_BOX_HPP_INCLUDED
