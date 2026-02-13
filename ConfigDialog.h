#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QMap>
#include "enums.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    QVariantMap getConfigData() const;

private slots:
    void chooseColor();

private:
    QLineEdit *m_inputField;
    QSpinBox *m_trackWidthSpinBox;
    QDoubleSpinBox *m_padRadiusSpinBox;
    QSpinBox *m_nodeSizeSpinBox;
    QMap<Color, QPushButton*> m_colorButtons;
    QMap<Color, QString> m_buttonColors;
    QPushButton *m_nodeColorButton;
    QString m_nodeColor;
};

#endif // CONFIGDIALOG_H