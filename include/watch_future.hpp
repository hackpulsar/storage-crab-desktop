#pragma once

#include <functional>

#include <QObject>
#include <QFutureWatcher>
#include <QtConcurrent>

#include "api/requests.hpp"

// Wraps an API call into QtConcurrent::run returning a QFuture.
template<typename Fn>
auto dispatch(Fn&& f) {
    return QtConcurrent::run(std::forward<Fn>(f));
}

// Helper function for setting response handlers,
// when making concurrent requests.
template <typename FutureT, typename SuccessFn, typename FailureFn>
void watchFuture(
    QObject* parent,
    QFuture<FutureT> future,
    SuccessFn onSuccess,
    FailureFn onFailure
)
{
    auto* watcher = new QFutureWatcher<FutureT>(parent);

    QObject::connect(
        watcher, &QFutureWatcher<FutureT>::finished,
        parent, [=] {
            auto result = watcher->result();
            
            if (result.ok)
                onSuccess(result);
            else
                onFailure(result);

            watcher->deleteLater();
        }
    );

    watcher->setFuture(future);
}
