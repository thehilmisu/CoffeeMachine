// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#include "coffeemaker.h"

#include <QEventTransition>
#include <QSettings>
#include <QRandomGenerator>
#include <QState>
#include <QStateMachine>
#include <QFinalState>
#include <QTimer>
#include <QDebug>

#include <array>

// -------------------------------------------------------------------------------------------------
namespace {
    constexpr auto milkMax = 750;
    constexpr auto waterMax = 1150;
    constexpr auto beansMax = 550;
    constexpr auto overflowMax = 700;
    constexpr auto restBinMax = 600;
    constexpr auto maxCupsUntilCleanReq = 25;

    enum CustomTypes {
        CustomString = QEvent::User+1,

        CustomInteger,
        CustomRestBin,
        CustomOverflow,
        CustomCupCount,
        CustomBeans,
        CustomWater,
        CustomMilk,

        CustomPrepMilk,
        CustomBrewStep,
        CustomGrind,
    };

    constexpr auto StringEventType = QEvent::Type(CustomString);
    constexpr auto IntegerEventType = QEvent::Type(CustomInteger);

    constexpr auto RestBinEventType = QEvent::Type(CustomRestBin);
    constexpr auto OverflowEventType = QEvent::Type(CustomOverflow);
    constexpr auto WaterEventType = QEvent::Type(CustomWater);
    constexpr auto MilkEventType = QEvent::Type(CustomMilk);
    constexpr auto BeansEventType = QEvent::Type(CustomBeans);
    constexpr auto CupCountEventType = QEvent::Type(CustomCupCount);

    constexpr auto PrepMilkEventType = QEvent::Type(CustomPrepMilk);
    constexpr auto BrewEventType = QEvent::Type(CustomBrewStep);
    constexpr auto GrindEventType = QEvent::Type(CustomGrind);
}

// -------------------------------------------------------------------------------------------------
struct StringEvent : public QEvent
{
    StringEvent(const QString &val)
    : QEvent(StringEventType),
      value(val) {}

    const QString value;
};

// -------------------------------------------------------------------------------------------------
struct IntegerEvent : public QEvent
{
    IntegerEvent(int val, QEvent::Type t = IntegerEventType)
    : QEvent(t),
      value(val) {}

    const int value;
};

// -------------------------------------------------------------------------------------------------
struct PrepMilkEvent : public QEvent
{
    PrepMilkEvent(const CoffeeMaker::MilkOptions& options)
    : QEvent(PrepMilkEventType),
      value(options) {}

    const CoffeeMaker::MilkOptions value;
};

// -------------------------------------------------------------------------------------------------
struct BrewEvent : public QEvent
{
    BrewEvent(const CoffeeMaker::WaterOptions& options)
    : QEvent(BrewEventType),
      value(options) {}

    const CoffeeMaker::WaterOptions value;
};

// -------------------------------------------------------------------------------------------------
struct GrindEvent : public QEvent
{
    GrindEvent(const CoffeeMaker::GrindOptions& options)
    : QEvent(GrindEventType),
      value(options) {}

    const CoffeeMaker::GrindOptions value;
};

// -------------------------------------------------------------------------------------------------
class PrepMilkTransition : public QAbstractTransition
{
public:
    PrepMilkTransition(std::shared_ptr<CoffeeMaker::MilkOptions> options)
        : m_options(std::move(options)) {}

protected:
    bool eventTest(QEvent *e) override {
        return (e->type() == PrepMilkEventType);
    }

    void onTransition(QEvent* e) override {
        if (e->type() != PrepMilkEventType) return;
        const auto ce = static_cast<PrepMilkEvent*>(e);
        (*m_options) = ce->value;
    }

private:
    const std::shared_ptr<CoffeeMaker::MilkOptions> m_options;
};

// -------------------------------------------------------------------------------------------------
class GrindTransition : public QAbstractTransition
{
public:
    GrindTransition(std::shared_ptr<CoffeeMaker::GrindOptions> options)
        : m_options(std::move(options)) {}

protected:
    bool eventTest(QEvent *e) override {
        return (e->type() == GrindEventType);
    }

