#include "ConfigDialog.h"
#include "Config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColorDialog>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Configuration");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Track Width input
    QHBoxLayout *trackWidthLayout = new QHBoxLayout();
    QLabel *trackWidthLabel = new QLabel("Track Width:");
    m_trackWidthSpinBox = new QSpinBox();
    m_trackWidthSpinBox->setRange(0, 100);
    m_trackWidthSpinBox->setValue(Config::instance()->m_linkWidth);
    trackWidthLayout->addWidget(trackWidthLabel);
    trackWidthLayout->addWidget(m_trackWidthSpinBox);
    mainLayout->addLayout(trackWidthLayout);

    // Pad Radius input
    QHBoxLayout *padRadiusLayout = new QHBoxLayout();
    QLabel *padRadiusLabel = new QLabel("Pad Radius:");
    m_padRadiusSpinBox = new QDoubleSpinBox();
    m_padRadiusSpinBox->setRange(0.0, 100.0);
    m_padRadiusSpinBox->setSingleStep(0.1);
    m_padRadiusSpinBox->setValue(Config::instance()->m_padSize);
    padRadiusLayout->addWidget(padRadiusLabel);
    padRadiusLayout->addWidget(m_padRadiusSpinBox);
    mainLayout->addLayout(padRadiusLayout);

    // Color pickers
    QList<Color> colors = {Color::FRONT, Color::BACK, Color::WIP, Color::HIGHLIGHTED, Color::NODE};
    for (const auto &color : colors) {
        QHBoxLayout *colorLayout = new QHBoxLayout();
        QLabel *colorLabel = new QLabel(QString("%1 Color:").arg(ColorUtils::toString(color)));
        QPushButton *colorButton = new QPushButton("Choose Color");
        QString defaultColor = Config::instance()->color(color);
        colorButton->setStyleSheet(QString("background-color: %1;").arg(defaultColor));
        colorButton->setProperty("color", QVariant::fromValue(color));
        connect(colorButton, &QPushButton::clicked, this, &ConfigDialog::chooseColor);
        colorLayout->addWidget(colorLabel);
        colorLayout->addWidget(colorButton);
        mainLayout->addLayout(colorLayout);
        m_colorButtons[color] = colorButton;
        m_buttonColors[color] = defaultColor;
    }

    // OK and Cancel buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void ConfigDialog::chooseColor()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QColor currentColor = button->palette().button().color();
    QColor pickedColor = QColorDialog::getColor(currentColor, this);
    if (pickedColor.isValid()) {
        button->setStyleSheet(QString("background-color: %1;").arg(pickedColor.name()));        
            auto color = button->property("color").value<Color>();
            m_buttonColors[color] = pickedColor.name();
    }
}

QVariantMap ConfigDialog::getConfigData() const
{
    QVariantMap data;
    data["link_width"] = m_trackWidthSpinBox->value();
    data["pad_size"] = m_padRadiusSpinBox->value();
    
    QVariantMap colors;

    for (auto it = m_buttonColors.begin(); it != m_buttonColors.end(); ++it) {
        colors[ColorUtils::toString(it.key())] = it.value();
    }
    data["colors"] = colors;
    return data;
}