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

#ifndef QGLVIEWER_QGLVIEWER_H
#define QGLVIEWER_QGLVIEWER_H


#include <iostream>
#include <math.h>


#include <QTimer>
#include <QClipboard>
#include <QGL>
#include <QMap>
#include <QTime>
#include <QString>
#include <QOpenGLWidget>
#include <QKeyEvent>



#include "camera.h"

class QTabWidget;

/*namespace qglviewer 
{

    //class Camera;

} // namespace qglviewer*/

class QGLViewer : public QOpenGLWidget 
{
    Q_OBJECT

    public:

 
    

    private :     

        // M o u s e   a c t i o n s
        struct MouseActionPrivate 
        {
            qglviewer::CameraFrame::MouseHandler handler;
            qglviewer::CameraFrame::MouseAction action;
            bool withConstraint;
        };

        // M o u s e   b i n d i n g s
        struct MouseBindingPrivate 
        {
            const Qt::KeyboardModifiers modifiers;
            const Qt::MouseButton button;
            const Qt::Key key;

            MouseBindingPrivate(Qt::KeyboardModifiers m, Qt::MouseButton b, Qt::Key k)
            : modifiers(m), button(b), key(k) {}

            // This sort order is used in mouseString() to display sorted mouse bindings
            bool operator<(const MouseBindingPrivate &mbp) const 
            {
                if (key != mbp.key)
                    return key < mbp.key;
                if (modifiers != mbp.modifiers)
                    return modifiers < mbp.modifiers;
                return button < mbp.button;
            }
        };

        // W h e e l   b i n d i n g s
        struct WheelBindingPrivate 
        {
            const Qt::KeyboardModifiers modifiers;
            const Qt::Key key;

            WheelBindingPrivate(Qt::KeyboardModifiers m, Qt::Key k)
            : modifiers(m), key(k) {}

            // This sort order is used in mouseString() to display sorted wheel bindings
            bool operator<(const WheelBindingPrivate &wbp) const 
            {
                if (key != wbp.key)
                    return key < wbp.key;
                return modifiers < wbp.modifiers;
            }
        };

        // C l i c k   b i n d i n g s
        struct ClickBindingPrivate 
        {
            const Qt::KeyboardModifiers modifiers;
            const Qt::MouseButton button;
            const bool doubleClick;
            const Qt::MouseButtons
            buttonsBefore; // only defined when doubleClick is true
            const Qt::Key key;

            ClickBindingPrivate(Qt::KeyboardModifiers m, Qt::MouseButton b, bool dc, Qt::MouseButtons bb, Qt::Key k)
            : modifiers(m), button(b), doubleClick(dc), buttonsBefore(bb), key(k) {}

            // This sort order is used in mouseString() to display sorted mouse bindings
            bool operator<(const ClickBindingPrivate &cbp) const 
            {
                if (key != cbp.key)
                    return key < cbp.key;
                if (buttonsBefore != cbp.buttonsBefore)
                    return buttonsBefore < cbp.buttonsBefore;
                if (modifiers != cbp.modifiers)
                    return modifiers < cbp.modifiers;
                if (button != cbp.button)
                    return button < cbp.button;
                return doubleClick != cbp.doubleClick;
            }
        };

        QMap<MouseBindingPrivate, MouseActionPrivate> mouseBinding_;
        QMap<WheelBindingPrivate, MouseActionPrivate> wheelBinding_;
        QMap<ClickBindingPrivate, qglviewer::CameraFrame::ClickAction> clickBinding_;
        Qt::Key currentlyPressedKey_;

        // C a m e r a
        qglviewer::Camera *camera_;



    public:
  
        // GETTERS ------------------------------------------------------------------

        qglviewer::Camera  *camera() const { return camera_; }
        double sceneRadius() const { return camera()->sceneRadius(); }
        glm::vec3 sceneCenter() const { return camera()->sceneCenter(); }



        // SETTERS ------------------------------------------------------------------

    public Q_SLOTS:

        virtual void setSceneRadius(double radius) 
        {
            camera()->setSceneRadius(radius);
        }

 
        virtual void setSceneCenter(const glm::vec3 &center) 
        {
            camera()->setSceneCenter(center);
        }

  
        void setSceneBoundingBox(const glm::vec3 &min, const glm::vec3 &max) 
        {
            camera()->setSceneBoundingBox(min, max);
            
        }



    private:


        void setCamera(qglviewer::Camera  *const camera) 
        {
            if (!camera)
                return;

            camera->setSceneRadius(this->sceneRadius());
            camera->setSceneCenter(this->sceneCenter());
            std::cout<<" --------------------- screenWidth_ = "<< width() <<" screenHeight_ "<< height() <<std::endl; // wrong here
            camera->setScreenWidthAndHeight(width(), height());

            camera_ = camera;

        }



        // CONSTRUCTORS ------------------------------------------------------------------

        // Set parameters to their default values. Called by the constructors.
        void defaultConstructor() 
        { 
            setFocusPolicy(Qt::StrongFocus); 

            camera_ = new qglviewer::Camera();
            this->setSceneRadius(1.0);
            this->setSceneCenter( glm::vec3(0.0f) );
            showEntireScene();
            setCamera( this->camera() ); // force to use = operator

            setAttribute(Qt::WA_NoSystemBackground);
        }


    public:

        explicit QGLViewer(QWidget *parent = 0, Qt::WindowFlags flags = Qt::WindowFlags())
        : QOpenGLWidget(parent, flags) 
        {
            defaultConstructor();
        }