    void onTransition(QEvent* e) override {
        if (e->type() != GrindEventType) return;
        const auto ce = static_cast<GrindEvent*>(e);
        qDebug() << Q_FUNC_INFO;
        (*m_options) = ce->value;
    }

private:
    const std::shared_ptr<CoffeeMaker::GrindOptions> m_options;
};

// -------------------------------------------------------------------------------------------------
class BrewTransition : public QAbstractTransition
{
public:
    BrewTransition(std::shared_ptr<CoffeeMaker::WaterOptions> options)
        : m_options(std::move(options)) {}

protected:
    bool eventTest(QEvent *e) override {
        return (e->type() == BrewEventType);
    }

    void onTransition(QEvent* e) override {
        if (e->type() != BrewEventType) return;
        const auto ce = static_cast<BrewEvent*>(e);
        (*m_options) = ce->value;
    }

private:
    const std::shared_ptr<CoffeeMaker::WaterOptions> m_options;
};

// -------------------------------------------------------------------------------------------------
class StringTransition : public QAbstractTransition
{
public:
    StringTransition(const QString &value) : m_value(value) {}

protected:
    bool eventTest(QEvent *e) override
    {
        if (e->type() != StringEventType) return false;
        const auto ce = static_cast<StringEvent*>(e);
        return (m_value == ce->value);
    }

    void onTransition(QEvent*) override {}

private:
    const QString m_value;
};

// -------------------------------------------------------------------------------------------------
class IntLargerThanTransition : public QAbstractTransition
{
public:
    IntLargerThanTransition(int value, QEvent::Type t = IntegerEventType)
        : m_value(value), m_type(t) {}

protected:
    bool eventTest(QEvent *e) override
    {
        if (e->type() != m_type) return false;
        const auto ce = static_cast<IntegerEvent*>(e);
        return (m_value < ce->value);
    }

    void onTransition(QEvent *) override {}

private:
    const int m_value;
    const QEvent::Type m_type;
};

// -------------------------------------------------------------------------------------------------
class IntSmallerThanTransition : public QAbstractTransition
{
public:
    IntSmallerThanTransition(int value, QEvent::Type t = IntegerEventType)
        : m_value(value), m_type(t) {}

protected:
    bool eventTest(QEvent *e) override
    {
        if (e->type() != m_type) return false;
        const auto ce = static_cast<IntegerEvent*>(e);
        return (m_value > ce->value);
    }

    void onTransition(QEvent *) override {}

private:
    const int m_value;
    const QEvent::Type m_type;
};

// -------------------------------------------------------------------------------------------------
class IntEqualToTransition : public QAbstractTransition
{
public:
    IntEqualToTransition(int value, QEvent::Type t = IntegerEventType)
        : m_value(value), m_type(t) {}

protected:
    bool eventTest(QEvent *e) override
    {
        if (e->type() != m_type) return false;
        const auto ce = static_cast<IntegerEvent*>(e);
        return (m_value == ce->value);
    }

    void onTransition(QEvent *) override {}

private:
    const int m_value;
    const QEvent::Type m_type;
};

