/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQuick>
#include <sailfishapp.h>
#include "loader.h"
#include "event.h"
#include "conferenceeventsortingorder.h"
#include "staticloader.h"
#include <unistd.h>
#include <grp.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
    // Get elevated rights to access calendar
    setuid(getpwnam("nemo")->pw_uid);
    setgid(getgrnam("privileged")->gr_gid);

    // SailfishApp::main() will display "qml/template.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    // Set up QML engine.
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> v(SailfishApp::createView());

    // If you wish to publish your app on the Jolla harbour, it is recommended
    // that you prefix your internal namespaces with "harbour.".
    //
    // For details see:
    // https://harbour.jolla.com/faq#1.5.0
    qmlRegisterSingletonType<StaticLoader>("harbour.companion", 1, 0, "StaticLoader", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        return new StaticLoader();
    });
    qmlRegisterType<Loader>("harbour.companion", 1, 0, "Loader");
    qmlRegisterType<Conference>("harbour.companion", 1, 0, "Conference");
    qmlRegisterType<ConferenceDay>("harbour.companion", 1, 0, "ConferenceDay");
    qmlRegisterType<ConferenceEvent>("harbour.companion", 1, 0, "ConferenceEvent");
    qmlRegisterType<ConferenceEventSortingOrder>("harbour.companion", 1, 0, "SortingOrder");

    QList<QObject*> eventList = generateEventList();
    QQmlContext* ctxt = v->rootContext();
    ctxt->setContextProperty("eventList", QVariant::fromValue(eventList));

    // Start the application.
    v->setSource(SailfishApp::pathTo("qml/companion.qml"));
    v->show();
    return app->exec();
}
