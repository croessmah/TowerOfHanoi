#include "HanoiTowerModel.h"
#include "Hanoi.h"

namespace
{

QJsonArray buildRingsJsonModel(const Hanoi::rings_container_t& rings)
{
    QJsonArray result;
    for (unsigned char ring: rings)
        result << static_cast<int>(ring);

    return result;
}

}

HanoiTowerModel::HanoiTowerModel(QObject *parent)
    : QObject{parent}
{
    connect(this, &HanoiTowerModel::refreshed, &HanoiTowerModel::ringsCountChanged);
    connect(this, &HanoiTowerModel::refreshed, &HanoiTowerModel::hasNextStepChanged);
    connect(this, &HanoiTowerModel::refreshed, &HanoiTowerModel::hasPrevStepChanged);
    connect(this, &HanoiTowerModel::refreshed, &HanoiTowerModel::stepsCountChanged);
    connect(this, &HanoiTowerModel::refreshed, &HanoiTowerModel::currentStepChanged);
}

HanoiTowerModel::~HanoiTowerModel()
{
}

void HanoiTowerModel::refresh(unsigned ringsCount)
{
    if (ringsCount < 1 || ringsCount > Hanoi::maxRings())
        return;

    hanoi_ = std::make_unique<Hanoi>(static_cast<unsigned>(ringsCount));

    emit refreshed();
}

void HanoiTowerModel::moveToStep(QVariant stepVariant)
{
    uint64_t step = stepVariant.toULongLong();//check ok?
    if (step > stepsCount())
        step = stepsCount();

    const uint64_t cur = currentStep();
    if (step == cur)
        return;

    const uint64_t forward = step > cur;
    while(step != hanoi_->currentStep())
    {
        if (forward)
            hanoi_->next();
        else
            hanoi_->prev();
    }

    emit refreshed();
}

void HanoiTowerModel::moveNext()
{
    if (!hanoi_ || hanoi_->isEnd())
        return;

    const bool isFirst = hanoi_->isBegin();

    Hanoi::step_t step = hanoi_->next();
    emit stepped(step.from, step.to);

    if (hanoi_->isEnd())
        emit hasNextStepChanged();

    if (isFirst != hanoi_->isBegin())
        emit hasPrevStepChanged();

    emit currentStepChanged();
}

void HanoiTowerModel::movePrev()
{
    if (!hanoi_ || hanoi_->isBegin())
        return;

    const bool isLast = hanoi_->isEnd();

    Hanoi::step_t step = hanoi_->prev();
    emit stepped(step.from, step.to);

    if (hanoi_->isBegin())
        emit hasPrevStepChanged();

    if (isLast != hanoi_->isEnd())
        emit hasNextStepChanged();

    emit currentStepChanged();
}

QJsonArray HanoiTowerModel::from() const
{
    if (!hanoi_)
        return {};

    return buildRingsJsonModel(hanoi_->from());
}

QJsonArray HanoiTowerModel::to() const
{
    if (!hanoi_)
        return {};

    return buildRingsJsonModel(hanoi_->to());
}

QJsonArray HanoiTowerModel::buffer() const
{
    if (!hanoi_)
        return {};

    return buildRingsJsonModel(hanoi_->buffer());
}

bool HanoiTowerModel::hasNextStep() const
{
    return hanoi_ && !hanoi_->isEnd();
}

bool HanoiTowerModel::hasPrevStep() const
{
    return hanoi_ && !hanoi_->isBegin();
}

uint64_t HanoiTowerModel::stepsCount() const
{
    return hanoi_ ? hanoi_->steps(): 0;
}

uint64_t HanoiTowerModel::currentStep() const
{
    return hanoi_ ? hanoi_->currentStep(): 0;
}

unsigned HanoiTowerModel::ringsCount() const
{
    return hanoi_ ? hanoi_->ringsCount() : 0;
}
