#pragma once

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include "Link.h"

class ColorBox : public QWidget
{
    Q_OBJECT

public:
    ColorBox();
    void setSide(LinkSide side);

private:
    QFrame* m_colorFrame;
    QLabel* m_textLabel;
};