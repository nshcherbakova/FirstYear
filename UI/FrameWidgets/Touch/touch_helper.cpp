/*
Gwenview: an image viewer
Copyright 2019 Steffen Hartleib <steffenhartleib@t-online.de>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/
#include "touch_helper.h"

#include <QEvent>
#include <QGraphicsWidget>
#include <QPoint>
#include <QTouchEvent>

namespace Touch_Helper {
QPoint simpleTapPosition(QEvent *event) {
  if (event->type() == QEvent::TouchEnd) {
    event->accept();
    if (touchStationary(event)) {
      return simpleTouchPosition(event);
    }
  }
  return QPoint(-1, -1);
}

QPoint simpleTouchPosition(QEvent *event, int at) {
  if (auto touchEvent = static_cast<QTouchEvent *>(event)) {
    if (touchEvent->points().size() > at) {
      return touchEvent->points().at(at).position().toPoint();
    }
  }
  return QPoint(-1, -1);
}

bool touchStationary(QEvent *event) {
  if (auto touchEvent = static_cast<QTouchEvent *>(event)) {
    const QPointF distance = touchEvent->points().first().pressPosition() -
                             touchEvent->points().first().position();
    if (distance.manhattanLength() <= Touch::wiggleRoomForTap) {
      return true;
    }
  }
  return false;
}

} // namespace Touch_Helper
