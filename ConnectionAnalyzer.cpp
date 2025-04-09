#include <QGraphicsScene>
#include <QDebug>
#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include "ConnectionAnalyzer.h"
#include "Editor.h"

void ConnectionAnalyzer::showResultDialog(const QString& result)
{
    QDialog dialog(Editor::instance());
    dialog.setWindowTitle("Connection Analysis Results");
    dialog.setMinimumSize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    layout->addWidget(textEdit);

    QPushButton* closeButton = new QPushButton("Close", &dialog);
    layout->addWidget(closeButton);
    QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    textEdit->setText(result);

    dialog.exec();
}


void ConnectionAnalyzer::getConnections()
{
    // Map to store each graph_id and its associated Pad nodes
    QMap<int, QList<Pad*>> graphIdToPads;

    auto scene = Editor::instance()->getScene();

    for (QGraphicsItem* item : scene->items()) {
        Link* link = dynamic_cast<Link*>(item);
        if (link) {
            int graphId = link->m_graphId;

            // Get nodes from each link's fromNode() and toNode()
            Node* fromNode = link->fromNode();
            Node* toNode = link->toNode();

            // Check if the nodes are of class Pad, and add them to the list for the graph_id
            Pad* fromPad = dynamic_cast<Pad*>(fromNode);
            if (fromPad && !graphIdToPads[graphId].contains(fromPad)) {
                graphIdToPads[graphId].append(fromPad);
            }       

            Pad* toPad = dynamic_cast<Pad*>(toNode);
            if (toPad && !graphIdToPads[graphId].contains(toPad)) {
                graphIdToPads[graphId].append(toPad);
            }
        }
    }

    QString resultText;
    for (auto it = graphIdToPads.constBegin(); it != graphIdToPads.constEnd(); ++it) {
        int graphId = it.key();
        const QList<Pad*>& pads = it.value();

        QStringList padNames;
        for (const Pad* pad : pads) {
            padNames << pad->m_name;
        }

        QString result = padNames.join(" | ");
        resultText += result + "\n";
    }

    showResultDialog(resultText);

}