        explicit QGLViewer(QWidget *parent, const QGLWidget *shareWidget, Qt::WindowFlags flags = 0)
        : QOpenGLWidget(parent, flags) 
        {
            Q_UNUSED(shareWidget);
            defaultConstructor();
        }

        explicit QGLViewer(QGLContext *context, QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags flags = 0)
        : QOpenGLWidget(parent, flags) 
        {
            Q_UNUSED(context);
            Q_UNUSED(shareWidget);
            defaultConstructor();
        }


        explicit QGLViewer(const QGLFormat &format, QWidget *parent = 0, const QGLWidget *shareWidget = 0, Qt::WindowFlags flags = 0)
        : QOpenGLWidget(parent, flags)
        {
            Q_UNUSED(format);
            Q_UNUSED(shareWidget);
            defaultConstructor();
        }


        virtual ~QGLViewer(){}


             

      
        // RENDER ------------------------------------------------------------------
   
    public:

        //virtual void update() {}


    public Q_SLOTS:

        virtual void resize(int width, int height) {}
        virtual void help() {}
        virtual void aboutQGLViewer() {}

        void showEntireScene() 
        {
            camera()->showEntireScene();
            this->update();
        }


    protected:

        virtual void resizeGL(int width, int height) 
        {
            QOpenGLWidget::resizeGL(width, height);
            std::cout<<" screenWidth_ = "<< this->width() <<" screenHeight_ "<< this->height() <<std::endl;
            glViewport(0, 0, GLint(width), GLint(height));
            camera()->setScreenWidthAndHeight(width, height);
        }

        virtual void init() {}

        virtual void initializeGL() { init(); }


        virtual void draw() {}

        virtual void paintGL() { draw(); }



        // EVENTS ------------------------------------------------------------------
 
    protected:

        virtual void mousePressEvent(QMouseEvent * _e)
        {
            const MouseBindingPrivate mbp(_e->modifiers(), _e->button(), currentlyPressedKey_);

            MouseActionPrivate map = mouseBinding_[mbp];

            map.handler =  qglviewer::CameraFrame::MouseHandler::CAMERA;
            if( _e->button()  == Qt::RightButton)
                map.action = qglviewer::CameraFrame::MouseAction::TRANSLATE;//SCREEN_TRANSLATE;
            else if( _e->button()  == Qt::LeftButton)
                map.action = qglviewer::CameraFrame::MouseAction::ROTATE;
            map.withConstraint = true;

            camera()->frame()->startAction(map.action, map.withConstraint);
            camera()->frame()->mousePressEvent(_e/*, camera()*/ );

            update();

        }



        virtual void mouseMoveEvent(QMouseEvent *_e)
        {

            if (camera()->frame()->isManipulated()) 
            {
                //camera()->frame()->mouseMoveEvent(_e, /*camera()*/ camera()->frame()->coordinatesOf(camera()->pivotPoint()) );
                camera()->mouseMoveEvent(_e );

                /*if (camera()->frame()->action_ == ZOOM_ON_REGION)
                    update();*/
            } 
            //else if ((manipulatedFrame()) && (manipulatedFrame()->isManipulated()))
            //    if (manipulatedFrameIsACamera_)
            //        manipulatedFrame()->ManipulatedFrame::mouseMoveEvent(e, camera());
            //    else
            //        manipulatedFrame()->mouseMoveEvent(e, camera());
            //else if (hasMouseTracking()) 
            //{
            //    Q_FOREACH (MouseGrabber *mg, MouseGrabber::MouseGrabberPool()) 
            //    {
            //        mg->checkIfGrabsMouse(e->x(), e->y(), camera());
            //        if (mg->grabsMouse()) 
            //        {
            //            setMouseGrabber(mg);

            //            if (mouseGrabber() == mg) 
            //            {
            //                update();
            //                break;
            //            }
            //        }
            //    }
            //}
            this->update();
        }



        virtual void mouseReleaseEvent(QMouseEvent *_e){ _e->ignore(); }
        virtual void mouseDoubleClickEvent(QMouseEvent *_e){ _e->ignore(); }

        virtual void wheelEvent(QWheelEvent *_e)
        {
            WheelBindingPrivate wbp(_e->modifiers(), currentlyPressedKey_);

            MouseActionPrivate map = wheelBinding_[wbp];

            map.handler =  qglviewer::CameraFrame::MouseHandler::CAMERA;
            map.action = qglviewer::CameraFrame::MouseAction::ZOOM;
            map.withConstraint = true;


            camera()->frame()->startAction(map.action, map.withConstraint);
            camera()->frame()->wheelEvent(_e,  camera()->frame()->coordinatesOf(camera()->pivotPoint())  );
   
            this->update();
        }


        virtual void keyPressEvent(QKeyEvent *_e){ std::cout<<"pressed FU"<<std::endl;/*_e->ignore();*/ }
        virtual void keyReleaseEvent(QKeyEvent *_e){ _e->ignore(); }
        virtual void timerEvent(QTimerEvent *_e){ _e->ignore(); }
        virtual void closeEvent(QCloseEvent *_e){ _e->ignore(); }
  


    private:
        // Copy constructor and operator= are declared private and undefined
        // Prevents everyone from trying to use them
        QGLViewer(const QGLViewer &v);
        QGLViewer &operator=(const QGLViewer &v);

 


        


};

#endif // QGLVIEWER_QGLVIEWER_H
