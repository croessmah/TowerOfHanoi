#include "Hanoi.h"
#include <cassert>

namespace
{

enum KernelIndex
{
    eFrom = 0,
    eBuffer = 1,
    eTo = 2
};

constexpr static unsigned gcMaxRings = 63;
constexpr static std::array<Hanoi::step_t, 3> gcEvenMoves_ {{ {eFrom, eBuffer}, {eFrom, eTo}, {eBuffer, eTo} }};
constexpr static std::array<Hanoi::step_t, 3> gcOddMoves_  {{ {eFrom, eTo}, {eFrom, eBuffer}, {eBuffer, eTo} }};

Hanoi::step_t step(std::array<Hanoi::rings_container_t, 3> &out, Hanoi::step_t step)
{
    const auto& initFrom = out[step.from];
    const auto& initTo = out[step.to];
    if (initFrom.empty() || (!initTo.empty() && initTo.back() < initFrom.back()))
        std::swap(step.from, step.to);

    out[step.to].push_back(out[step.from].back());
    out[step.from].pop_back();

    return {step.from, step.to};
}

uint64_t slice(std::array<Hanoi::rings_container_t, 3> &out, uint64_t curStep, uint64_t toStep, unsigned ringsCount, unsigned from, unsigned to)
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

}

Hanoi::Hanoi(unsigned count)
    : steps_((count & 1) ? gcOddMoves_ : gcEvenMoves_)
    , currentStep_(0)
{
    assert(count <= gcMaxRings && count > 1);
    for (auto& kernel: kernels_)
        kernel.reserve(count);

    for (unsigned i = count; i > 0; --i)
        kernels_[eFrom].push_back(i - 1);
}

void Hanoi::reset()
{
    const unsigned count = ringsCount();
    for (auto& kernel: kernels_)
        kernel.clear();

    for (unsigned i = count; i > 0; --i)
        kernels_[eFrom].push_back(i - 1);

    currentStep_ = 0;
}

Hanoi::step_t Hanoi::next()
{
    if (isEnd())
        return {eTo, eTo};

    const step_t currentMove = steps_[currentStep_ % 3];
    ++currentStep_;

    return step(kernels_, currentMove);
}

Hanoi::step_t Hanoi::prev()
{
    if (isBegin())
        return {eFrom, eFrom};

    --currentStep_;
    const step_t currentMove = steps_[currentStep_ % 3];

    return step(kernels_, currentMove);
}

void Hanoi::toStepEtalon(uint64_t newStep)
{
    if (newStep > steps())
        return;

    while (currentStep_ < newStep) {
        const step_t currentMove = steps_[currentStep_ % 3];
        ++currentStep_;
        step(kernels_, currentMove);
    }
    while (currentStep_ > newStep) {
        --currentStep_;
        const step_t currentMove = steps_[currentStep_ % 3];
        step(kernels_, currentMove);
    }
}

void Hanoi::toStep(uint64_t newStep)
{
    if (newStep > steps())
        return;

    const unsigned rCount = ringsCount();
    reset();
    currentStep_ = slice(kernels_, 0, newStep, rCount, eFrom, ((rCount & 1) ? eBuffer : eTo));
}

unsigned Hanoi::ringsCount() const
{
    return static_cast<unsigned>(from().size() + to().size() + buffer().size());
}

uint64_t Hanoi::steps() const
{
    return (uint64_t(1) << ringsCount()) - 1;
}

uint64_t Hanoi::currentStep() const
{
    return currentStep_;
}

bool Hanoi::isBegin() const
{
    return buffer().empty() && to().empty();
}

bool Hanoi::isEnd() const
{
    return buffer().empty() && from().empty();
}

const Hanoi::rings_container_t &Hanoi::from() const
{
    return kernels_[eFrom];
}

const Hanoi::rings_container_t &Hanoi::to() const
{
    return kernels_[eTo];
}

const Hanoi::rings_container_t &Hanoi::buffer() const
{
    return kernels_[eBuffer];
}

unsigned Hanoi::maxRings()
{
    return gcMaxRings;
}
