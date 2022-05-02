#ifndef HANOI_H
#define HANOI_H

#include <vector>
#include <array>
#include <cstdint>

class Hanoi
{
public:
    using rings_container_t = std::vector<unsigned char>;

    struct step_t
    {
        unsigned char from;
        unsigned char to;
    };

    Hanoi(unsigned count);
    void reset();
    step_t next();
    step_t prev();
    [[deprecated]] void toStepEtalon(uint64_t newStep);  //Оставлен для тестов корректности
    void toStep(uint64_t newStep);
    unsigned ringsCount() const;
    uint64_t steps() const;
    uint64_t currentStep() const;
    bool isBegin() const;
    bool isEnd() const;
    const rings_container_t& from() const;
    const rings_container_t& to() const;
    const rings_container_t& buffer() const;
    static unsigned maxRings();

private:
    std::array<rings_container_t, 3> kernels_;
    const std::array<step_t, 3>& steps_;
    uint64_t currentStep_;
};

#endif // HANOI_H
