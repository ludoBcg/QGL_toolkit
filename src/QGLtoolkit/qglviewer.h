/*********************************************************************************************************************
 *
 * qglviewer.h
 *
 * 3D OpenGL viewer based on QOpenGLWidget
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/


#ifndef QGLTOOLKIT_QGLVIEWER_H
#define QGLTOOLKIT_QGLVIEWER_H


#include <iostream>
#include <math.h>

// Qt includes
#include <QTimer>
#include <QClipboard>
#include <QGL>
#include <QMap>
#include <QTime>
#include <QString>
#include <QOpenGLWidget>
#include <QKeyEvent>



#include "camera.h"



namespace qgltoolkit 
{

    
/*!
* \class QGLViewer
* \brief 3D OpenGL viewer based on QOpenGLWidget
* To use a QGLViewer, derive you viewer class from the QGLViewer and 
* overload its draw() virtual method.
*/
class QGLViewer : public QOpenGLWidget 
{
    Q_OBJECT



    private :     

     
        qgltoolkit::Camera *m_camera;   /*!< camera */


    public:
  
        /*------------------------------------------------------------------------------------------------------------+
        |                                              GETTERS / SETTERS                                              |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn camera
        * \brief Returns the camera.
        */
        qgltoolkit::Camera  *camera() const { return m_camera; }

        /*!
        * \fn sceneRadius
        * \brief Returns the scene radius defined in the Camera.
        */
        double sceneRadius() const { return camera()->sceneRadius(); }

        /*!
        * \fn sceneCenter
        * \brief Returns the scene center defined in the Camera.
        */
        glm::vec3 sceneCenter() const { return camera()->sceneCenter(); }

        /*!
        * \fn setSceneRadius
        * \brief Set scene radius.
        */
        virtual void setSceneRadius(double _radius) 
        {
            camera()->setSceneRadius(_radius);
        }

        /*!
        * \fn setSceneCenter
        * \brief Set scene center.
        */
        virtual void setSceneCenter(const glm::vec3 &_center) 
        {
            camera()->setSceneCenter(_center);
        }

