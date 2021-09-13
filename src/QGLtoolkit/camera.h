/*********************************************************************************************************************
 *
 * camera.h
 *
 * A perspective or orthographic camera
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/

#ifndef QGLTOOLKIT_CAMERA_H
#define QGLTOOLKIT_CAMERA_H


#include "cameraFrame.h"


namespace qgltoolkit {


/*!
* \class Camera
* \brief A perspective or orthographic camera.
*
* A Camera defines some intrinsic parameters (fieldOfView(), position(),
* viewDirection(), upVector()...) and useful positioning tools that ease its
* placement (showEntireScene(), fitSphere(), lookAt()...). 
* It exports its associated matrices and can interactively be modified using the mouse.
*
* The position() and orientation() of the Camera are defined by a CameraFrame (retrieved using frame()). 
* These methods are just convenient wrappers to the equivalent Frame methods.
*/
class Camera : public QObject 
{
    Q_OBJECT


    private:


        CameraFrame *m_frame;

        int m_screenWidth, m_screenHeight;          /*!< window size in pixels */ 
        double m_fieldOfView;                       /*!< FOV angle in radisn */
        CameraFrame::ProjectionType m_projType;        

        // mutable: can be modfied in a function foo() const
        mutable glm::mat4 m_viewMatrix;             /*!< view matrix */
        mutable bool m_viewMatrixIsUpToDate;        /*!< false if view matrix has been modified */
        mutable glm::mat4 m_projectionMatrix;       /*!< projection matrix */
        mutable bool m_projectionMatrixIsUpToDate;  /*!< false if projection matrix has been modified*/
        glm::vec3 m_sceneCenter;                    /*!< coords of scene center */
        double m_sceneRadius;                       /*!< scene radius */
        double m_zClippingCoef;                     /*!< defines margin between scene radius and frustum borders  */
        double m_orthoCoef;                         /*!< defines dimesnions for orthogonal projection */
       


    public Q_SLOTS:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                   SLOTS                                                     |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn onFrameModified
        * \brief Called when camera frame is modified.
        */
        void onFrameModified()
        {
            m_projectionMatrixIsUpToDate = false;
            m_viewMatrixIsUpToDate = false;
        }


    public:

        glm::vec3 m_center;
        

        /*------------------------------------------------------------------------------------------------------------+
        |                                                  GETTERS                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn frame
        * \brief Returns the camera frame.
        */
        CameraFrame *frame() const { return m_frame; }

        /*!
        * \fn pivotPoint
        * \brief Returns pivot point.
        */
        glm::vec3 pivotPoint() const { return frame()->pivotPoint(); }

        /*!
        * \fn screenWidth
        * \brief Returns screen width.
        */
        int screenWidth() const { return m_screenWidth; }

        /*!
        * \fn screenHeight
        * \brief Returns screen height.
        */
        int screenHeight() const { return m_screenHeight; }

        /*!
        * \fn aspectRatio
        * \brief Calculates and returns aspect ratio.
        */
        double aspectRatio() const { return static_cast<double>(m_screenWidth) / static_cast<double>(m_screenHeight); }

        /*!
        * \fn fieldOfView
        * \brief Returns FOV.
        */
        double fieldOfView() const { return m_fieldOfView; }

        /*!
        * \fn projType
        * \brief Returns projection type.
        */
        CameraFrame::ProjectionType projType() const { return m_projType; }

        /*!
        * \fn projectionMatrix
        * \brief Returns projection matrix.
        */
        glm::mat4 projectionMatrix() const { return m_projectionMatrix; }
        /*!
        * \fn viewMatrix
        * \brief Returns view matrix.
        */

        glm::mat4 viewMatrix() const { return m_viewMatrix; }

        /*!
        * \fn viewProjectionMatrix
        * \brief Calculates and returns view projection Matrix.
        */
        glm::mat4 viewProjectionMatrix() const { return  m_projectionMatrix * m_viewMatrix; }

        /*!
        * \fn sceneCenter
        * \brief Returns cords of scene center.
        */
        glm::vec3 sceneCenter() const { return m_sceneCenter; }

        /*!
        * \fn sceneRadius
        * \brief Returns scene radius.
        */
        double sceneRadius() const { return m_sceneRadius; }

        /*!
        * \fn distanceToSceneCenter
        * \brief Calculates and returns distance between camera and scene center.
        */
        double distanceToSceneCenter() const {  return std::abs( glm::length(m_sceneCenter - frame()->position() ) ); }