// -------------------------------------------------------------------------------------------------
CoffeeMaker::CoffeeMaker(QObject* parent)
    : QObject(parent)
    , settings_(new QSettings("MyCoffeeMachine", "MachineState", this))
    , stateMachine_(new QStateMachine(this))
    , stateOff_(new QState(stateMachine_))
    , stateSelfCheck_(new QState(stateMachine_))
    , stateBinFull_(new QState(stateMachine_))
    , stateOverflowFull1_(new QState(stateMachine_))
    , stateCleaningReq_(new QState(stateMachine_))
    , stateStandBy_(new QState(stateMachine_))
    , stateCommandMode_(new QState(stateMachine_))
    , stateGrinding_(new QState(stateMachine_))
    , stateBeansEmpty_(new QState(stateMachine_))
    , stateBrewing_(new QState(stateMachine_))
    , stateWaterEmpty_(new QState(stateMachine_))
    , statePrepMilk_(new QState(stateMachine_))
    , stateMilkEmpty_(new QState(stateMachine_))
    , grindOptions_(std::make_shared<GrindOptions>())
    , waterOptions_(std::make_shared<WaterOptions>())
    , milkOptions_(std::make_shared<MilkOptions>())
{
    // Initialize from last state or assign randomly within max values
    beansContainerLevel_ = settings_->value("beansContainerLevel", QRandomGenerator::global()->bounded(0, beansMax)).toInt();
    milkContainerLevel_ = settings_->value("milkContainerLevel", QRandomGenerator::global()->bounded(0, waterMax)).toInt();
    waterContainerLevel_ = settings_->value("waterContainerLevel", QRandomGenerator::global()->bounded(0, milkMax)).toInt();
    restBinLevel_ = settings_->value("restBinLevel", QRandomGenerator::global()->bounded(0, restBinMax)).toInt();
    overflowContainerLevel_ = settings_->value("overflowLevel", QRandomGenerator::global()->bounded(0, overflowMax)).toInt();
    cupsProcessed_ = settings_->value("cupsProcessed", 0).toInt();

    stateMachine_->setInitialState(stateOff_);

    qDebug() << qPrintable(QString("beans: %1/%2").arg(beansContainerLevel_).arg(beansMax));
    qDebug() << qPrintable(QString("water: %1/%2").arg(waterContainerLevel_).arg(waterMax));
    qDebug() << qPrintable(QString("milk : %1/%2").arg(milkContainerLevel_).arg(milkMax));
    qDebug() << qPrintable(QString("restbin: %1/%2").arg(restBinLevel_).arg(restBinMax));
    qDebug() << qPrintable(QString("overflow: %1/%2").arg(overflowContainerLevel_).arg(overflowMax));
    qDebug() << qPrintable(QString("cups: %1/%2").arg(cupsProcessed_).arg(maxCupsUntilCleanReq));

    std::array<QState*, 13> allStates = {
        stateOff_, stateSelfCheck_, stateBinFull_, stateOverflowFull1_, stateCleaningReq_, stateStandBy_,
        stateCommandMode_, stateGrinding_, stateBeansEmpty_, stateBrewing_, stateWaterEmpty_, statePrepMilk_,
        stateMilkEmpty_
    };

    { // Start transition
        const auto startTransition = new StringTransition("turn_on");
        startTransition->setTargetState(stateSelfCheck_);
        stateOff_->addTransition(startTransition);
    }

    { // Self-check timer
        const auto selfCheckTimer = new QTimer(stateSelfCheck_);
        selfCheckTimer->setInterval(1234);
        selfCheckTimer->setSingleShot(true);
        const auto selfCheckSubState = new QState(stateSelfCheck_);
        connect(selfCheckSubState, &QState::entered, selfCheckTimer, QOverload<>::of(&QTimer::start));
        const auto selfCheckDone = new QFinalState(stateSelfCheck_);
        selfCheckSubState->addTransition(selfCheckTimer, &QTimer::timeout, selfCheckDone);
        stateSelfCheck_->setInitialState(selfCheckSubState);
    }

    { // Self check ok transition, and others
        const auto chkOkTransition = new StringTransition("check_ok");
        chkOkTransition->setTargetState(stateStandBy_);
        stateSelfCheck_->addTransition(chkOkTransition);

        const auto toBinFull = new IntLargerThanTransition(restBinMax -1, RestBinEventType);
        toBinFull->setTargetState(stateBinFull_);
        stateSelfCheck_->addTransition(toBinFull);

        const auto toCleanReq = new IntLargerThanTransition(maxCupsUntilCleanReq -1, CupCountEventType);
        toCleanReq->setTargetState(stateCleaningReq_);
        stateSelfCheck_->addTransition(toCleanReq);

        const auto toOverflow = new IntLargerThanTransition(overflowMax -1, OverflowEventType);
        toOverflow->setTargetState(stateOverflowFull1_);
        stateSelfCheck_->addTransition(toOverflow);
    }

    { // Standby Transitions outgoing
        const auto startTransition = new StringTransition("start");
        startTransition->setTargetState(stateCommandMode_);
        stateStandBy_->addTransition(startTransition);

        const auto toBinFull = new IntLargerThanTransition(restBinMax -1, RestBinEventType);
        toBinFull->setTargetState(stateBinFull_);
        stateStandBy_->addTransition(toBinFull);

        const auto toCleanReq = new IntLargerThanTransition(maxCupsUntilCleanReq -1, CupCountEventType);
        toCleanReq->setTargetState(stateCleaningReq_);
        stateStandBy_->addTransition(toCleanReq);

        const auto toOverflow = new IntLargerThanTransition(overflowMax -1, OverflowEventType);
        toOverflow->setTargetState(stateOverflowFull1_);
        stateStandBy_->addTransition(toOverflow);
    }

    { // Transitions to standby
        const auto fromBinFull = new IntSmallerThanTransition(restBinMax, RestBinEventType);
        fromBinFull->setTargetState(stateStandBy_);
        stateBinFull_->addTransition(fromBinFull);

        const auto fromClean = new IntSmallerThanTransition(maxCupsUntilCleanReq, CupCountEventType);
        fromClean->setTargetState(stateStandBy_);
        stateCleaningReq_->addTransition(fromClean);

        const auto fromOverflow = new IntSmallerThanTransition(overflowMax, OverflowEventType);
        fromOverflow->setTargetState(stateStandBy_);
        stateOverflowFull1_->addTransition(fromOverflow);

        std::array<QState*, 7> someStates = {
            stateGrinding_, stateBeansEmpty_, stateBrewing_, stateWaterEmpty_, statePrepMilk_,
            stateMilkEmpty_, stateCommandMode_
        };

        for (const auto s : someStates) {
            const auto cancelTransition = new StringTransition("cancel");
            cancelTransition->setTargetState(stateStandBy_);
            s->addTransition(cancelTransition);
            connect(cancelTransition, &StringTransition::triggered, this, [this](){
                addToCupsProcessed(1);
            });
        }

        const auto finishTransition = new StringTransition("finish");
        finishTransition->setTargetState(stateStandBy_);
        stateCommandMode_->addTransition(finishTransition);
        connect(finishTransition, &StringTransition::triggered, this, [this](){
            addToCupsProcessed(1);
        });
    }

    { // Commandstate Transitions outgoing
        const auto grindTransition = new GrindTransition(grindOptions_);
        grindTransition->setTargetState(stateGrinding_);
        stateCommandMode_->addTransition(grindTransition);

        const auto brewTransition = new BrewTransition(waterOptions_);
        brewTransition->setTargetState(stateBrewing_);
        stateCommandMode_->addTransition(brewTransition);

        const auto milkTransition = new PrepMilkTransition(milkOptions_);
        milkTransition->setTargetState(statePrepMilk_);
        stateCommandMode_->addTransition(milkTransition);
    }

    // on grinding state
    connect(stateGrinding_, &QState::entered, this, [this]()
    {
        qDebug() << qPrintable(QString("current beans: %1/%2").arg(beansContainerLevel_).arg(beansMax));
        qDebug() << "Start grinding: beans: " << grindOptions_->beansInGram << static_cast<int>(grindOptions_->grindLevel);
        const auto beans = getBeans(grindOptions_->beansInGram);
        currentCoffeeGroundAmount_ += beans;
        grindOptions_->beansInGram -= beans;
    });

    { // Grinding timer
        const auto timer = new QTimer(stateGrinding_);
        timer->setInterval(2500);
        timer->setSingleShot(true);
        const auto timingState = new QState(stateGrinding_);
        connect(timingState, &QState::entered, timer, QOverload<>::of(&QTimer::start));
        const auto done = new QFinalState(stateGrinding_);
        timingState->addTransition(timer, &QTimer::timeout, done);
        stateGrinding_->setInitialState(timingState);
    }

    stateGrinding_->addTransition(stateGrinding_, &QState::finished, stateCommandMode_);

    { // Beans Empty
        const auto toBeansEmpty = new IntSmallerThanTransition(1, BeansEventType);
        toBeansEmpty->setTargetState(stateBeansEmpty_);
        stateGrinding_->addTransition(toBeansEmpty);

        const auto fromBeansEmpty = new IntLargerThanTransition(0, BeansEventType);
        fromBeansEmpty->setTargetState(stateGrinding_);
        stateBeansEmpty_->addTransition(fromBeansEmpty);
    }

    // on brewing state ---------------------
    connect(stateBrewing_, &QState::entered, this, [this]()
    {
        qDebug() << qPrintable(QString("current water: %1/%2").arg(waterContainerLevel_).arg(waterMax));
        qDebug() << "Start brewing: water: " << waterOptions_->waterMl << ", temp:"<< waterOptions_->temperatureC;
        const auto water = getWater(waterOptions_->waterMl);
        waterOptions_->waterMl -= water;
        if (!cupDetected()) {
            addToOverflow(water);
        }
    });

    { // Brewing timer
        const auto timer = new QTimer(stateBrewing_);
        timer->setInterval(3003);
        timer->setSingleShot(true);
        const auto timingState = new QState(stateBrewing_);
        connect(timingState, &QState::entered, timer, QOverload<>::of(&QTimer::start));
        const auto done = new QFinalState(stateBrewing_);
        timingState->addTransition(timer, &QTimer::timeout, done);
        stateBrewing_->setInitialState(timingState);
    }

    stateBrewing_->addTransition(stateBrewing_, &QState::finished, stateCommandMode_);

    { // Water Empty
        const auto toEmpty = new IntSmallerThanTransition(1, WaterEventType);
        toEmpty->setTargetState(stateWaterEmpty_);
        stateBrewing_->addTransition(toEmpty);

        const auto fromEmpty = new IntLargerThanTransition(0, WaterEventType);
        fromEmpty->setTargetState(stateBrewing_);
        stateWaterEmpty_->addTransition(fromEmpty);
    }

    // on prep milk state ----------------
    connect(statePrepMilk_, &QState::entered, this, [this]()
    {
        qDebug() << qPrintable(QString("current milk: %1/%2").arg(milkContainerLevel_).arg(milkMax));
        qDebug() << "Start prepping milk: amount: " << milkOptions_->milkMl
                 << ", temp:"<< milkOptions_->temperatureC << ", foam: " << milkOptions_->foam;
        const auto milk = getMilk(milkOptions_->milkMl);
        milkOptions_->milkMl -= milk;
        if (!cupDetected()) {
            addToOverflow(milk);
        }
    });

    { // Milk prepare timer
        const auto timer = new QTimer(statePrepMilk_);
        timer->setInterval(3500);
        timer->setSingleShot(true);
        const auto timingState = new QState(statePrepMilk_);
        connect(timingState, &QState::entered, timer, QOverload<>::of(&QTimer::start));
        const auto done = new QFinalState(statePrepMilk_);
        timingState->addTransition(timer, &QTimer::timeout, done);
        statePrepMilk_->setInitialState(timingState);
    }

    statePrepMilk_->addTransition(statePrepMilk_, &QState::finished, stateCommandMode_);

    { // Milk Empty
        const auto toEmpty = new IntSmallerThanTransition(1, MilkEventType);
        toEmpty->setTargetState(stateMilkEmpty_);
        statePrepMilk_->addTransition(toEmpty);

        const auto fromEmpty = new IntLargerThanTransition(0, MilkEventType);
        fromEmpty->setTargetState(statePrepMilk_);
        stateMilkEmpty_->addTransition(fromEmpty);
    }

    connect(this, &CoffeeMaker::cupsProcessedChanged, this, &CoffeeMaker::doSelfCheck);
    connect(this, &CoffeeMaker::waterContainerLevelChanged, this, &CoffeeMaker::doSelfCheck);
    connect(this, &CoffeeMaker::beansContainerLevelChanged, this, &CoffeeMaker::doSelfCheck);
    connect(this, &CoffeeMaker::restBinLevelChanged, this, &CoffeeMaker::doSelfCheck);
    connect(this, &CoffeeMaker::milkContainerLevelChanged, this, &CoffeeMaker::doSelfCheck);
    connect(this, &CoffeeMaker::overflowContainerLevelChanged, this, &CoffeeMaker::doSelfCheck);

    // Emit state changed signals for the coffee maker
    for (const auto s : allStates) {
        connect(s, &QState::entered, this, [this](){ emit currentStateChanged(currentState()); });
        if (s != stateOff_)
            connect(s, &QState::entered, this, [this](){ doSelfCheck(); });
    }

    // Turn off transistion from every state (but the off state itself)
    for (const auto s : allStates)
    {
        if (s == stateOff_) continue;
        const auto offTransition = new StringTransition("turn_off");
        offTransition->setTargetState(stateOff_);
        s->addTransition(offTransition);

        if (s == stateWaterEmpty_ || s == stateMilkEmpty_ || s == stateBeansEmpty_
            || s == stateGrinding_ || s == stateBrewing_ || s == statePrepMilk_
            || s == stateCommandMode_ )
        {
            connect(offTransition, &StringTransition::triggered, this, [this](){
                addToCupsProcessed(1);
            });
        }

    }

    // on stand by
    connect(stateStandBy_, &QState::entered, this, [this]() {
        emptyCoffeeGrounds(); // coffee ground that might be in the chamber to bin
    });

    stateMachine_->start();
}

