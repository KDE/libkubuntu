/*
  Copyright (C) 2010-2012 Harald Sitter <apachelogger@ubuntu.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) version 3, or any
  later version accepted by the membership of KDE e.V. (or its
  successor approved by the membership of KDE e.V.), which shall
  act as a proxy defined in Section 6 of version 3 of the license.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "busyoverlay.h"
#include "ui_busyoverlay.h"

namespace Kubuntu {

class BusyOverlayPrivate
{
public:
    BusyOverlayPrivate(BusyOverlay *q, QWidget *baseWidget);

    void reposition();

    QScopedPointer<Ui::BusyOverlay> ui;
    QWidget *baseWidget;

private:
    BusyOverlay *const q_ptr;
    Q_DECLARE_PUBLIC(BusyOverlay)

    BusyOverlayPrivate() : q_ptr(nullptr) { Q_ASSERT(q_ptr); }
};

BusyOverlayPrivate::BusyOverlayPrivate(BusyOverlay *q, QWidget *baseWidget)
    : ui(new Ui::BusyOverlay)
    , baseWidget(baseWidget)
    , q_ptr(q)
{
    ui->setupUi(q);

    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);

    baseWidget->setEnabled(false);
    baseWidget->installEventFilter(q);
}

void BusyOverlayPrivate::reposition()
{
    Q_Q(BusyOverlay);
    if (!baseWidget->isVisible()) {
        q->hide();
        return;
    }

    // Follow position changes.
    const QPoint topLevelPos = baseWidget->mapTo(q->window(), QPoint(0, 0));
    const QPoint parentPos = q->parentWidget()->mapFrom(q->window(), topLevelPos);
    q->move(parentPos);

    QSize baseSize = baseWidget->size();
    q->resize(baseSize);
}

BusyOverlay::BusyOverlay(QWidget *baseWidget, QWidget *parent)
    : QWidget(parent ? parent : baseWidget)
    , d_ptr(new BusyOverlayPrivate(this, baseWidget))
{
    Q_D(BusyOverlay);

    QPalette p = palette();
    p.setColor(backgroundRole(), QColor(0, 0, 0, 165));
    p.setColor(foregroundRole(), Qt::white);
    setPalette(p);

    setAutoFillBackground(true);

    show();
    d->reposition();
}

BusyOverlay::~BusyOverlay()
{
}

void BusyOverlay::setProgress(int percent)
{
    Q_D(BusyOverlay);
    d->ui->progressBar->setValue(percent);
}

void BusyOverlay::throwAway()
{
    hide();
    deleteLater();
}

bool BusyOverlay::eventFilter(QObject * object, QEvent * event)
{
    Q_D(BusyOverlay);
    if (object == d->baseWidget &&
            (event->type() == QEvent::Move ||
             event->type() == QEvent::Resize ||
             event->type() == QEvent::Show ||
             event->type() == QEvent::Hide ||
             event->type() == QEvent::ParentChange ||
             event->type() == QEvent::ActivationChange)) {
        if (event->type() == QEvent::Show) {
            show(); // force a show when the base is shown
        }
        d->reposition();
    }
    return QWidget::eventFilter(object, event);
}

} // namespace Kubuntu

#include "busyoverlay.moc"
