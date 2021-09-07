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


class Camera : public QObject 
{
    Q_OBJECT

    public:
    //    enum ProjectionType { PERSPECTIVE, ORTHOGRAPHIC };

    private:

        // F r a m e
        CameraFrame *frame_;

        // C a m e r a   p a r a m e t e r s
        int screenWidth_, screenHeight_; // size of the window, in pixels
        double fieldOfView_;              // in radians
        CameraFrame::ProjectionType projType_;         // PERSPECTIVE or ORTHOGRAPHIC
        // mutable: can be midfied in a function foo() const
        mutable glm::mat4 viewMatrix_; // Buffered model view matrix.
        mutable bool viewMatrixIsUpToDate_;
        mutable glm::mat4 projectionMatrix_; // Buffered projection matrix.
        mutable bool projectionMatrixIsUpToDate_;
        glm::vec3 sceneCenter_;
        double sceneRadius_; // OpenGL units
        double zClippingCoef_;
        double orthoCoef_;
       



    public:
        
        CameraFrame *frame() const { return frame_; }
        glm::vec3 pivotPoint() const { return frame()->pivotPoint(); }
        int screenWidth() const { return screenWidth_; }
        int screenHeight() const { return screenHeight_; }
        double aspectRatio() const { return static_cast<double>(screenWidth_) / static_cast<double>(screenHeight_); }
        double fieldOfView() const { return fieldOfView_; }
        CameraFrame::ProjectionType projType() const { return projType_; }
        glm::mat4 getProjectionMatrix() const { return projectionMatrix_; }
        glm::mat4 getViewMatrix() const { return viewMatrix_; }
        glm::mat4 getViewProjectionMatrix() const { return  projectionMatrix_ * viewMatrix_; }
        glm::vec3 sceneCenter() const { return sceneCenter_; }
        double sceneRadius() const { return sceneRadius_; }
        double distanceToSceneCenter() const {  return std::abs( glm::length(sceneCenter_ - frame()->position() ) ); }
        double zClippingCoefficient() const { return zClippingCoef_; }
        double zOrthoCoefficient() const { return orthoCoef_; }
        glm::vec3 position() const { return frame()->position(); }
        glm::vec3 upVector() const { return frame()->inverseTransformOf(glm::vec3(0.0f, 1.0f, 0.0f));  } 
        glm::vec3 viewDirection() const { return frame()->inverseTransformOf(glm::vec3(0.0f, 0.0f, -1.0f)); } 
        Quaternion orientation() const { return frame()->orientation(); }


        glm::vec3 center_;


        virtual double zNear() const
        {

            double zNear = distanceToSceneCenter() - zClippingCoefficient() * sceneRadius();

            // Prevents negative or null zNear values.
            const double zMin = 0.0f;
            if (zNear < 0.0f)
                zNear = 0.0f;

            return zNear;
        }

        virtual double zFar() const
        {
            return distanceToSceneCenter() + zClippingCoefficient() * sceneRadius();
        }