        /*!
        * \fn zClippingCoefficient
        * \brief Returns zClippingCoefficient.
        */
        double zClippingCoefficient() const { return m_zClippingCoef; }

        /*!
        * \fn zOrthoCoefficient
        * \brief Returns zOrthoCoefficient.
        */
        double zOrthoCoefficient() const { return m_orthoCoef; }

        /*!
        * \fn position
        * \brief Returns camera frame position.
        */
        glm::vec3 position() const { return frame()->position(); }

        /*!
        * \fn upVector
        * \brief Returns up vector.
        */
        glm::vec3 upVector() const { return frame()->inverseTransformOf(glm::vec3(0.0f, 1.0f, 0.0f));  } 

        /*!
        * \fn viewDirection
        * \brief Returns view direction.
        */
        glm::vec3 viewDirection() const { return frame()->viewDirection(); } 

        /*!
        * \fn orientation
        * \brief Returns camera frame orientation.
        */
        Quaternion orientation() const { return frame()->orientation(); }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                  MATRICES                                                   |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn zNear
        * \brief Calculates and returns z-near distance.
        */
        virtual double zNear() const
        {

            double zNear = distanceToSceneCenter() - zClippingCoefficient() * sceneRadius();

            // Prevents negative or null zNear values.
            const double zMin = 0.1f;
            if (zNear < zMin)
                zNear = zMin;

            return zNear;
        }

        /*!
        * \fn zFar
        * \brief Calculates and returns z-far distance.
        */
        virtual double zFar() const
        {
            return distanceToSceneCenter() + zClippingCoefficient() * sceneRadius();
        }

