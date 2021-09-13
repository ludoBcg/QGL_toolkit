/*********************************************************************************************************************
 *
 * camera.h
 *
 * A Frame with Camera specific mouse bindings
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/

#ifndef QGLTOOLKIT_CAMERA_FRAME_H
#define QGLTOOLKIT_CAMERA_FRAME_H

#include <QDateTime>
#include <QString>
#include <QTimer>
#include <QObject>
#include <QMap>
#include <QPoint>
#include <QMouseEvent>


#include "frame.h"



namespace qgltoolkit 
{


/*!
* \class CameraFrame
* \brief A Frame that can be rotated and translated using the mouse.
*  It converts the mouse motion into a translation and an orientation updates.
*
* Emits a manipulated() signal each time its state is modified by the mouse. 
* This signal is automatically connected to the QGLViewer::update() slot when the 
* ManipulatedFrame is attached to a viewer using QGLViewer::setManipulatedFrame().
*
* A camera frame rotates around its pivotPoint(), which corresponds to the associated Camera::pivotPoint().
*/
class CameraFrame : public qgltoolkit::Frame 
{

    Q_OBJECT


    public:

        // Camera projection type
        enum ProjectionType { PERSPECTIVE, ORTHOGRAPHIC };

        // Mouse actions
        enum MouseAction {
            NO_MOUSE_ACTION,
            ROTATE,
            ZOOM,
            TRANSLATE
            };


    private:

        // interactions sensitivity
        double m_rotationSensitivity;       /*!< rotation sensitivity factor */
        double m_translationSensitivity;    /*!< translation sensitivity factor */
        double m_wheelSensitivity;          /*!< wheel sensitivity factor */
        double m_zoomSensitivity;           /*!< zoom sensitivity factor */

        // flags
        bool m_rotatesAroundUpVector;       /*!< rotates around pivot point if false (defaulf behavior) */
        bool m_zoomsOnPivotPoint;           /*!< zoom on pivot point if true (defaulf behavior) */

        // scene parameters
        glm::vec3 m_sceneUpVector;          /*!< up direction vector */
        glm::vec3 m_pivotPoint;             /*!< pivot point coords */
        double m_sceneRadius;               /*!< scene radius */

        // camera parameters
        int m_screenWidth, m_screenHeight;  /*!< windows dims (in pixels) */
        double m_fieldOfView;               /*!< fov angle (in radians) */
        ProjectionType m_projType;          /*!< camera projection type */
        
        // UI event
        MouseAction m_action;               /*!< mouse action type */
        QPoint m_prevPos;                   /*!< previous mouse cursor position */


    public:


        /*------------------------------------------------------------------------------------------------------------+
        |                                            CONSTRUCTORS/ SETTERS                                            |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn setCamParam
        * \brief Set up camera parameters.
        * \param _screenWidth, m_screenHeight: windows dims (in pixels)
        * \param _fov: fov angle (in radians)
        * \param _projType: camera projection type
        * \param _sceneRadius: scene radius
        * \param _pivotPoint: pivot point coords
        */
        void setCamParam(int _screenWidth, int  _screenHeight, double _fov, ProjectionType _projType, double _sceneRadius, glm::vec3 _pivotPoint)
        { 
            m_screenWidth = _screenWidth;  
            m_screenHeight = _screenHeight;  
            m_fieldOfView = _fov;
            m_projType = _projType;
            m_sceneRadius = _sceneRadius;
            m_pivotPoint = _pivotPoint;
        }

        /*!
        * \fn CameraFrame
        * \brief Destructor of CameraFrame.
        */
        CameraFrame()
        : m_sceneUpVector(0.0, 1.0, 0.0), m_rotatesAroundUpVector(false), m_zoomsOnPivotPoint(true) 
        {
            setRotationSensitivity(1.0f);
            setTranslationSensitivity(1.0f);
            setWheelSensitivity(1.0f);
            setZoomSensitivity(1.0f);

            setCamParam(0, 0, 0, PERSPECTIVE, 0, glm::vec3(0.0) );
        }

        /*!
        * \fn ~CameraFrame
        * \brief Destructor of CameraFrame.
        */
        virtual ~CameraFrame() {}

        /*!
        * \fn operator=
        * \brief Equal operator.
        */
        CameraFrame &operator=(const CameraFrame &_cf)
        {
            Frame::operator=(_cf);

            setRotationSensitivity(_cf.rotationSensitivity());
            setTranslationSensitivity(_cf.translationSensitivity());
            setWheelSensitivity(_cf.wheelSensitivity());
            setZoomSensitivity(_cf.zoomSensitivity());

            m_action = NO_MOUSE_ACTION;

            setSceneUpVector(_cf.sceneUpVector());
            setRotatesAroundUpVector(_cf.m_rotatesAroundUpVector);
            setZoomsOnPivotPoint(_cf.m_zoomsOnPivotPoint);

            setCamParam(_cf.m_screenWidth, _cf.m_screenHeight, _cf.m_fieldOfView, _cf.m_projType , _cf.m_sceneRadius, _cf.m_pivotPoint);

            return *this;
        }