        /*!
        * \fn setSceneBoundingBox
        * \brief Set scene center and radius from bounding box.
        * \param _min, _max: min and max corners of the AABBox
        */
        void setSceneBoundingBox(const glm::vec3 &_min, const glm::vec3 &_max) 
        {
            camera()->setSceneBoundingBox(_min, _max);
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                    MISC.                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn showEntireScene
        * \brief Move camera to show entire scene.
        */
        void showEntireScene() 
        {
            camera()->showEntireScene();
            this->update();
        }


    private:

        /*!
        * \fn setCamera
        * \brief Set a new camera.
        */
        void setCamera(qgltoolkit::Camera  *const _camera) 
        {
            if (!_camera)
                return;

            _camera->setSceneRadius(this->sceneRadius());
            _camera->setSceneCenter(this->sceneCenter());

            _camera->setScreenWidthAndHeight(width(), height());

            //// Disconnect current camera from this viewer.
            //disconnect(this->camera()->frame(), SIGNAL(manipulated()), this, SLOT(update()));

            //// Connect camera frame to this viewer.
            //connect(camera->frame(), SIGNAL(manipulated()), SLOT(update()));

            m_camera = _camera;

        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                 CONSTRUCTORS                                                |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn defaultConstructor
        * \brief Set parameters to their default values. Called by the constructors.
        */
        void defaultConstructor() 
        { 
            setFocusPolicy(Qt::StrongFocus); 

            m_camera = new qgltoolkit::Camera();
            this->setSceneRadius(1.0);
            this->setSceneCenter( glm::vec3(0.0f) );
            showEntireScene();
            setCamera( this->camera() ); // force to use = operator

            setAttribute(Qt::WA_NoSystemBackground);
        }


    public:

        /*!
        * \fn QGLViewer
        * \brief 
        */
        explicit QGLViewer(QWidget *_parent = 0, Qt::WindowFlags _flags = Qt::WindowFlags())
        : QOpenGLWidget(_parent, _flags) 
        {
            defaultConstructor();
        }

        /*!
        * \fn QGLViewer
        * \brief 
        */
        explicit QGLViewer(QWidget *_parent, const QGLWidget *_shareWidget, Qt::WindowFlags _flags = 0)
        : QOpenGLWidget(_parent, _flags) 
        {
            Q_UNUSED(_shareWidget);
            defaultConstructor();
        }

        /*!
        * \fn QGLViewer
        * \brief 
        */
        explicit QGLViewer(QGLContext *_context, QWidget *_parent = 0, const QGLWidget *_shareWidget = 0, Qt::WindowFlags _flags = 0)
        : QOpenGLWidget(_parent, _flags) 
        {
            Q_UNUSED(_context);
            Q_UNUSED(_shareWidget);
            defaultConstructor();
        }

        /*!
        * \fn QGLViewer
        * \brief 
        */
        explicit QGLViewer(const QGLFormat &_format, QWidget *_parent = 0, const QGLWidget *_shareWidget = 0, Qt::WindowFlags _flags = 0)
        : QOpenGLWidget(_parent, _flags)
        {
            Q_UNUSED(_format);
            Q_UNUSED(_shareWidget);
            defaultConstructor();
        }

        /*!
        * \fn ~QGLViewer
        * \brief QGLViewer destructor
        */
        virtual ~QGLViewer(){}

    

    protected:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                    MISC.                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        virtual void resize(int _width, int _height) {}

        virtual void resizeGL(int _width, int _height) 
        {
            QOpenGLWidget::resizeGL(_width, _height);
            glViewport(0, 0, GLint(_width), GLint(_height));
            camera()->setScreenWidthAndHeight(_width, _height);
        }

        virtual void init() {}

        virtual void initializeGL() { init(); }


        virtual void draw() {}

        virtual void paintGL() { draw(); }

        virtual std::string helpString() const 
        {
            std::string text(" \n HELP: \n");
                        text += " Left mouse button: rotates camera round the scene \n";
                        text += " Right mouse button: translates camera in image plane \n";
                        text += " Middle mouse button / wheel: translates camera toward scene center (zoom) \n";
                        text += " Double click left: aligns the closer axis with the camera (if close enough) \n";
                        text += " Double click right : re-centers the scene \n";
            return text;
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                    EVENTS                                                   |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn mousePressEvent
        * \brief Event handler for mouse button pressed.
        */
        virtual void mousePressEvent(QMouseEvent * _e)
        {
            qgltoolkit::CameraFrame::MouseAction action;

            if( _e->button()  == Qt::RightButton)
                action = qgltoolkit::CameraFrame::MouseAction::TRANSLATE;
            else if( _e->button()  == Qt::LeftButton)
                action = qgltoolkit::CameraFrame::MouseAction::ROTATE;
            else if( _e->button()  == Qt::MiddleButton)
                action = qgltoolkit::CameraFrame::MouseAction::ZOOM;

            camera()->frame()->startAction(action);
            camera()->frame()->mousePressEvent(_e );

            update();

        }

        /*!
        * \fn mouseMoveEvent
        * \brief Event handler for mouse movement.
        */
        virtual void mouseMoveEvent(QMouseEvent *_e)
        {

            //if (camera()->frame()->isManipulated()) 
            //{
            //    camera()->mouseMoveEvent(_e );
            //} 
             camera()->frame()->mouseMoveEvent(_e, camera()->sceneCenter() );

            this->update();
        }

        /*!
        * \fn mouseReleaseEvent
        * \brief Event handler for mouse button released.
        */
        virtual void mouseReleaseEvent(QMouseEvent *_e){ _e->ignore(); }

        /*!
        * \fn mouseDoubleClickEvent
        * \brief Event handler for mouse button double click.
        */
        virtual void mouseDoubleClickEvent(QMouseEvent *_e)
        { 
            //_e->ignore(); 
            camera()->frame()->mouseDoubleClickEvent(_e, camera()->sceneCenter() );
            this->update();
        }

        /*!
        * \fn wheelEvent
        * \brief Event handler for mouse wheel rolled.
        */
        virtual void wheelEvent(QWheelEvent *_e)
        {
            qgltoolkit::CameraFrame::MouseAction action = qgltoolkit::CameraFrame::MouseAction::ZOOM;

            camera()->frame()->startAction(action);
            camera()->frame()->wheelEvent(_e);
   
            this->update();
        }

        /*!
        * \fn keyPressEvent
        * \brief Event handler for keyboard key pressed.
        */
        virtual void keyPressEvent(QKeyEvent *_e) { }

        /*!
        * \fn keyPressEvent
        * \brief Event handler for keyboard key released.
        */
        virtual void keyReleaseEvent(QKeyEvent *_e){ _e->ignore(); }

        /*!
        * \fn timerEvent
        * \brief Event handler for timer.
        */
        virtual void timerEvent(QTimerEvent *_e){ _e->ignore(); }

        /*!
        * \fn closeEvent
        * \brief Event handler for window closed.
        */
        virtual void closeEvent(QCloseEvent *_e){ _e->ignore(); }
  
        /*!
        * \fn help
        * \brief Print-out the help() string.
        * Override for different behavior.
        */
        virtual void help() { std::cout << helpString() << std::endl; }

    private:

        // Copy constructor and operator= are declared private and undefined
        // Prevents everyone from trying to use them
        QGLViewer(const QGLViewer &v);
        QGLViewer &operator=(const QGLViewer &v);


};


} // namespace qgltoolkit


#endif // QGLTOOLKIT_QGLVIEWER_H
