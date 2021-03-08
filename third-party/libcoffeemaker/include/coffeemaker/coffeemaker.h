// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#pragma once

#include <QObject>
#include <QString>

#include <memory>

class QSettings;
class QState;
class QStateMachine;

class CoffeeMaker : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(GrindLevel)

    Q_PROPERTY(State currentState READ currentState NOTIFY currentStateChanged)
    Q_PROPERTY(int waterContainerLevel READ waterContainerLevel NOTIFY waterContainerLevelChanged)
    Q_PROPERTY(int milkContainerLevel READ milkContainerLevel NOTIFY milkContainerLevelChanged)
    Q_PROPERTY(int beansContainerLevel READ beansContainerLevel NOTIFY beansContainerLevelChanged)
    Q_PROPERTY(int restBinLevel READ restBinLevel NOTIFY restBinLevelChanged)
    Q_PROPERTY(int overflowContainerLevel READ overflowContainerLevel NOTIFY overflowContainerLevelChanged)

    Q_PROPERTY(bool cupDetected READ cupDetected NOTIFY cupDetectedChanged)
    Q_PROPERTY(int cupsProcessed READ cupsProcessed NOTIFY cupsProcessedChanged)

public:
    enum class State {
        Off, SelfCheck, BinFull, OverflowFull, CleaningRequired, StandBy,
        CommandMode, Grinding, BeansEmpty, Brewing, WaterEmpty, PrepMilk, MilkEmpty,
        Unknown
    };

    enum class GrindLevel {
        ExtraCourse, Course, MediumCoarse, Medium, MediumFine, Fine, ExtraFine
    };

    struct GrindOptions {
        int beansInGram = 0;
        GrindLevel grindLevel = GrindLevel::Medium;
    };

    struct WaterOptions {
        int waterMl = 0;
        int temperatureC = 95;
    };

    struct MilkOptions {
        int milkMl = 0;
        int temperatureC = 85;
        bool foam = false;
    };

    explicit CoffeeMaker(QObject* parent = nullptr);

    /// Returns if the machine is powered
    Q_INVOKABLE bool isPoweredOn() const { return currentState() != State::Off && currentState() != State::Unknown; }

    /// Turn on the machine
    Q_INVOKABLE void turnOn();

    /// Turn off the machine (hard-reset, like long pressing a computer power button)
    Q_INVOKABLE void turnOff();

    /// Start command mode
    Q_INVOKABLE void startCommandMode();

    /// Cancel command mode, returns to stand by
    Q_INVOKABLE void cancelCommandMode();

    /// Finish command mode, returns to stand by
    Q_INVOKABLE void finishCommandMode();

    /// Do a cleaning (also resets the cups processed)
    Q_INVOKABLE void cleanTheMachine();

    /// Empty the overflow container
    Q_INVOKABLE void emptyOverflowContainer();

    /// Empty the coffee rest bin
    Q_INVOKABLE void emptyRestBinContainer();

    /// Add the given amount of milk in ml to the milk container,
    Q_INVOKABLE void addMilkToContainer(int milkMl);

    /// Add the given amount of water in ml to the water container,
    Q_INVOKABLE void addWatertoContainer(int waterMl);

    /// Add the given amount of beans to the beans container
    Q_INVOKABLE void addBeanstoContainer(int beansGram);

    /// When in command state, start grinding the given options
    Q_INVOKABLE void doGrinding(const GrindOptions& grindOptions);
    Q_INVOKABLE void doGrinding(int amount, GrindLevel level);

    /// When in command state, start brewing using the given options
    Q_INVOKABLE void doBrew(const WaterOptions& waterOptions);
    Q_INVOKABLE void doBrew(int amount, int temp);

    /// When in command state, start preparing and adding milk with the given options
    Q_INVOKABLE void doMilkPrep(const MilkOptions& milkOptions);
    Q_INVOKABLE void doMilkPrep(int amount, int temp, bool foam = false);

    /// Returns current state
    State currentState() const;

    /// Returns the maximum possible water in the water container in ml
    Q_INVOKABLE int waterContainerMax() const;

    /// Returns the maximum possible milk in the milk container in ml
    Q_INVOKABLE int milkContainerMax() const;

    /// Returns the maximum possible beans in the beans container in gram
    Q_INVOKABLE int beansContainerMax() const;

    /// Returns the maximum fill level of the coffee rest bin
    Q_INVOKABLE int restBinLevelMax() const;

    /// Returns the maximum fill level of the overflow container.
    Q_INVOKABLE int overflowContainerMax() const;

    /// Maximum of processed cups until cleaning is necessary
    Q_INVOKABLE int maxCupsProcessedUntilCleanMode() const;

    /// Physically place a cup at the output tray
    Q_INVOKABLE void placeCup();

    /// Physically remove a cup from the output tray (if one is there)
    Q_INVOKABLE void removeCup();

    /// Returns the remaining water in ml
    int waterContainerLevel() const { return waterContainerLevel_; }

    /// Returns the available remaining beans in gram
    int beansContainerLevel() const { return beansContainerLevel_; }

    /// Returns the remaining milk available in ml
    int milkContainerLevel() const { return milkContainerLevel_; }

    /// Returns the fill level of the overflow container
    int overflowContainerLevel() const { return overflowContainerLevel_; }

    /// Returns the current coffee rest bin level
    int restBinLevel() const { return restBinLevel_; }

    /// Returns the number of coffees made (cups) since last cleaning process
    int cupsProcessed() const { return cupsProcessed_; }

    /// Returns if a cup is detected in the output tray
    bool cupDetected() const;

