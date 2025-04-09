#ifndef CONNECTIONANALYZER_H
#define CONNECTIONANALYZER_H

#include <QMap>
#include <QList>
#include <QString>
#include "Link.h"
#include "Component.h"

class QGraphicsScene;

class ConnectionAnalyzer
{
public:
    static void getConnections();

private:
    ConnectionAnalyzer() = delete;  // Prevent instantiation
    static void showResultDialog(const QString& result);
};

#endif // CONNECTIONANALYZER_H