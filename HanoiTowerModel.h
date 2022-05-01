#ifndef HANOITOWERMODEL_H
#define HANOITOWERMODEL_H

#include <QObject>
#include <memory>
#include <QJsonArray>

class Hanoi;

class HanoiTowerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasNext READ hasNextStep NOTIFY hasNextStepChanged)
    Q_PROPERTY(bool hasPrev READ hasPrevStep NOTIFY hasPrevStepChanged)
    Q_PROPERTY(QVariant stepsCount READ stepsCount NOTIFY stepsCountChanged)
    Q_PROPERTY(QVariant currentStep READ currentStep NOTIFY currentStepChanged)
    Q_PROPERTY(unsigned ringsCount READ ringsCount NOTIFY ringsCountChanged)

public:
    explicit HanoiTowerModel(QObject *parent = nullptr);
    ~HanoiTowerModel();

    Q_INVOKABLE void refresh(unsigned ringsCount);
    Q_INVOKABLE void moveToStep(int step);
    Q_INVOKABLE void moveNext();
    Q_INVOKABLE void movePrev();
    Q_INVOKABLE QJsonArray from() const;
    Q_INVOKABLE QJsonArray to() const;
    Q_INVOKABLE QJsonArray buffer() const;

    bool hasNextStep() const;
    bool hasPrevStep() const;
    uint64_t stepsCount() const;
    uint64_t currentStep() const;
    unsigned ringsCount() const;

signals:
    void refreshed();
    void stepped(int from, int to);
    void hasNextStepChanged();
    void hasPrevStepChanged();
    void stepsCountChanged();
    void currentStepChanged();
    void ringsCountChanged();

private:
    std::unique_ptr<Hanoi> hanoi_;
};

#endif // HANOITOWERMODEL_H