signals:
    void cupDetectedChanged(bool cupInTray);
    void milkContainerLevelChanged(int milkMl);
    void waterContainerLevelChanged(int waterMl);
    void beansContainerLevelChanged(int beansGram);
    void restBinLevelChanged(int lvl);
    void overflowContainerLevelChanged(int lvl);
    void cupsProcessedChanged(int cups);

    void currentStateChanged(State state);

private:
    void setBeansContainerLevel(int level);
    void setWaterContainerLevel(int level);
    void setMilkContainerLevel(int level);
    void setRestBinLevel(int level);
    void setOverflowLevel(int level);
    void setCupsProcessed(int cups);
    void doSelfCheck();

    int getBeans(int amount);
    int getWater(int amount);
    int getMilk(int amount);

    void addToBin(int amount);
    void addToOverflow(int amount);
    void addToCupsProcessed(int cups);

    void emptyCoffeeGrounds();

private:
    QSettings* settings_ = nullptr;
    QStateMachine* stateMachine_ = nullptr;

    QState* stateOff_  = nullptr;
    QState* stateSelfCheck_ = nullptr;
    QState* stateBinFull_ = nullptr;
    QState* stateOverflowFull1_ = nullptr;
    QState* stateCleaningReq_ = nullptr;
    QState* stateStandBy_ = nullptr;
    QState* stateCommandMode_ = nullptr;
    QState* stateGrinding_ = nullptr;
    QState* stateBeansEmpty_ = nullptr;
    QState* stateBrewing_ = nullptr;
    QState* stateWaterEmpty_ = nullptr;
    QState* statePrepMilk_ = nullptr;
    QState* stateMilkEmpty_ = nullptr;

    int milkContainerLevel_ = 0;
    int waterContainerLevel_ = 0;
    int beansContainerLevel_ = 0;
    int restBinLevel_ = 0;
    int overflowContainerLevel_ = 0;
    int cupsProcessed_ = 0;
    bool cupDetected_ = false;

    std::shared_ptr<GrindOptions> grindOptions_;
    std::shared_ptr<WaterOptions> waterOptions_;
    std::shared_ptr<MilkOptions> milkOptions_;

    int currentCoffeeGroundAmount_ = 0;
};

Q_DECLARE_METATYPE(CoffeeMaker::State)
Q_DECLARE_METATYPE(CoffeeMaker::GrindLevel)