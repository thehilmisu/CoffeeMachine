// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#include "coffeeweb.h"

#include <QFile>
#include <QRandomGenerator>
#include <QTimer>
#include <QTextStream>

#include <map>

// -------------------------------------------------------------------------------------------------
namespace {
    struct RequestTimers
    {
        RequestTimers(QTimer* replyTimer, QTimer* timeoutTimer)
            : replyTimer_(replyTimer), timeoutTimer_(timeoutTimer) {}

        ~RequestTimers() {
            if (replyTimer_) {
                replyTimer_->stop();
                replyTimer_->deleteLater();
            }
            if (timeoutTimer_) {
                timeoutTimer_->stop();
                timeoutTimer_->deleteLater();
            }
        }

        QTimer* replyTimer_ = nullptr;
        QTimer* timeoutTimer_ = nullptr;
    };



}

// -------------------------------------------------------------------------------------------------
struct CoffeeWeb::Impl
{
    Impl(CoffeeWeb* parent)
        : parent_(parent)
        , nextRequestId_(QRandomGenerator::global()->generate()) // random request id at start.
    {
    }

    CoffeeWeb* const parent_ = nullptr;
    quint32 nextRequestId_ = 0;
    std::map<uint32_t, std::unique_ptr<RequestTimers>> requests_;
};

// -------------------------------------------------------------------------------------------------
CoffeeWeb::CoffeeWeb(QObject* parent)
    : QObject(parent)
    , impl_(std::make_unique<Impl>(this))
{
    Q_INIT_RESOURCE(json);
}

// -------------------------------------------------------------------------------------------------
CoffeeWeb::~CoffeeWeb() = default;

// -------------------------------------------------------------------------------------------------
quint32 CoffeeWeb::requestRecipes(quint32 timeoutMs, bool forceTimeout)
{
    const auto requestId = impl_->nextRequestId_++;

    const auto timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->setInterval(timeoutMs);

    // fake a random reply time, that sometimes is also longer than the timeout time
    // in milliseconds - therefore the request would time out..
    const auto replyTimer = new QTimer(this);
    replyTimer->setSingleShot(true);
    if (forceTimeout) {
        replyTimer->setInterval(timeoutMs + 1000);
    } else {
        replyTimer->setInterval(QRandomGenerator::global()->bounded(
                                    timeoutMs/4, timeoutMs + timeoutMs / 8));
    }

    connect(timeoutTimer, &QTimer::timeout, this,
    [this, requestId]() {
        const auto it = impl_->requests_.find(requestId);
        if (it != impl_->requests_.end()) {
            impl_->requests_.erase(it);
        }
        static const QString timeoutReply(R"({"return_code":408, "error_message": "Request timed out."}})");
        emit recipesRequestReply(requestId, timeoutReply);
    });

    connect(replyTimer, &QTimer::timeout, this,
    [this, requestId]() {
        const auto it = impl_->requests_.find(requestId);
        if (it != impl_->requests_.end()) {
            impl_->requests_.erase(it);
        }
        static const QString requestReply(R"({"return_code":200, "error_message": ""}})");
        static const QString requestReplyOk = []()
        {
            QFile file(":/recipes.json");
            if (!file.open(QFile::ReadOnly | QFile::Text)) {
                return QString(R"({"return_code":500, "error_message": "Could not read file."}})");
            }
            QTextStream in(&file);
            return in.readAll();
        }();

        emit recipesRequestReply(requestId, requestReplyOk);
    });

    impl_->requests_.emplace(requestId, std::make_unique<RequestTimers>(replyTimer, timeoutTimer));
    replyTimer->start();
    timeoutTimer->start();

    return requestId;
}
