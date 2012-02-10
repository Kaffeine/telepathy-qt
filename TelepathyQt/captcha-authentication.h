/**
 * This file is part of TelepathyQt
 *
 * @copyright Copyright (C) 2012 Collabora Ltd. <http://www.collabora.co.uk/>
 * @license LGPL 2.1
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

#ifndef _TelepathyQt_captcha_authentication_h_HEADER_GUARD_
#define _TelepathyQt_captcha_authentication_h_HEADER_GUARD_

// FIXME: Uncomment once in tp-qt
//#ifndef IN_TP_QT_HEADER
//#error IN_TP_QT_HEADER
//#endif

#include <TelepathyQt/Channel>

#include <TelepathyQt/Global>
#include <TelepathyQt/SharedPtr>

#include "constants.h"
#include "types.h"

namespace Tp
{

class PendingCaptchas;
class ChannelFuture;

class CaptchaAuthentication : public Tp::Object
{
    Q_OBJECT
    Q_DISABLE_COPY(CaptchaAuthentication)

public:
    enum ChallengeType {
        UnknownChallenge = 0,
        OCRChallenge = 1,
        AudioRecognitionChallenge = 2,
        PictureQuestionChallenge = 4,
        PictureRecognitionChallenge = 8,
        TextQuestionChallenge = 16,
        SpeechQuestionChallenge = 32,
        SpeechRecognitionChallenge = 64,
        VideoQuestionChallenge = 128,
        VideoRecognitionChallenge = 256
    };
    typedef QFlags<ChallengeType> ChallengeTypes;

    explicit CaptchaAuthentication(const ChannelPtr &parent);
    virtual ~CaptchaAuthentication();

    bool canRetry() const;
    Tp::CaptchaStatus status() const;

    QString lastError() const;
    QVariantMap lastErrorDetails() const;

    Tp::PendingCaptchas *requestCaptchas(const QStringList &preferredMimeTypes = QStringList(),
            ChallengeTypes preferredTypes = ~ChallengeTypes(UnknownChallenge));
    Tp::PendingOperation *answer(uint id, const QString &answer);
    Tp::PendingOperation *answer(const Tp::CaptchaAnswers &response);

Q_SIGNALS:
    void statusChanged(Tp::CaptchaStatus status);

public Q_SLOTS:
    Tp::PendingOperation *cancel(Tp::CaptchaCancelReason reason,
            const QString &message = QString());

protected:

private Q_SLOTS:
    TP_QT_NO_EXPORT void onPropertiesChanged(const QVariantMap &changedProperties,
            const QStringList &invalidatedProperties);

private:
    friend class ChannelFuture;
    friend class PendingCaptchas;

    struct Private;
    friend struct Private;
    Private *mPriv;
};

} // namespace Tp

Q_DECLARE_OPERATORS_FOR_FLAGS(Tp::CaptchaAuthentication::ChallengeTypes)

#endif