// -------------------------------------------------------------------------------------------------
CoffeeMaker::State CoffeeMaker::currentState() const
{
    if (stateMachine_->configuration().contains(stateOff_)) return State::Off;
    if (stateMachine_->configuration().contains(stateSelfCheck_))return State::SelfCheck;
    if (stateMachine_->configuration().contains(stateBinFull_)) return State::BinFull;
    if (stateMachine_->configuration().contains(stateOverflowFull1_)) return State::OverflowFull;
    if (stateMachine_->configuration().contains(stateCleaningReq_)) return State::CleaningRequired;
    if (stateMachine_->configuration().contains(stateStandBy_)) return State::StandBy;
    if (stateMachine_->configuration().contains(stateCommandMode_)) return State::CommandMode;
    if (stateMachine_->configuration().contains(stateGrinding_)) return State::Grinding;
    if (stateMachine_->configuration().contains(stateBeansEmpty_)) return State::BeansEmpty;
    if (stateMachine_->configuration().contains(stateBrewing_)) return State::Brewing;
    if (stateMachine_->configuration().contains(stateWaterEmpty_)) return State::WaterEmpty;
    if (stateMachine_->configuration().contains(statePrepMilk_)) return State::PrepMilk;
    if (stateMachine_->configuration().contains(stateMilkEmpty_)) return State::MilkEmpty;
    return State::Unknown;
}

