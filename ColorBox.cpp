#include "ColorBox.h"
#include "Editor.h"
#include "Config.h"

ColorBox::ColorBox()
{
    // Create a QFrame for the colored box
    m_colorFrame = new QFrame(this);
    m_colorFrame->setFixedSize(60, 25);  // Set the size of the color box

    // Create a QLabel for the text
    m_textLabel = new QLabel("", this);
    m_textLabel->setStyleSheet("color: black;");
    m_textLabel->setAlignment(Qt::AlignCenter);  // Align the text to the center

    // Create a layout for the QFrame to hold the text label
    QVBoxLayout* frameLayout = new QVBoxLayout(m_colorFrame);
    frameLayout->addWidget(m_textLabel, 0, Qt::AlignCenter);  // Center the text label

    // Set the layout for the QFrame
    m_colorFrame->setLayout(frameLayout);

    // Create a main layout for the ColorBox
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_colorFrame);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Set the main layout for the widget
    setLayout(mainLayout);
    setSide(LinkSide::FRONT);
}

void ColorBox::setSide(LinkSide side)
{
    QString text;
    QColor color;

    switch (side)
    {
    case LinkSide::BACK:
        text = "BACK";
        color = Config::instance()->color(Color::BACK);
        break;
    case LinkSide::FRONT:
        text = "FRONT";
        color = Config::instance()->color(Color::FRONT);
        break;
    case LinkSide::WIP:
        text = "WIP";
        color = Config::instance()->color(Color::WIP);
        break;
    case LinkSide::NOTES:
        text = "NOTES";
        color = Config::instance()->color(Color::NOTES);
        break;
    }

    m_textLabel->setText(text);
    qDebug() << "ColorBox set to side" << LinkSideUtils::toString(side);
    m_colorFrame->setStyleSheet(QString("background-color: %1;").arg(color.name()));
}