        void computeProjectionMatrix() const
        {
            if (projectionMatrixIsUpToDate_)
                return;

            const double ZNear = zNear();
            const double ZFar = zFar();

            switch (projType()) 
            {
                case CameraFrame::PERSPECTIVE: 
                {
                    const double f = 1.0 / tan(fieldOfView() / 2.0);

                    projectionMatrix_ = glm::perspective(fieldOfView(), aspectRatio(), zNear(), zFar() ); 
                    
                    break;
                }
                case CameraFrame::ORTHOGRAPHIC: 
                {
                    double halfWidth = orthoCoef_ * ((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
                    double halfHeight = orthoCoef_ * ((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);

                    projectionMatrix_ = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear(), zFar() ); 
                    break;
                }
            }

            projectionMatrixIsUpToDate_ = true;
        }

        void computeViewMatrix() const
        {
            if (viewMatrixIsUpToDate_)
                return;

            const Quaternion q = frame()->orientation();
            const glm::vec3 t = q.inverseRotate(frame()->position());
        
//std::cout<<"center_  "<<center_.x<< " " << center_.y << " " << center_.z << std::endl;     

glm::vec3 diff = glm::normalize(center_ - position());
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

            viewMatrix_ = glm::lookAt(position(),  position() - quatZ , quatU );

            viewMatrixIsUpToDate_ = true; 
        }

        
        void setScreenWidthAndHeight(int width, int height) 
        {
            // Prevent negative and zero dimensions that would cause divisions by zero.
            screenWidth_ = width > 0 ? width : 1;
            screenHeight_ = height > 0 ? height : 1;

            
            frame_->setCamParam(screenWidth_, screenHeight_, fieldOfView(), projType(), sceneRadius(), pivotPoint() ); //@@
            
            projectionMatrixIsUpToDate_ = false;
        }

        void setViewDirection(glm::vec3 &direction)
        {
            if (direction == glm::vec3(0.0f) )
                return;

            direction = glm::normalize(direction);
            glm::vec3 xAxis = glm::cross(direction , upVector() );
            if (xAxis == glm::vec3(0.0f)) 
            {
                // target is aligned with upVector, this means a rotation around X axis
                // X axis is then unchanged, let's keep it !
                xAxis = frame()->inverseTransformOf( glm::vec3(1.0, 0.0, 0.0) );
            }

            Quaternion q;
            q.setFromRotatedBasis(xAxis, glm::cross(xAxis, direction), -direction);
            //frame()->setOrientationWithConstraint(q);
            frame()->setOrientation(q);
        }

        void lookAt(const glm::vec3 &target)
        {
            setViewDirection(target - position() );
        }


        glm::vec3 cameraCoordinatesOf(const glm::vec3 &src) const { return frame()->coordinatesOf(src); }
        glm::vec3 worldCoordinatesOf(const glm::vec3 &src) const { return frame()->inverseCoordinatesOf(src); }



        virtual void getOrthoWidthHeight(double &halfWidth, double &halfHeight) const 
        {
            const double dist = orthoCoef_ * fabs(cameraCoordinatesOf(pivotPoint()).z);

            halfWidth = dist * ((aspectRatio() < 1.0) ? 1.0 : aspectRatio());
            halfHeight = dist * ((aspectRatio() < 1.0) ? 1.0 / aspectRatio() : 1.0);
        }

        /*glm::vec3 projectedCoordinatesOf(const glm::vec3 &src, const Frame *frame = NULL) const;
          glm::vec3 unprojectedCoordinatesOf(const glm::vec3 &src, const Frame *frame = NULL) const;
          void getProjectedCoordinatesOf(const double src[3], double res[3], const Frame *frame = NULL) const;
          void getUnprojectedCoordinatesOf(const double src[3], double res[3], const Frame *frame = NULL) const;
          void convertClickToLine(const QPoint &pixel, glm::vec3 &orig, glm::vec3 &dir) const;
          glm::vec3 pointUnderPixel(const QPoint &pixel, bool &found) const;*/



        //glm::vec3 projectedCoordinatesOf(const glm::vec3 &src, const Frame *frame) const 
        //{
        //    double x, y, z;
        //    /*static GLint viewport[4];
        //    getViewport(viewport);

        //    if (frame) 
        //    {
        //        const Vec tmp = frame->inverseCoordinatesOf(src);
        //        gluProject(tmp.x, tmp.y, tmp.z, modelViewMatrix_, projectionMatrix_, viewport, &x, &y, &z);
        //    } 
        //    else
        //        gluProject(src.x, src.y, src.z, modelViewMatrix_, projectionMatrix_, viewport, &x, &y, &z);*/

        //    return glm::vec3(x, y, z);
        //}

        //glm::vec3 unprojectedCoordinatesOf(const glm::vec3 &src, const Frame *frame) const 
        //{
        //    double x, y, z;
        //    /*static GLint viewport[4];
        //    getViewport(viewport);
        //    gluUnProject(src.x, src.y, src.z, modelViewMatrix_, projectionMatrix_, viewport, &x, &y, &z);
        //    if (frame)
        //        return frame->coordinatesOf(Vec(x, y, z));
        //    else*/
        //        return glm::vec3(x, y, z);
        //}

        //void getProjectedCoordinatesOf(const glm::vec3 src, glm::vec3 &res, const Frame *frame) const 
        //{
        //    glm::vec3 r = projectedCoordinatesOf(glm::vec3(src), frame);
        //    for (int i = 0; i < 3; ++i)
        //        res[i] = r[i];
        //}

        //void getUnprojectedCoordinatesOf(const glm::vec3 src, glm::vec3 &res, const Frame *frame) const 
        //{
        //    glm::vec3 r = unprojectedCoordinatesOf(glm::vec3(src), frame);
        //    for (int i = 0; i < 3; ++i)
        //        res[i] = r[i];
        //}


        void mouseMoveEvent(QMouseEvent *const event)
        {
            glm::vec3 centerTranslat(0.0f);
            frame()->mouseMoveEvent(event, frame()->coordinatesOf(pivotPoint()), center_ );

        }


    public Q_SLOTS:


        void onFrameModified()
        {
            projectionMatrixIsUpToDate_ = false;
            viewMatrixIsUpToDate_ = false;
        }


        void setFrame(CameraFrame *const mcf)
        {
            if (!mcf)
            return;

            if (frame_) 
            {
                disconnect(frame_, SIGNAL(modified()), this, SLOT(onFrameModified()));
            }

            frame_ = mcf;

            connect(frame_, SIGNAL(modified()), this, SLOT(onFrameModified()));
            onFrameModified();
        }


        void setPivotPoint(const glm::vec3 &point)
        {
            const float prevDist = std::abs(cameraCoordinatesOf(pivotPoint()).z);

            // If frame's RAP is set directly, projectionMatrixIsUpToDate_ should also be
            // set to false to ensure proper recomputation of the ORTHO projection matrix.
            //frame()->setPivotPoint(point);

            // orthoCoef_ is used to compensate for changes of the pivotPoint, so that the
            // image does not change when the pivotPoint is changed in ORTHOGRAPHIC mode.
            const float newDist = std::abs(cameraCoordinatesOf(pivotPoint()).z);
            // Prevents division by zero when rap is set to camera position
            if ((prevDist > std::numeric_limits<float>::epsilon()) && (newDist > std::numeric_limits<float>::epsilon()))
                orthoCoef_ *= prevDist / newDist;

            projectionMatrixIsUpToDate_ = false;
        }


        void setAspectRatio(double aspect) 
        {
            setScreenWidthAndHeight(int(100.0 * aspect), 100);
        }

        void setFieldOfView(double fov)
        {
            fieldOfView_ = fov;
            projectionMatrixIsUpToDate_ = false;
        }

        void setFOVToFitScene()
        {
            if (distanceToSceneCenter() > sqrt(2.0) * sceneRadius())
                setFieldOfView(2.0 * asin(sceneRadius() / distanceToSceneCenter()));
            else
                setFieldOfView(M_PI / 2.0);
        }

        void setProjType(CameraFrame::ProjectionType type)
        {
            if ((type == CameraFrame::ORTHOGRAPHIC) && (projType_ == CameraFrame::PERSPECTIVE))
                orthoCoef_ = tan(fieldOfView() / 2.0);

            projType_ = type;
            projectionMatrixIsUpToDate_ = false;
        }


        void setSceneCenter(const glm::vec3 &center)
        {
            sceneCenter_ = center;
            setPivotPoint(sceneCenter());
            projectionMatrixIsUpToDate_ = false;
        }

        /*bool setSceneCenterFromPixel(const QPoint &pixel)
        {
            sceneCenter_ = center;
            setPivotPoint(sceneCenter());
            projectionMatrixIsUpToDate_ = false;
        }*/


        void setSceneRadius(double radius)
        {
            if (radius <= 0.0) 
            {
                std::cerr<<"Scene radius must be positive - Ignoring value"<<std::endl;
                return;
            }
            sceneRadius_ = radius;
            projectionMatrixIsUpToDate_ = false;
        }

        void setSceneBoundingBox(const glm::vec3 &min, const glm::vec3 &max)
        {
            setSceneCenter((min + max) / 2.0f);
            setSceneRadius(0.5f * glm::length( max - min ) );
        }


        void setZClippingCoefficient(double coef) 
        {
            zClippingCoef_ = coef;
            projectionMatrixIsUpToDate_ = false;
        }

        void setPosition(const glm::vec3 &pos) { frame()->setPosition(pos); }

        void setOrientation(const Quaternion &q)
        {
            frame()->setOrientation(q);
            frame()->updateSceneUpVector();
        }

        void setOrientation(double theta, double phi)
        {
            glm::vec3 axis(0.0, 1.0, 0.0);
            const Quaternion rot1(axis, theta);
            axis = glm::vec3(-cos(theta), 0.0, sin(theta));
            const Quaternion rot2(axis, phi);
            setOrientation(rot1 * rot2);
        }

        void setUpVector(const glm::vec3 &up, bool noMove = true)
        {
           /* Quaternion q( glm::vec3(0.0, 1.0, 0.0), frame()->transformOf(up));

            if (!noMove)
                frame()->setPosition(pivotPoint() - (frame()->orientation() * q).rotate(frame()->coordinatesOf(pivotPoint())));

            frame()->rotate(q);*/

            // Useful in fly mode to keep the horizontal direction.
            frame()->updateSceneUpVector();
        }

 
        void fitSphere(const glm::vec3 &center, double radius)
        {
            float distance = 0.0;
            switch (projType()) 
            {
                case CameraFrame::PERSPECTIVE: 
                {
                    distance = radius / sin(fieldOfView() / 2.0);
                    break;
                }
                case CameraFrame::ORTHOGRAPHIC: 
                {
                    distance = glm::dot( (center - pivotPoint()), viewDirection()) + (radius / orthoCoef_);
                    break;
                }
            }
            glm::vec3 newPos(center - (viewDirection() * distance) );
            //frame()->setPositionWithConstraint(newPos);
            frame()->setPosition(newPos);
        }

        void showEntireScene() { fitSphere(sceneCenter(), sceneRadius()); }

        

        void fitBoundingBox(const glm::vec3 &min, const glm::vec3 &max)
        {
            float diameter = std::max( std::abs(max[1] - min[1]),  std::abs(max[0] - min[0]));
            diameter = std::max( std::abs(max[2] - min[2]), diameter);
            fitSphere(0.5f * (min + max), 0.5f * diameter);
        }

        void centerScene(){ frame()->projectOnLine(sceneCenter(), viewDirection()); } 



    public:

        Camera() : frame_(NULL), fieldOfView_(M_PI / 4.0), viewMatrixIsUpToDate_(false), projectionMatrixIsUpToDate_(false)
        {
            setFrame(new CameraFrame());
            setSceneRadius(1.0);
            orthoCoef_ = tan(fieldOfView() / 2.0);

            setSceneCenter(glm::vec3(0.0f));

            setProjType(CameraFrame::PERSPECTIVE);

            setZClippingCoefficient(sqrt(3.0));

            setScreenWidthAndHeight(600, 400);

            viewMatrix_ = glm::mat4(1.0f);
            projectionMatrix_ = glm::mat4(1.0f);

            computeProjectionMatrix();
        }


        virtual ~Camera()
        {
            delete frame_;
        }

        Camera(const Camera &camera) : QObject(), frame_(nullptr) 
        {
            setFrame(new CameraFrame(*camera.frame()));

            viewMatrix_ = glm::mat4(1.0f);
            projectionMatrix_ = glm::mat4(1.0f);

            (*this) = camera;
        }

        Camera &operator=(const Camera &camera)
        {
            setScreenWidthAndHeight(camera.screenWidth(), camera.screenHeight());
            setFieldOfView(camera.fieldOfView());
            setSceneRadius(camera.sceneRadius());
            setSceneCenter(camera.sceneCenter());
            setZClippingCoefficient(camera.zClippingCoefficient());
            setProjType(camera.projType());


            orthoCoef_ = camera.orthoCoef_;
            projectionMatrixIsUpToDate_ = false;

            // frame_ and interpolationKfi_ pointers are not shared.
            //frame_->setReferenceFrame(nullptr);
            frame_->setPosition(camera.position());
            frame_->setOrientation(camera.orientation());

            frame_->setCamParam(camera.screenWidth(), camera.screenHeight(), camera.fieldOfView(), camera.projType(), camera.sceneRadius(), camera.pivotPoint() );
      
            computeProjectionMatrix();
            computeViewMatrix();

            return *this;
        }



};

} // namespace qgltoolkit

#endif // QGLTOOLKIT_CAMERA_H