        /*!
        * \fn CameraFrame
        * \brief Copy constructor of CameraFrame.
        */
        CameraFrame(const CameraFrame &_cf)
        : Frame(_cf) 
        {
            (*this) = (_cf);
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                             GETTERS / SETTERS                                               |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn viewDirection
        * \brief Returns the normalized view direction of the Camera, 
        * defined in the world coordinate system.
        * This corresponds to the negative Z axis of the frame().
        */
        glm::vec3 viewDirection() const { return inverseTransformOf(glm::vec3(0.0f, 0.0f, -1.0f)); } 

        /*! \fn pivotPoint 
        * \brief Get pivot point.
        */
        glm::vec3 pivotPoint() const { return m_pivotPoint; }

        /*! \fn setPivotPoint 
        * \brief Set pivot point.
        */
        void setPivotPoint(const glm::vec3 &point) { m_pivotPoint = point; }

        /*! \fn rotatesAroundUpVector 
        * \brief Get rotatesAroundUpVector flag.
        */
        bool rotatesAroundUpVector() const { return m_rotatesAroundUpVector; }

        /*! \fn setRotatesAroundUpVector
        * \brief Set rotatesAroundUpVector flag.
        */
        void setRotatesAroundUpVector(bool constrained) { m_rotatesAroundUpVector = constrained; }

        /*! \fn zoomsOnPivotPoint 
        * \brief Get zoomsOnPivotPoint flag.
        */
        bool zoomsOnPivotPoint() const { return m_zoomsOnPivotPoint; }

        /*! \fn setZoomsOnPivotPoint
        * \brief Set zoomsOnPivotPoint flag.
        */
        void setZoomsOnPivotPoint(bool _enabled) { m_zoomsOnPivotPoint = _enabled; }

        /*! \fn sceneUpVector 
        * \brief Get up vector.
        */
        glm::vec3 sceneUpVector() const { return m_sceneUpVector; }

        /*! \fn setSceneUpVector
        * \brief Set up vector.
        */
        void setSceneUpVector(const glm::vec3 &up) { m_sceneUpVector = up; }

        /*! \fn rotationSensitivity 
        * \brief Get rotation sensitivity.
        */
        double rotationSensitivity() const { return m_rotationSensitivity; }

        /*! \fn setRotationSensitivity
        * \brief Set rotation sensitivity.
        */
        void setRotationSensitivity(double sensitivity) { m_rotationSensitivity = sensitivity; }
 
        /*! \fn translationSensitivity 
        * \brief Get translation sensitivity.
        */
        double translationSensitivity() const { return m_translationSensitivity; }

        /*! \fn setTranslationSensitivity
        * \brief Set translation sensitivity.
        */
        void setTranslationSensitivity(double sensitivity) { m_translationSensitivity = sensitivity; }
  
        /*! \fn zoomSensitivity 
        * \brief Get zoom sensitivity.
        */
        double zoomSensitivity() const { return m_zoomSensitivity; }

        /*! \fn setZoomSensitivity
        * \brief Set zoom sensitivity.
        */
        void setZoomSensitivity(double sensitivity) { m_zoomSensitivity = sensitivity; }
  
        /*! \fn wheelSensitivity 
        * \brief Get wheel sensitivity.
        */
        double wheelSensitivity() const { return m_wheelSensitivity; }

        /*! \fn setWheelSensitivity
        * \brief Set wheel sensitivity.
        */
        void setWheelSensitivity(double sensitivity) { m_wheelSensitivity = sensitivity; }

        /*! \fn currentMouseAction 
        * \brief Get mouse action.
        */
        MouseAction currentMouseAction() const { return m_action; }

        /*! \fn isManipulated 
        * \brief Returns true if camera frame is being manupilated.
        */
        bool isManipulated() const { return m_action != NO_MOUSE_ACTION; }
 

        void updateSceneUpVector()
        {
            m_sceneUpVector = inverseTransformOf( glm::vec3(0.0, 1.0, 0.0) );
        }


    Q_SIGNALS:
  
        /*! \fn manipulated 
        * \brief Signal when camera frame is being manipulated.
        */
        void manipulated();


    private:


        /*------------------------------------------------------------------------------------------------------------+
        |                                      TRACKBALL / ZOOM TRANSFORMATIONS                                       |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn zoom
        * \brief Translates camera according to zoom delta
        * \param 
        */
        void zoom(double _delta, glm::vec3 _camCoordPivot)
        {
            const float sceneRadius = m_sceneRadius;
            if ( m_zoomsOnPivotPoint ) 
            {
                glm::vec3 direction = position() - m_pivotPoint;
                if ( ( glm::length(direction) > 0.1 * sceneRadius || _delta > 0.0)  && ( glm::length(direction) < 10.0 * sceneRadius || _delta < 0.0) )
                    translate( (float)_delta * direction);
            } 
            else 
            {
                const float coef = std::max( std::abs( _camCoordPivot.z), 0.2f * sceneRadius);
                glm::vec3 trans(0.0, 0.0, -coef * _delta);
                translate(inverseTransformOf(trans));
            }
        }

        /*!
        * \fn projectOnBall
        * \brief Returns "pseudo-distance" from (_x,_y) to unit ball.
        * For a point inside the ball, it is proportional to the euclidean distance to the ball.
        * For a point outside the ball, it is proportional to the inverse of this distance 
        * (tends to zero) on the ball, the function is continuous.
        * \param _x, _y: 2D coords of point to project
        * \return distance to ball
        */
        static double projectOnBall(double _x, double _y) 
        {
            // If you change the size value, change angle computation in
            // deformedBallQuaternion().
            const double size = 1.0;
            const double size2 = size * size;
            const double size_limit = size2 * 0.5;

            const double d = _x * _x + _y * _y;
            return d < size_limit ? sqrt(size2 - d) : size_limit / sqrt(d);
        }

        /*!
        * \fn deformedBallQuaternion
        * \brief Returns a quaternion computed according to the mouse motion. 
        * Mouse positions are projected on a deformed ball, centered on (_cx, _cy).
        * \param _x, _y: 2D coords of point on screen
        * \param _cx, _cy: 2D coords of center point
        * \return trackball rotation as quaternion
        */
        Quaternion deformedBallQuaternion(int _x, int _y, double _cx, double _cy)
        {
            // Points on the deformed ball
            double px = rotationSensitivity() * (m_prevPos.x() - _cx) / m_screenWidth;
            double py = rotationSensitivity() * (_cy - m_prevPos.y()) / m_screenHeight;
            double dx = rotationSensitivity() * (_x - _cx) / m_screenWidth ;
            double dy = rotationSensitivity() * (_cy - _y) / m_screenHeight ;

            const glm::vec3 p1(px, py, projectOnBall(px, py));
            const glm::vec3 p2(dx, dy, projectOnBall(dx, dy));
            // Approximation of rotation angle
            // Should be divided by the projectOnBall size, but it is 1.0
            const glm::vec3 axis = glm::cross(p2, p1);
            const double angle = 5.0 * asin(sqrt( squaredNorm(axis) / squaredNorm(p1) / squaredNorm(p2)));
            return Quaternion(axis, angle);
        }

        /*!
        * \fn deltaWithPrevPos
        * \brief Returns a screen scaled delta from event's position to m_prevPos,
        * along the X or Y direction, whichever has the largest magnitude.
        * \param _event: UI event
        */
        double deltaWithPrevPos(QMouseEvent *const _event) const
        {
            double dx = double(_event->x() - m_prevPos.x()) / m_screenWidth ;
            double dy = double(_event->y() - m_prevPos.y()) / m_screenHeight ;

            double value = std::abs(dx) > std::abs(dy) ? dx : dy;
            return value * zoomSensitivity();
        }

        /*!
        * \fn wheelDelta
        * \brief Returns a normalized wheel delta, proportionnal to wheelSensitivity().
        * \param _event: UI event
        */
        double wheelDelta(const QWheelEvent *_event) const
        {
            static const double WHEEL_SENSITIVITY_COEF = 8E-4;
            return _event->delta() * wheelSensitivity() * WHEEL_SENSITIVITY_COEF;
        }


    public:


        /*------------------------------------------------------------------------------------------------------------+
        |                                                   EVENTS                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn mousePressEvent
        * \brief Initiates the mouse manipulation.
        * Previous mouse position is updated with current position.
        * \param _event: UI event
        */
        virtual void mousePressEvent(QMouseEvent *const _event) 
        {
            m_prevPos = _event->pos();
        }

        /*!
        * \fn mouseReleaseEvent
        * \brief Stops the mouse manipulation.
        * \param _event: UI event
        */
        virtual void mouseReleaseEvent(QMouseEvent *const _event)
        {
            Q_UNUSED(_event);

            m_action = NO_MOUSE_ACTION;
        }
        
        /*!
        * \fn mouseMoveEvent
        * \brief Modifies the camera frame according to the mouse motion.
        * Actual behavior depends on mouse action type.
        * Emits the manipulated() signal.
        * \param _event: UI event
        * \param _sceneCenter: scene center coords
        */
        virtual void mouseMoveEvent(QMouseEvent *const _event, glm::vec3 &_sceneCenter )
        {
            switch (m_action) 
            {
                case TRANSLATE: 
                {
                    //const QPoint delta = prevPos_ - event->pos();
                    const QPoint delta = _event->pos() - m_prevPos;
                    glm::vec3 trans(delta.x(), -delta.y(), 0.0);
                    // Scale to fit the screen mouse displacement
                    switch (m_projType) 
                    {
                        case PERSPECTIVE:
                            trans *= 2.0 * tan(m_fieldOfView / 2.0) * std::abs(( this->coordinatesOf(pivotPoint())).z) / m_screenHeight;
                            break;
                        case ORTHOGRAPHIC: 
                        {
                            std::cout<<"orthographics"<<std::endl;
                            //double w, h;
                            //camera->getOrthoWidthHeight(w, h);
                            //trans[0] *= 2.0 * w / screenWidth_ /*camera->screenWidth()*/;
                            //trans[1] *= 2.0 * h / screenHeight_ /*camera->screenHeight()*/;
                            break;
                        }
                    }
                    translate(inverseTransformOf( (float)translationSensitivity() * -trans));
                    //_sceneCenter = _sceneCenter + inverseTransformOf( (float)translationSensitivity() * -trans);  // @@@ 


                    break;
                }

                case ZOOM: 
                {
                    zoom(deltaWithPrevPos(_event),  this->coordinatesOf(pivotPoint()) );
                    break;
                }

                case ROTATE: 
                {
                    Quaternion rot;
                    if (m_rotatesAroundUpVector) 
                    {
                        // Multiply by 2.0 to get on average about the same speed as with the
                        // deformed ball
                        double dx = 2.0 * rotationSensitivity() * (m_prevPos.x() - _event->x()) / m_screenWidth ;
                        double dy = 2.0 * rotationSensitivity() * (m_prevPos.y() - _event->y()) / m_screenHeight ;
                        dx = -dx;
                        dy = -dy;
                        glm::vec3 verticalAxis = transformOf(m_sceneUpVector);
                        rot = Quaternion(verticalAxis, dx) * Quaternion( glm::vec3(1.0, 0.0, 0.0), dy);
                        rotate(rot);
                    } 
                    else 
                    {   
                        glm::vec3 trans = this->coordinatesOf(pivotPoint()); //camera->projectedCoordinatesOf(pivotPoint());
                        rot = deformedBallQuaternion(_event->x(), _event->y(), trans[0], trans[1]);
                        
                        rotateAroundPoint(rot, pivotPoint(), _sceneCenter);  // @@@ center ?
                    }

                    break;
                }

                case NO_MOUSE_ACTION:
                    break;
            }

            if (m_action != NO_MOUSE_ACTION) 
            {
                m_prevPos = _event->pos();

                Q_EMIT manipulated();
            }
        }
        
        /*!
        * \fn mouseDoubleClickEvent
        * \brief Modifies camera frame according to double click event.
        * Left button double click aligns the camera frame with the camera axis (see alignWithFrame() ). 
        * Right button projects the camera frame on the camera view direction.
        * \param _event: UI event
        * \param _sceneCenter: scene center coords
        */
        virtual void mouseDoubleClickEvent(QMouseEvent *const _event, glm::vec3 _sceneCenter) 
        {

            Frame *frame = new Frame();
            frame->setTranslation( pivotPoint() );

            if (_event->modifiers() == Qt::NoModifier)
            {
                switch (_event->button()) 
                {
                    case Qt::LeftButton:
                        alignWithFrame(frame, true);
                        break;
                    case Qt::RightButton:
                        projectOnLine( _sceneCenter, viewDirection() );
                        break;
                    default:
                        break;
                }
            }
        }

        /*!
        * \fn wheelEvent
        * \brief Call zoom acording to wheel delta
        * \param _event: UI event
        */
        virtual void wheelEvent(QWheelEvent *const _event )
        {

            if (m_action == ZOOM) 
            {
                zoom(-wheelDelta(_event), this->coordinatesOf(pivotPoint()) );
                Q_EMIT manipulated();
            }

            m_action = NO_MOUSE_ACTION;
        }

        /*!
        * \fn startAction
        * \brief Handles mouse events.
        * \param _ma: type of mouse action
        */
        virtual void startAction( int _ma) // int is really a QGLViewer::MouseAction
        {
            m_action = (MouseAction)(_ma);
        }

      

};

} // namespace qgltoolkit

#endif // QGLTOOLKIT_CAMERA_FRAME_H
