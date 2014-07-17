/*
  Copyright (C) 2010 Harald Sitter <apachelogger@ubuntu.com>

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

#ifndef KUBUNTU_BUSYOVERLAY_H
#define KUBUNTU_BUSYOVERLAY_H

#include "export.h"

#include <QWidget>

namespace Kubuntu {

class BusyOverlayPrivate;
/**
 * @brief A very busy overlay.
 * BusyOverlay is a QWidget that positions itself on top of another QWidget
 * preventing access to it while still being slighly visible.
 * This class also features a progressbar to indicate progress of the busyness.
 *
 * BusyOverlay is meant to be as unintrusive as possible. One simply constructs
 * it with the base and sets up a suitable connection for setProgress and throwAway.
 * Everything else is handled by the overlay automatically. It will also take
 * care of deleting itself again.
 */
class KUBUNTU_EXPORT BusyOverlay : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructs a new busy overlay. This automatically disables the baseWidget
     * and overlays the BusyOverlay on top.
     *
     * \warning BusyOverlays must be allocated using \c new on the heap as
     *          they are meant to be non-intrusive and among other things delete
     *          themself.
     *
     * @param baseWidget the widget to overlay on top
     * @param parent the qobject lifetime parent (if 0 the base will be the parent)
     */
    explicit BusyOverlay(QWidget *baseWidget, QWidget *parent = 0);
    virtual ~BusyOverlay();

public slots:
    /** \param percent Set busy progress to percent (0-100) */
    void setProgress(int percent);

    /** Discards the BusyOverlay. This results in hide() and deleteLater() */
    void throwAway();

protected:
    bool eventFilter(QObject *object, QEvent *event) final override;

private:
    const QScopedPointer<BusyOverlayPrivate> d_ptr;
    Q_DECLARE_PRIVATE(BusyOverlay)
};

} // namespace Kubuntu

#endif // KUBUNTU_BUSYOVERLAY_H
