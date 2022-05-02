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
        assert(count <= csMaxRings && count > 1);
        for (auto& kernel: kernels_)
            kernel.reserve(count);

        for (unsigned i = count; i > 0; --i)
            kernels_[eFrom].push_back(i - 1);
    }

    void reset()
    {
        const unsigned count = ringsCount();
        for (auto& kernel: kernels_)
            kernel.clear();

        for (unsigned i = count; i > 0; --i)
            kernels_[eFrom].push_back(i - 1);

        currentStep_ = 0;
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

    [[deprecated]] void toStepEtalon(uint64_t newStep) //Оставлен для тестов корректности
    {
        if (newStep > steps())
            return;

        while (currentStep_ < newStep) {
            const step_t currentMove = steps_[currentStep_ % 3];
            ++currentStep_;
            step(currentMove);
        }
        while (currentStep_ > newStep) {
            --currentStep_;
            const step_t currentMove = steps_[currentStep_ % 3];
            step(currentMove);
        }
    }

    void toStep(uint64_t newStep)
    {
        if (newStep > steps())
            return;

        const unsigned rCount = ringsCount();
        reset();
        currentStep_ = slice(kernels_, 0, newStep, rCount, eFrom, ((rCount & 1) ? eBuffer : eTo));
    }

    unsigned ringsCount() const
    {
        return static_cast<unsigned>(from().size() + to().size() + buffer().size());
    }

    uint64_t steps() const
    {
        return (uint64_t(1) << ringsCount()) - 1;
    }

    uint64_t currentStep() const
    {
        return currentStep_;
    }

    bool isBegin() const
    {
        return buffer().empty() && to().empty();
    }

    bool isEnd() const
    {
        return buffer().empty() && from().empty();
    }

    const rings_container_t& from() const
    {
        return kernels_[eFrom];
    }

    const rings_container_t& to() const
    {
        return kernels_[eTo];
    }

    const rings_container_t& buffer() const
    {
        return kernels_[eBuffer];
    }

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
        const rings_container_t& initFrom = kernels_[step.from];
        const rings_container_t& initTo = kernels_[step.to];
        if (initFrom.empty() || (!initTo.empty() && initTo.back() < initFrom.back()))
            std::swap(step.from, step.to);

        kernels_[step.to].push_back(kernels_[step.from].back());
        kernels_[step.from].pop_back();

        return {step.from, step.to};
    }

    static uint64_t slice(std::array<rings_container_t, 3>& out, uint64_t curStep, uint64_t toStep, unsigned ringsCount, unsigned from, unsigned to)
    {
        const unsigned buff = 0x03 & (~from & ~to);
        const uint64_t nStep = (uint64_t(1) << ringsCount) - 1;

        if ((curStep + nStep) <= toStep)
        {
            out[to].insert(out[to].end(), std::prev(out[from].end(), ringsCount), out[from].end());
            out[from].erase(std::prev(out[from].end(), ringsCount), out[from].end());

            if (curStep + nStep == toStep)
                return toStep;

            if (!out[from].empty())
            {
                out[buff].push_back(out[from].back());
                out[from].pop_back();
            }

            return slice(out, curStep + nStep + 1, toStep, ringsCount, to, buff);
        }

        return slice(out, curStep, toStep, ringsCount - 1, from, buff);
    }

    std::array<rings_container_t, 3> kernels_;
    const std::array<step_t, 3>& steps_;
    unsigned currentStep_;

    constexpr static unsigned csMaxRings = 63;
    constexpr static std::array<step_t, 3> csEvenMoves_ {{ {eFrom, eBuffer}, {eFrom, eTo}, {eBuffer, eTo} }};
    constexpr static std::array<step_t, 3> csOddMoves_  {{ {eFrom, eTo}, {eFrom, eBuffer}, {eBuffer, eTo} }};
};

#endif // HANOI_H
