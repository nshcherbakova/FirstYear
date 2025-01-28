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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301,
USA.

*/
#ifndef DOUBLETAP_H
#define DOUBLETAP_H

#include <QGesture>
#include <QGestureRecognizer>

struct DoubleTapRecognizerPrivate;

class DoubleTap : public QGesture {

  Q_PROPERTY(QPointF position READ position WRITE setPosition)
public:
  explicit DoubleTap(QObject *parent = nullptr);

public:
  QPointF position() const;
  void setPosition(const QPointF &pos);

private:
  QPointF pos;
};

class DoubleTapRecognizer : public QGestureRecognizer {
public:
  explicit DoubleTapRecognizer();
  ~DoubleTapRecognizer() override;

private:
  DoubleTapRecognizerPrivate *d;

  QGesture *create(QObject *) override;
  Result recognize(QGesture *, QObject *, QEvent *) override;
};

#endif /* DOUBLETAP_H */
