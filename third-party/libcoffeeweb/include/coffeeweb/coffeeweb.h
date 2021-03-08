// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#pragma once

#include <QObject>
#include <memory>

class CoffeeWeb : public QObject
{
    Q_OBJECT

public:
    explicit CoffeeWeb(QObject* parent = nullptr);
    ~CoffeeWeb();

    /// Request recipes, returns a request id.
    quint32 requestRecipes(quint32 timeoutMs = 4000, bool forceTimeout = false);

signals:
    /// Emitted when results are ready for a request id,
    /// when an error occured this is visible in the 'return_code' and
    /// 'error_message' properties in the json object.
    void recipesRequestReply(quint32 id, const QString& recipesJson);

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};