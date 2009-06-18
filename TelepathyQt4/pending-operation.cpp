/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <TelepathyQt4/PendingOperation>

#define IN_TELEPATHY_QT4_HEADER
#include "simple-pending-operations.h"
#undef IN_TELEPATHY_QT4_HEADER

#include "TelepathyQt4/_gen/pending-operation.moc.hpp"
#include "TelepathyQt4/_gen/simple-pending-operations.moc.hpp"

#include "TelepathyQt4/debug-internal.h"

#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QTimer>

namespace Tp
{

struct PendingOperation::Private
{
    Private() : finished(false)
    {
    }

    QString errorName;
    QString errorMessage;
    bool finished;
};

PendingOperation::PendingOperation(QObject *parent)
    : QObject(parent),
      mPriv(new Private())
{
}

PendingOperation::~PendingOperation()
{
    if (!mPriv->finished) {
        warning() << this <<
            "still pending when it was deleted - finished will "
            "never be emitted";
    }

    delete mPriv;
}

void PendingOperation::emitFinished()
{
    Q_ASSERT(mPriv->finished);
    emit finished(this);
    deleteLater();
}

void PendingOperation::setFinished()
{
    if (mPriv->finished) {
        if (mPriv->errorName.isEmpty()) {
            warning() << this << "trying to finish with success, but already"
                " failed with" << errorName() << ":" << errorMessage();
        } else {
            warning() << this << "trying to finish with success, but already"
                " succeeded";
        }
        return;
    }

    mPriv->finished = true;
    Q_ASSERT(isValid());
    QTimer::singleShot(0, this, SLOT(emitFinished()));
}

void PendingOperation::setFinishedWithError(const QString &name,
        const QString &message)
{
    if (mPriv->finished) {
        if (mPriv->errorName.isEmpty()) {
            warning() << this << "trying to fail with" << name <<
                "but already failed with" << errorName() << ":" <<
                errorMessage();
        } else {
            warning() << this << "trying to fail with" << name <<
                "but already succeeded";
        }
        return;
    }

    if (name.isEmpty()) {
        warning() << this << "should be given a non-empty error name";
        mPriv->errorName = "org.freedesktop.Telepathy.Qt4.ErrorHandlingError";
    } else {
        mPriv->errorName = name;
    }

    mPriv->errorMessage = message;
    mPriv->finished = true;
    Q_ASSERT(isError());
    QTimer::singleShot(0, this, SLOT(emitFinished()));
}

void PendingOperation::setFinishedWithError(const QDBusError &error)
{
    setFinishedWithError(error.name(), error.message());
}

bool PendingOperation::isValid() const
{
    return (mPriv->finished && mPriv->errorName.isEmpty());
}

bool PendingOperation::isFinished() const
{
    return mPriv->finished;
}

bool PendingOperation::isError() const
{
    return (mPriv->finished && !mPriv->errorName.isEmpty());
}

QString PendingOperation::errorName() const
{
    return mPriv->errorName;
}

QString PendingOperation::errorMessage() const
{
    return mPriv->errorMessage;
}


PendingVoidMethodCall::PendingVoidMethodCall(QObject *proxy,
        QDBusPendingCall call)
  : PendingOperation(proxy)
{
    connect(new QDBusPendingCallWatcher(call),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(watcherFinished(QDBusPendingCallWatcher*)));
}

void PendingVoidMethodCall::watcherFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError()) {
        setFinishedWithError(watcher->error());
    } else {
        setFinished();
    }
}

} // Tp
