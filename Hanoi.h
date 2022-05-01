#ifndef HANOI_H
#define HANOI_H

#include <vector>
#include <array>
#include <cstdint>
#include <cassert>

class Hanoi
{
public:
    using rings_container_t = std::vector<unsigned char>;

    struct step_t
    {
        unsigned char from;
        unsigned char to;
    };

    Hanoi(unsigned count)
        : steps_((count & 1) ? csOddMoves_ : csEvenMoves_)
        , currentStep_(0)
    {
        assert(count <= csMaxRings && csMaxRings > 0);
        for (auto& kernel: kernels_)
            kernel.reserve(count);

        for (unsigned i = count; i > 0; --i)
            kernels_[eFrom].push_back(i - 1);
    }

    step_t next()
    {
        if (isEnd())
            return {eTo, eTo};
        const step_t currentMove = steps_[currentStep_ % 3];
        ++currentStep_;
        return step(currentMove);
    }

    step_t prev()
    {
        if (isBegin())
            return {eFrom, eFrom};

        --currentStep_;
        const step_t currentMove = steps_[currentStep_ % 3];
        return step(currentMove);
    }

    unsigned ringsCount() const { return static_cast<unsigned>(from().size() + to().size() + buffer().size()); }
    uint64_t steps() const { return (uint64_t(1) << ringsCount()) - 1; }
    uint64_t currentStep() const { return currentStep_; }
    bool isBegin() const { return buffer().empty() && to().empty(); }
    bool isEnd() const { return buffer().empty() && from().empty(); }

    const rings_container_t& from() const { return kernels_[eFrom]; }
    const rings_container_t& to() const { return kernels_[eTo]; }
    const rings_container_t& buffer() const { return kernels_[eBuffer]; }

    constexpr static unsigned maxRings() { return csMaxRings; }
private:
    enum
    {
        eFrom = 0,
        eBuffer = 1,
        eTo = 2
    };

    step_t step(step_t step)
    {
        if (kernels_[step.from].empty())
            std::swap(step.from, step.to);
        else if (!kernels_[step.to].empty() && kernels_[step.to].back() < kernels_[step.from].back())
            std::swap(step.from, step.to);

        kernels_[step.to].push_back(kernels_[step.from].back());
        kernels_[step.from].pop_back();

        return {step.from, step.to};
    }

    std::array<rings_container_t, 3> kernels_;
    const std::array<step_t, 3>& steps_;
    unsigned currentStep_;
    constexpr static unsigned csMaxRings = 64;
    constexpr static std::array<step_t, 3> csEvenMoves_ {{ {eFrom, eBuffer}, {eFrom, eTo}, {eBuffer, eTo} }};
    constexpr static std::array<step_t, 3> csOddMoves_  {{ {eFrom, eTo}, {eFrom, eBuffer}, {eBuffer, eTo} }};
};

#endif // HANOI_H
