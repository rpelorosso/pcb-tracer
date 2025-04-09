#pragma once

#include <type_traits>
#include <QGraphicsItem>

template<typename... Ts>
bool isDynamicCastableToAny(const QGraphicsItem* item) {
    return (... || (dynamic_cast<const Ts*>(item) != nullptr));
}