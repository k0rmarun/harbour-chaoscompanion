#include "event.h"
#include <QDate>
Event::Event(const QString &name, int baseNumber, const QString &baseURL)
{
    mName = name;
    mBaseNumber = baseNumber;
    mBaseURL = baseURL;
}

QString Event::currentURL() const
{
    QDate date = QDate::currentDate();
    return mBaseURL.arg(date.year() - mBaseNumber);
}

QString Event::name() const
{
    return mName;
}


QList<QObject*> generateEventList()
{
    QList<QObject*> eventList;
    eventList.append(
        new Event(
            "Chaos Communication Congress",
            0,
            "https://fahrplan.events.ccc.de/congress/%0/Fahrplan/schedule.json"
        )
    );
    eventList.append(
        new Event(
            "Disco Chaos Communication Congress",
            0,
            "https://fahrplan.events.ccc.de/congress/%0/Lineup/schedule.json"
        )
    );
    eventList.append(
        new Event(
            "Gulasch Programmier Nacht",
            2018-18,
            "https://entropia.de/GPN%0:Fahrplan:JSON?action=raw"
        )
    );
    eventList.append(
        new Event(
            "MRMCD",
            2018-18,
            "https://%0.mrmcd.net/fahrplan/schedule.json"
        )
    );

    return eventList;
}