// -------------------------------------------------------------------------------------------------
int CoffeeMaker::beansContainerMax() const { return beansMax; }
int CoffeeMaker::waterContainerMax() const { return waterMax; }
int CoffeeMaker::milkContainerMax() const { return milkMax; }
int CoffeeMaker::restBinLevelMax() const { return restBinMax; }
int CoffeeMaker::overflowContainerMax() const { return overflowMax; }

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addMilkToContainer(int milkMl)
{
    setMilkContainerLevel(qMin(milkMax, milkContainerLevel_ + milkMl));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addWatertoContainer(int waterMl)
{
    setWaterContainerLevel(qMin(waterMax, waterContainerLevel_ + waterMl));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addBeanstoContainer(int beansGram)
{
    setBeansContainerLevel(qMin(beansMax, beansContainerLevel_ + beansGram));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setBeansContainerLevel(int level)
{
    if (beansContainerLevel_ == level) return;
    beansContainerLevel_ = level;
    settings_->setValue("beansContainerLevel", beansContainerLevel_);
    emit beansContainerLevelChanged(beansContainerLevel_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setWaterContainerLevel(int level)
{
    if (waterContainerLevel_ == level) return;
    waterContainerLevel_ = level;
    settings_->setValue("waterContainerLevel", waterContainerLevel_);
    emit waterContainerLevelChanged(waterContainerLevel_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setMilkContainerLevel(int level)
{
    if (milkContainerLevel_ == level) return;
    milkContainerLevel_ = level;
    settings_->setValue("milkContainerLevel", milkContainerLevel_);
    emit milkContainerLevelChanged(milkContainerLevel_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setOverflowLevel(int level)
{
    if (overflowContainerLevel_ == level) return;
    overflowContainerLevel_ = level;
    settings_->setValue("overflowLevel", overflowContainerLevel_);
    emit overflowContainerLevelChanged(overflowContainerLevel_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setRestBinLevel(int level)
{
    if (restBinLevel_ == level) return;
    restBinLevel_ = level;
    settings_->setValue("restBinLevel", restBinLevel_);
    emit restBinLevelChanged(restBinLevel_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::setCupsProcessed(int cups)
{
    if (cupsProcessed_ == cups) return;
    cupsProcessed_ = cups;
    settings_->setValue("cupsProcessed", cupsProcessed_);
    emit cupsProcessedChanged(cupsProcessed_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::placeCup()
{
    if (cupDetected_) return;
    cupDetected_ = true;
    emit cupDetectedChanged(cupDetected_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::removeCup()
{
    if (!cupDetected_) return;
    cupDetected_ = false;
    emit cupDetectedChanged(cupDetected_);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::turnOn()
{
    stateMachine_->postEvent(new StringEvent("turn_on"));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::turnOff()
{
    stateMachine_->postEvent(new StringEvent("turn_off"));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::startCommandMode()
{
    stateMachine_->postEvent(new StringEvent("start"));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::cancelCommandMode()
{
    stateMachine_->postEvent(new StringEvent("cancel"));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::finishCommandMode()
{
    stateMachine_->postEvent(new StringEvent("finish"));
}

// -------------------------------------------------------------------------------------------------
int CoffeeMaker::maxCupsProcessedUntilCleanMode() const
{
    return maxCupsUntilCleanReq;
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doSelfCheck()
{
    if (restBinLevel() < restBinMax
        && cupsProcessed() < maxCupsUntilCleanReq
        && overflowContainerLevel() < overflowMax)
    {
        stateMachine_->postEvent(new StringEvent("check_ok"));
    }

    // internally post all current container levels as events
    stateMachine_->postEvent(new IntegerEvent(waterContainerLevel(), WaterEventType));
    stateMachine_->postEvent(new IntegerEvent(beansContainerLevel(), BeansEventType));
    stateMachine_->postEvent(new IntegerEvent(milkContainerLevel(), MilkEventType));
    stateMachine_->postEvent(new IntegerEvent(restBinLevel(), RestBinEventType));
    stateMachine_->postEvent(new IntegerEvent(overflowContainerLevel(), OverflowEventType));
    stateMachine_->postEvent(new IntegerEvent(cupsProcessed(), CupCountEventType));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::cleanTheMachine() {
    setCupsProcessed(0);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::emptyOverflowContainer() {
    setOverflowLevel(0);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::emptyRestBinContainer() {
    setRestBinLevel(0);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doGrinding(const GrindOptions& grindOptions) {
    stateMachine_->postEvent(new GrindEvent(grindOptions));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doBrew(const WaterOptions& waterOptions) {
    stateMachine_->postEvent(new BrewEvent(waterOptions));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doMilkPrep(const MilkOptions& milkOptions) {
    stateMachine_->postEvent(new PrepMilkEvent(milkOptions));
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doGrinding(int amount, GrindLevel level) {
    doGrinding(GrindOptions{amount, level});
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doBrew(int amount, int temp) {
    doBrew(WaterOptions{amount, temp});
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::doMilkPrep(int amount, int temp, bool foam) {
    doMilkPrep(MilkOptions{amount, temp, foam});
}

// -------------------------------------------------------------------------------------------------
int CoffeeMaker::getMilk(int amount)
{
    const auto current = milkContainerLevel();
    if (amount > current) {
        setMilkContainerLevel(0);
        return current;
    }


    setMilkContainerLevel(current - amount);
    return amount;
}

// -------------------------------------------------------------------------------------------------
int CoffeeMaker::getWater(int amount)
{
    const auto current = waterContainerLevel();
    if (amount > current) {
        setWaterContainerLevel(0);
        return current;
    }


    setWaterContainerLevel(current - amount);
    return amount;
}

// -------------------------------------------------------------------------------------------------
int CoffeeMaker::getBeans(int amount)
{
    const auto current = beansContainerLevel();
    if (amount > current) {
        setBeansContainerLevel(0);
        return current;
    }


    setBeansContainerLevel(current - amount);
    return amount;
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addToBin(int amount) {
    setRestBinLevel(restBinLevel_ + amount);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addToOverflow(int amount) {
    setOverflowLevel(overflowContainerLevel_ + amount);
}

// -------------------------------------------------------------------------------------------------
void CoffeeMaker::addToCupsProcessed(int cups) {
    setCupsProcessed(cupsProcessed_ + cups);
}


// -------------------------------------------------------------------------------------------------
void CoffeeMaker::emptyCoffeeGrounds() {
    addToBin(currentCoffeeGroundAmount_);
    currentCoffeeGroundAmount_ = 0;
    qDebug() << Q_FUNC_INFO;
}

// -------------------------------------------------------------------------------------------------
bool CoffeeMaker::cupDetected() const
{
    return cupDetected_;
}