        /*!
        * \fn computeProjectionMatrix
        * \brief Calculates projection matrix.
        */
        void computeProjectionMatrix() const
        {
            if (m_projectionMatrixIsUpToDate)
                return;

            const double ZNear = zNear();
            const double ZFar = zFar();

            switch (projType()) 
            {
                case CameraFrame::PERSPECTIVE: 
                {
                    const double f = 1.0 / tan(fieldOfView() / 2.0);

                    m_projectionMatrix = glm::perspective(fieldOfView(), aspectRatio(), zNear(), zFar() ); 
                    
                    break;
                }
                case CameraFrame::ORTHOGRAPHIC: 
                {
                    double halfWidth = m_orthoCoef * ((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
                    double halfHeight = m_orthoCoef * ((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);

                    m_projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear(), zFar() ); 
                    break;
                }
            }

            m_projectionMatrixIsUpToDate = true;
        }

        /*!
        * \fn computeViewMatrix
        * \brief Calculates view matrix.
        */
        void computeViewMatrix() const
        {
            if (m_viewMatrixIsUpToDate)
                return;

            const Quaternion q = frame()->orientation();
            const glm::vec3 t = q.inverseRotate(frame()->position());
        
//std::cout<<"center_  "<<center_.x<< " " << center_.y << " " << center_.z << std::endl;     

glm::vec3 diff = glm::normalize(m_center - position());
glm::vec3 axis = glm::normalize(frame()->orientation().axis());
//std::cout<<"(center_ - position() )  "<<diff.x<< " " << diff.y << " " << diff.z << std::endl;  
//std::cout<<"axis  "<<axis.x<< " " << axis.y << " " << axis.z << std::endl;  
//std::cout<<"quat  "<<q[0]<< " " << q[1] << " " << q[2] << std::endl; 
const float q00 = 2.0 * q[0] * q[0];
const float q11 = 2.0 * q[1] * q[1];
const float q22 = 2.0 * q[2] * q[2];

const float q01 = 2.0 * q[0] * q[1];
const float q02 = 2.0 * q[0] * q[2];
const float q03 = 2.0 * q[0] * q[3];

const float q12 = 2.0 * q[1] * q[2];
const float q13 = 2.0 * q[1] * q[3];

const float q23 = 2.0 * q[2] * q[3];
glm::vec3 quatZ = glm::normalize( glm::vec3( q02 + q13 , q12 - q03 , 1.0f - q11 - q00 ) );
glm::vec3 quatU = glm::normalize( glm::vec3( q01 - q23 , 1.0 - q22 - q00 , q12 + q03 ) );
//std::cout<<"quat Z  "<< quatZ.x << " " << quatZ.y << " " << quatZ.z << std::endl;  

            m_viewMatrix = glm::lookAt(position(),  position() - quatZ , quatU );

            m_viewMatrixIsUpToDate = true; 
        }

        /*!
        * \fn setScreenWidthAndHeight
        * \brief Set windows' dimensions.
        */
        void setScreenWidthAndHeight(int _width, int _height) 
        {
            // Prevent negative and zero dimensions that would cause divisions by zero.
            m_screenWidth = _width > 0 ? _width : 1;
            m_screenHeight = _height > 0 ? _height : 1;

            
            frame()->setCamParam(m_screenWidth, m_screenHeight, fieldOfView(), projType(), sceneRadius(), pivotPoint() ); //@@
            
            m_projectionMatrixIsUpToDate = false;
        }

        /*!
        * \fn setViewDirection
        * \brief Set view direction vector.
        * \param _direction: direction as a 3D vector
        */
        void setViewDirection(glm::vec3 &_direction)
        {
            if (_direction == glm::vec3(0.0f) )
                return;

            _direction = glm::normalize(_direction);
            glm::vec3 xAxis = glm::cross(_direction , upVector() );
            if (xAxis == glm::vec3(0.0f)) 
            {
                // target is aligned with upVector, this means a rotation around X axis
                // X axis is then unchanged, let's keep it !
                xAxis = frame()->inverseTransformOf( glm::vec3(1.0, 0.0, 0.0) );
            }

            Quaternion q;
            q.setFromRotatedBasis(xAxis, glm::cross(xAxis, _direction), -_direction);
            //frame()->setOrientationWithConstraint(q);
            frame()->setOrientation(q);
        }

        /*!
        * \fn lookAt
        * \brief Set view direction vector, given a target position.
        * The view direction is calculated so it is oriented towards (or "look at") the target point.
        * \param _target: 3D coords of a point in scene to look at
        */
        void lookAt(const glm::vec3 &_target)
        {
            setViewDirection(_target - position() );
        }

        /*!
        * \fn cameraCoordinatesOf
        * \brief Transfers coordinates of a point from world space to camera space.
        * \param _src: point coords in world space
        * \return point coords in camera space
        */
        glm::vec3 cameraCoordinatesOf(const glm::vec3 &_src) const { return frame()->coordinatesOf(_src); }

        /*!
        * \fn worldCoordinatesOf
        * \brief Transfers coordinates of a point from camera space to world space.
        * \param _src: point coords in camera space
        * \return point coords in world space
        */
        glm::vec3 worldCoordinatesOf(const glm::vec3 &_src) const { return frame()->inverseCoordinatesOf(_src); }

        /*!
        * \fn getOrthoWidthHeight
        * \brief Calculates half diemrensions of window when orthographic projection is used.
        * \param _halfWidth, _halfHeight: half dimensions to be returned
        */
        virtual void getOrthoWidthHeight(double &_halfWidth, double &_halfHeight) const 
        {
            const double dist = m_orthoCoef * fabs(cameraCoordinatesOf(pivotPoint()).z);

            _halfWidth = dist * ((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
            _halfHeight = dist * ((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                  SETTERS                                                    |
        +------------------------------------------------------------------------------------------------------------*/


    

        /*! \fn setFrame 
        * \brief Set camera frame.
        */
        void setFrame(CameraFrame *const _cf)
        {
            if (!_cf)
                return;

            if (m_frame) 
            {
                disconnect(m_frame, SIGNAL(modified()), this, SLOT(onFrameModified()));
            }

            m_frame = _cf;

            connect(m_frame, SIGNAL(modified()), this, SLOT(onFrameModified()));
            onFrameModified();
        }

        /*! \fn setPivotPoint 
        * \brief Set pivot point.
        */
        void setPivotPoint(const glm::vec3 &_point)
        {
            const float prevDist = std::abs(cameraCoordinatesOf(pivotPoint()).z);

            // If frame's RAP is set directly, projectionMatrixIsUpToDate_ should also be
            // set to false to ensure proper recomputation of the ORTHO projection matrix.
            frame()->setPivotPoint(_point);
std::cout<<"Camera::frame()->pivotPoint() = "<<this->frame()->pivotPoint().x<<" "<<this->frame()->pivotPoint().y<<" "<<this->frame()->pivotPoint().z<<std::endl;
            // orthoCoef_ is used to compensate for changes of the pivotPoint, so that the
            // image does not change when the pivotPoint is changed in ORTHOGRAPHIC mode.
            const float newDist = std::abs(cameraCoordinatesOf(pivotPoint()).z);
            // Prevents division by zero when rap is set to camera position
            if ((prevDist > std::numeric_limits<float>::epsilon()) && (newDist > std::numeric_limits<float>::epsilon()))
                m_orthoCoef *= prevDist / newDist;

            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setAspectRatio 
        * \brief Set dimesnions from aspect ratio.
        */
        void setAspectRatio(double _aspect) 
        {
            setScreenWidthAndHeight(int(100.0 * _aspect), 100);
        }

        /*! \fn setFieldOfView 
        * \brief Set FOV.
        */
        void setFieldOfView(double _fov)
        {
            m_fieldOfView = _fov;
            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setFOVToFitScene 
        * \brief Set FOV so it contains the entire scene.
        */
        void setFOVToFitScene()
        {
            if (distanceToSceneCenter() > sqrt(2.0) * sceneRadius())
                setFieldOfView(2.0 * asin(sceneRadius() / distanceToSceneCenter()));
            else
                setFieldOfView(M_PI / 2.0);
        }

        /*! \fn setProjType 
        * \brief Set type of projection.
        */
        void setProjType(CameraFrame::ProjectionType _type)
        {
            if ((_type == CameraFrame::ORTHOGRAPHIC) && (m_projType == CameraFrame::PERSPECTIVE))
                m_orthoCoef = tan(fieldOfView() / 2.0);

            m_projType = _type;
            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setSceneCenter 
        * \brief Set scene center.
        */
        void setSceneCenter(const glm::vec3 &_center)
        {
            m_sceneCenter = _center;
            setPivotPoint(sceneCenter());
            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setSceneRadius 
        * \brief Set scene radius.
        */
        void setSceneRadius(double _radius)
        {
            if (_radius <= 0.0) 
            {
                std::cerr<<"Scene radius must be positive - Ignoring value"<<std::endl;
                return;
            }
            m_sceneRadius = _radius;
            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setSceneBoundingBox 
        * \brief Set scene center and radius given an axis aligned bounding box.
        * \param _min, _max: min amd max corners of the AABBox
        */
        void setSceneBoundingBox(const glm::vec3 &_min, const glm::vec3 &_max)
        {
            setSceneCenter((_min + _max) / 2.0f);
            setSceneRadius(0.5f * glm::length( _max - _min ) );
        }

        /*! \fn setZClippingCoefficient 
        * \brief Set ZClippingCoefficient.
        */
        void setZClippingCoefficient(double _coef) 
        {
            m_zClippingCoef = _coef;
            m_projectionMatrixIsUpToDate = false;
        }

        /*! \fn setPosition 
        * \brief Set camera frame position.
        */
        void setPosition(const glm::vec3 &_pos) { frame()->setPosition(_pos); }

        /*! \fn setOrientation 
        * \brief Set camera frame orientation from quaternion.
        * \param _q: orientation as quaternion
        */
        void setOrientation(const Quaternion &_q)
        {
            frame()->setOrientation(_q);
            frame()->updateSceneUpVector();
        }

        /*! \fn setOrientation 
        * \brief Set camera frame orientation from two angles.
        * \param _theta, _phi: orientation as angles
        */
        void setOrientation(double _theta, double _phi)
        {
            glm::vec3 axis(0.0, 1.0, 0.0);
            const Quaternion rot1(axis, _theta);
            axis = glm::vec3(-cos(_theta), 0.0, sin(_theta));
            const Quaternion rot2(axis, _phi);
            setOrientation(rot1 * rot2);
        }

        /*! \fn setUpVector 
        * \brief Set up vector
        * Rotates the Camera so that its upVector() becomes _up (defined in the
        * world coordinate system).
        * 
        * The Camera is rotated around an axis orthogonal to _up and to the current
        * upVector() direction. Use this method in order to define the Camera horizontal plane.
        * 
        * When _noMove is set to false, the orientation modification is compensated
        * by a translation, so that the pivotPoint() stays projected at the same position
        * on screen. This is especially useful when the Camera is used as an observer of
        * the scene (default mouse binding).
        */
        void setUpVector(const glm::vec3 &_up, bool _noMove = true)
        {
            Quaternion q( glm::vec3(0.0, 1.0, 0.0), frame()->transformOf(_up));

            if (!_noMove)
                frame()->setPosition(pivotPoint() - (frame()->orientation() * q).rotate(frame()->coordinatesOf(pivotPoint())));

            frame()->rotate(q);

            // Useful in fly mode to keep the horizontal direction.
            //frame()->updateSceneUpVector();
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                   MISC.                                                     |
        +------------------------------------------------------------------------------------------------------------*/

        /*! \fn setUpVector 
        * \brief Moves the Camera so that the sphere defined by _center 
        * and _radius is visible and fits in the frustum.
        * \param _center: sphere center
        * \param _radius: sphere radius
        */
        void fitSphere(const glm::vec3 &_center, double _radius)
        {
            float distance = 0.0;
            switch (projType()) 
            {
                case CameraFrame::PERSPECTIVE: 
                {
                    distance = _radius / sin(fieldOfView() / 2.0);
                    break;
                }
                case CameraFrame::ORTHOGRAPHIC: 
                {
                    distance = glm::dot( (_center - pivotPoint()), viewDirection()) + (_radius / m_orthoCoef);
                    break;
                }
            }
            glm::vec3 newPos(_center - (viewDirection() * distance) );
            //frame()->setPositionWithConstraint(newPos);
            frame()->setPosition(newPos);
        }

        /*! \fn showEntireScene 
        * \brief Moves the Camera so that the entire scene is visible.
        * The scene is described by its center coords and its radius.
        */
        void showEntireScene() { fitSphere(sceneCenter(), sceneRadius()); }

        /*! \fn fitBoundingBox 
        * \brief Moves the Camera so that the entire scene is visible.
        * The scene is described by a axis-aligned bounding box.
        * \param _min, _max: min amd max corners of the AABBox
        */
        void fitBoundingBox(const glm::vec3 &_min, const glm::vec3 &_max)
        {
            float diameter = std::max( std::abs(_max[1] - _min[1]),  std::abs(_max[0] - _min[0]));
            diameter = std::max( std::abs(_max[2] - _min[2]), diameter);
            fitSphere(0.5f * (_min + _max), 0.5f * diameter);
        }

        /*! \fn centerScene 
        * \brief Moves the Camera so that its sceneCenter() is projected on the 
        * center of the window. 
        * The orientation() and fieldOfView() are unchanged.
        * Simply projects the current position on a line passing through sceneCenter(). 
        */
        void centerScene(){ frame()->projectOnLine(sceneCenter(), viewDirection()); } 


        /*------------------------------------------------------------------------------------------------------------+
        |                                               CONSTRUCTORS                                                  |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn Camera
        * \brief Destructor of Camera.
        */
        Camera() 
        : m_frame(NULL), m_fieldOfView(M_PI / 4.0), m_viewMatrixIsUpToDate(false), m_projectionMatrixIsUpToDate(false)
        {
            setFrame(new CameraFrame());
            setSceneRadius(1.0);
            m_orthoCoef = tan(fieldOfView() / 2.0);

            setSceneCenter(glm::vec3(0.0f));

            setProjType(CameraFrame::PERSPECTIVE);

            setZClippingCoefficient(/*sqrt(3.0)*/1.25);

            setScreenWidthAndHeight(600, 400);

            m_viewMatrix = glm::mat4(1.0f);
            m_projectionMatrix = glm::mat4(1.0f);

            computeProjectionMatrix();
        }

        /*!
        * \fn ~Camera
        * \brief Destructor of Camera.
        */
        virtual ~Camera()
        {
            delete m_frame;
        }

        /*!
        * \fn operator=
        * \brief Equal operator.
        */
        Camera &operator=(const Camera &_camera)
        {
            setScreenWidthAndHeight(_camera.screenWidth(), _camera.screenHeight());
            setFieldOfView(_camera.fieldOfView());
            setSceneRadius(_camera.sceneRadius());
            setSceneCenter(_camera.sceneCenter());
            setZClippingCoefficient(_camera.zClippingCoefficient());
            setProjType(_camera.projType());


            m_orthoCoef = _camera.m_orthoCoef;
            m_projectionMatrixIsUpToDate = false;

            m_frame->setPosition(_camera.position());
            m_frame->setOrientation(_camera.orientation());

            m_frame->setCamParam(_camera.screenWidth(), _camera.screenHeight(), _camera.fieldOfView(), _camera.projType(), _camera.sceneRadius(), _camera.pivotPoint() );
      
            computeProjectionMatrix();
            computeViewMatrix();

            return *this;
        }

        /*!
        * \fn CameraFrame
        * \brief Copy constructor of CameraFrame.
        */
        Camera(const Camera &_camera) : QObject(), m_frame(nullptr) 
        {
            setFrame(new CameraFrame(*_camera.frame()));

            m_viewMatrix = glm::mat4(1.0f);
            m_projectionMatrix = glm::mat4(1.0f);

            (*this) = _camera;
        }


};

} // namespace qgltoolkit

#endif // QGLTOOLKIT_CAMERA_H
