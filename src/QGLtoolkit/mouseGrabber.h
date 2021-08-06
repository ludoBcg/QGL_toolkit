/****************************************************************************

 Copyright (C) 2002-2014 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.7.1.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License 
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain 
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef QGLVIEWER_MOUSE_GRABBER_H
#define QGLVIEWER_MOUSE_GRABBER_H

//#include "config.h"

#include <QOpenGLWidget>
#include <QList>
#include <QVector>

#include <QEvent>

//#include "camera.h"
//#include "qglviewer.h"

//class QGLViewer;



namespace qglviewer {




class MouseGrabber 
{


    private:

        bool grabsMouse_;

        // Q G L V i e w e r   p o o l
        static QList<MouseGrabber *> MouseGrabberPool_;

        MouseGrabber(const MouseGrabber &);
        MouseGrabber &operator=(const MouseGrabber &);



    public:

        bool isInMouseGrabberPool() const 
        {
            return MouseGrabber::MouseGrabberPool_.contains( const_cast<MouseGrabber *>(this) );
        }

        void addInMouseGrabberPool()
        {
            if (!isInMouseGrabberPool())
                MouseGrabber::MouseGrabberPool_.append(this);
        }

        void removeFromMouseGrabberPool()
        {
            if (isInMouseGrabberPool())
                MouseGrabber::MouseGrabberPool_.removeAll(const_cast<MouseGrabber *>(this));
        }

        void clearMouseGrabberPool(bool autoDelete = false)
        {
            if (autoDelete)
                qDeleteAll(MouseGrabber::MouseGrabberPool_);

            MouseGrabber::MouseGrabberPool_.clear();
        }

        MouseGrabber() : grabsMouse_(false) { addInMouseGrabberPool(); }
  
        virtual ~MouseGrabber() { MouseGrabber::MouseGrabberPool_.removeAll(this); }

        virtual void checkIfGrabsMouse(int x, int y,/* const Camera *const camera*/ glm::vec3 projPos) = 0;

        bool grabsMouse() const { return grabsMouse_; }

        static const QList<MouseGrabber *> &MouseGrabberPool() 
        {
            return MouseGrabber::MouseGrabberPool_;
        }




    protected:
  
        void setGrabsMouse(bool grabs) { grabsMouse_ = grabs; }

        virtual void mousePressEvent(QMouseEvent *const event/*, Camera *const camera*/) 
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);
        }

        virtual void mouseDoubleClickEvent(QMouseEvent *const event/*, Camera *const camera*/) 
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);
        }

        virtual void mouseReleaseEvent(QMouseEvent *const event/*, Camera *const camera*/) 
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);
        }

        virtual void mouseMoveEvent(QMouseEvent *const event/*, Camera *const camera*/) 
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);
        }

        virtual void wheelEvent(QWheelEvent *const event/*, Camera *const camera*/) 
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);
        }






};

} // namespace qglviewer

#endif // QGLVIEWER_MOUSE_GRABBER_H
