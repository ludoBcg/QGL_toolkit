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

#ifndef QGLVIEWER_MANIPULATED_CAMERA_FRAME_H
#define QGLVIEWER_MANIPULATED_CAMERA_FRAME_H

#include <QDateTime>
#include <QString>
#include <QTimer>
#include <QObject>
#include <QMap>
#include <QPoint>
#include <QMouseEvent>


#include "frame.h"

//#include "mouseGrabber.h"




namespace qglviewer {


        
class CameraFrame : public qglviewer::Frame //, public MouseGrabber
{

    //friend class Camera;
    //friend class ::QGLViewer;

    Q_OBJECT


    public:
        enum ProjectionType { PERSPECTIVE, ORTHOGRAPHIC };

         enum KeyboardAction {
            DRAW_AXIS,
            DRAW_GRID,
            DISPLAY_FPS,
            ENABLE_TEXT,
            EXIT_VIEWER,
            SAVE_SCREENSHOT,
            CAMERA_MODE,
            FULL_SCREEN,
            STEREO,
            ANIMATION,
            HELP,
            EDIT_CAMERA,
            MOVE_CAMERA_LEFT,
            MOVE_CAMERA_RIGHT,
            MOVE_CAMERA_UP,
            MOVE_CAMERA_DOWN,
            INCREASE_FLYSPEED,
            DECREASE_FLYSPEED,
            SNAPSHOT_TO_CLIPBOARD
            };

        enum MouseHandler { CAMERA, FRAME };


        enum ClickAction {
            NO_CLICK_ACTION,
            ZOOM_ON_PIXEL,
            ZOOM_TO_FIT,
            SELECT,
            RAP_FROM_PIXEL,
            RAP_IS_CENTER,
            CENTER_FRAME,
            CENTER_SCENE,
            SHOW_ENTIRE_SCENE,
            ALIGN_FRAME,
            ALIGN_CAMERA
            };


        enum MouseAction {
            NO_MOUSE_ACTION,
            ROTATE,
            ZOOM,
            TRANSLATE,
            MOVE_FORWARD,
            LOOK_AROUND,
            MOVE_BACKWARD,
            SCREEN_ROTATE,
            ROLL,
            DRIVE,
            SCREEN_TRANSLATE,
            ZOOM_ON_REGION
            };


    private:

        bool grabsMouse_;

        // Sensitivity
        double rotationSensitivity_;
        double translationSensitivity_;
        double spinningSensitivity_;
        double wheelSensitivity_;
        double zoomSensitivity_;

        // Mouse speed 
        QTime last_move_time;
        double mouseSpeed_;
        int delay_;

        bool dirIsFixed_;

        // MouseGrabber
        bool keepsGrabbingMouse_;



        glm::vec3 sceneUpVector_;

        bool rotatesAroundUpVector_;

        bool constrainedRotationIsReversed_;

        bool zoomsOnPivotPoint_;

        glm::vec3 pivotPoint_;

        
        // C a m e r a   p a r a m e t e r s
        int screenWidth_, screenHeight_; // size of the window, in pixels
        double fieldOfView_;              // in radians
        ProjectionType projType_; 
        double sceneRadius_;

        


    protected:
            
        /*QGLViewer::*/MouseAction action_;
        Constraint *previousConstraint_; 
        QPoint prevPos_, pressPos_;

    public:

        void setCamParam(int screenWidth, int  screenHeight, double fov, ProjectionType projType, double sceneRadius, glm::vec3 pivotPoint)
        { 
            screenWidth_ = screenWidth;  
            screenHeight_ = screenHeight;  
            fieldOfView_ = fov;
            projType_ = projType;
            sceneRadius_ = sceneRadius;
            pivotPoint_ = pivotPoint;
        }


        CameraFrame()
        : sceneUpVector_(0.0, 1.0, 0.0), rotatesAroundUpVector_(false), zoomsOnPivotPoint_(false) 
        {
            setRotationSensitivity(1.0f);
            setTranslationSensitivity(1.0f);
            setWheelSensitivity(1.0f);
            setZoomSensitivity(1.0f);

            setCamParam(0, 0, 0, PERSPECTIVE, 0, glm::vec3(0.0) );

            previousConstraint_ = nullptr;
            grabsMouse_ = false;
        }

        virtual ~CameraFrame() {}

        CameraFrame &operator=(const CameraFrame &mcf)
        {
            Frame::operator=(mcf);

            setRotationSensitivity(mcf.rotationSensitivity());
            setTranslationSensitivity(mcf.translationSensitivity());
            setWheelSensitivity(mcf.wheelSensitivity());
            setZoomSensitivity(mcf.zoomSensitivity());

            mouseSpeed_ = 0.0f;
            dirIsFixed_ = false;
            keepsGrabbingMouse_ = false;
            action_ = /*QGLViewer::*/NO_MOUSE_ACTION;

            setSceneUpVector(mcf.sceneUpVector());
            setRotatesAroundUpVector(mcf.rotatesAroundUpVector_);
            setZoomsOnPivotPoint(mcf.zoomsOnPivotPoint_);

            setCamParam(mcf.screenWidth_, mcf.screenHeight_, mcf.fieldOfView_, mcf.projType_ , mcf.sceneRadius_, mcf.pivotPoint_);
            grabsMouse_ = mcf.grabsMouse_;

            return *this;
        }


        CameraFrame(const CameraFrame &mcf)
        : Frame(mcf) 
        {
           
            (*this) = (mcf);
        }


        bool grabsMouse() const { return grabsMouse_; }
        void setGrabsMouse(bool grabs) { grabsMouse_ = grabs; }

        glm::vec3 pivotPoint() const { return pivotPoint_; }

        void setPivotPoint(const glm::vec3 &point) { pivotPoint_ = point; }


        bool rotatesAroundUpVector() const { return rotatesAroundUpVector_; }

        void setRotatesAroundUpVector(bool constrained) { rotatesAroundUpVector_ = constrained; }

 
        bool zoomsOnPivotPoint() const { return zoomsOnPivotPoint_; }

        void setZoomsOnPivotPoint(bool enabled) { zoomsOnPivotPoint_ = enabled; }


        glm::vec3 sceneUpVector() const { return sceneUpVector_; }

        double rotationSensitivity() const { return rotationSensitivity_; }
 
        double translationSensitivity() const { return translationSensitivity_; }
  
        double zoomSensitivity() const { return zoomSensitivity_; }
  
        double wheelSensitivity() const { return wheelSensitivity_; }


        bool isManipulated() const { return action_ != /*QGLViewer::*/NO_MOUSE_ACTION; }
 
        /*QGLViewer::*/MouseAction currentMouseAction() const { return action_; }
  
        virtual void checkIfGrabsMouse(int x, int y, /*const Camera *const camera,*/ glm::vec3 projPos)
        {
            const int thresold = 10;
            const glm::vec3 proj = projPos/*camera->projectedCoordinatesOf(position())*/;
            setGrabsMouse(keepsGrabbingMouse_ || ((fabs(x - proj.x) < thresold) && (fabs(y - proj.y) < thresold)));
        }

    Q_SIGNALS:
  
        void manipulated();


    public Q_SLOTS:

        void setRotationSensitivity(double sensitivity) { rotationSensitivity_ = sensitivity; }

        void setTranslationSensitivity(double sensitivity) { translationSensitivity_ = sensitivity; }

        void setWheelSensitivity(double sensitivity) { wheelSensitivity_ = sensitivity; }

        void setZoomSensitivity(double sensitivity) { zoomSensitivity_ = sensitivity; }

        void setSceneUpVector(const glm::vec3 &up) { sceneUpVector_ = up; }

        void updateSceneUpVector()
        {
            sceneUpVector_ = inverseTransformOf( glm::vec3(0.0, 1.0, 0.0) );
        }

    private:

        void zoom(double delta, /*const Camera *const camera*/ glm::vec3 CamCoordPivot)
        {
            const float sceneRadius = /*camera->sceneRadius()*/ sceneRadius_;
            //if (zoomsOnPivotPoint_) 
            {
                std::cout<<"zoom on pivot" << std::endl;
                glm::vec3 direction = position() - /*camera->pivotPoint()*/pivotPoint_;
                if (glm::length(direction) > 0.02 * sceneRadius || delta > 0.0)
                    translate( (float)delta * direction);

                std::cout<<"Frame position = " << this->position().x << " "<< this->position().y << " "<< this->position().z<< std::endl;
            } 
            //else 
            //{std::cout<<"zoom not on  pivot" << std::endl;
            //    const float coef = std::max( std::abs(   /*(camera->frame()->coordinatesOf(camera->pivotPoint()))*/CamCoordPivot.z), 0.2f * sceneRadius);
            //    glm::vec3 trans(0.0, 0.0, -coef * delta);
            //    translate(inverseTransformOf(trans));
            //}
        }


        Quaternion turnQuaternion(int x/*, const Camera *const camera*/)
        {
            return Quaternion( glm::vec3(0.0, 1.0, 0.0), rotationSensitivity() * (prevPos_.x() - x) / screenWidth_/*camera->screenWidth()*/);
        }


        Quaternion pitchYawQuaternion(int x, int y/*, const Camera *const camera*/)
        {
            const Quaternion rotX( glm::vec3(1.0, 0.0, 0.0), rotationSensitivity() * (prevPos_.y() - y) / screenHeight_ /*camera->screenHeight()*/);
            const Quaternion rotY(transformOf(sceneUpVector()), rotationSensitivity() * (prevPos_.x() - x) / screenWidth_ /*camera->screenWidth()*/);
            return rotY * rotX;
        }
        


    protected:


        static double projectOnBall(double x, double y) 
        {
            // If you change the size value, change angle computation in
            // deformedBallQuaternion().
            const double size = 1.0;
            const double size2 = size * size;
            const double size_limit = size2 * 0.5;

            const double d = x * x + y * y;
            return d < size_limit ? sqrt(size2 - d) : size_limit / sqrt(d);
        }


        Quaternion deformedBallQuaternion(int x, int y, double cx, double cy/*, const Camera *const camera*/)
        {
            // Points on the deformed ball
            double px = rotationSensitivity() * (prevPos_.x() - cx) / screenWidth_ /*camera->screenWidth()*/;
            double py = rotationSensitivity() * (cy - prevPos_.y()) / screenHeight_ /*camera->screenHeight()*/;
            double dx = rotationSensitivity() * (x - cx) / screenWidth_ /*camera->screenWidth()*/;
            double dy = rotationSensitivity() * (cy - y) / screenHeight_ /*camera->screenHeight()*/;

            const glm::vec3 p1(px, py, projectOnBall(px, py));
            const glm::vec3 p2(dx, dy, projectOnBall(dx, dy));
            // Approximation of rotation angle
            // Should be divided by the projectOnBall size, but it is 1.0
            const glm::vec3 axis = glm::cross(p2, p1);
            const double angle = 5.0 * asin(sqrt( squaredNorm(axis) / squaredNorm(p1) / squaredNorm(p2)));
            return Quaternion(axis, angle);
        }


        void computeMouseSpeed(const QMouseEvent *const e)
        {
            const QPoint delta = (e->pos() - prevPos_);
            const double dist = sqrt( double(delta.x() * delta.x() + delta.y() * delta.y()));
            delay_ = last_move_time.restart();
            if (delay_ == 0)
                mouseSpeed_ = dist;
            else
                mouseSpeed_ = dist / delay_;
        }


        int mouseOriginalDirection(const QMouseEvent *const e)
        {
            static bool horiz = true; // Two simultaneous manipulatedFrame require two mice !

            if (!dirIsFixed_) 
            {
                const QPoint delta = e->pos() - pressPos_;
                dirIsFixed_ = abs(delta.x()) != abs(delta.y());
                horiz = abs(delta.x()) > abs(delta.y());
            }

            if (dirIsFixed_)
            {
                if (horiz)
                    return 1;
                else
                    return -1;
            }
            else
                return 0;
        }



        double deltaWithPrevPos(QMouseEvent *const event/*, Camera *const camera*/) const
        {
            double dx = double(event->x() - prevPos_.x()) / screenWidth_ /*camera->screenWidth()*/;
            double dy = double(event->y() - prevPos_.y()) / screenHeight_ /*_camera->screenHeight()*/;

            double value = std::abs(dx) > std::abs(dy) ? dx : dy;
            return value * zoomSensitivity();
        }


        double wheelDelta(const QWheelEvent *event) const
        {
            static const double WHEEL_SENSITIVITY_COEF = 8E-4;
            return event->delta() * wheelSensitivity() * WHEEL_SENSITIVITY_COEF;
        }




    public:


        virtual void mousePressEvent(QMouseEvent *const event/*, Camera *const camera*/) 
        {
            //Q_UNUSED(camera);

            if (grabsMouse())
                keepsGrabbingMouse_ = true;

            prevPos_ = pressPos_ = event->pos();

            std::cout<<"mouse pressed"<<std::endl;
        }

        
        virtual void mouseReleaseEvent(QMouseEvent *const event/*, Camera *const camera*/)
        {
            Q_UNUSED(event);
            //Q_UNUSED(camera);

            keepsGrabbingMouse_ = false;

            if (previousConstraint_)
                setConstraint(previousConstraint_);

            action_ = /*QGLViewer::*/NO_MOUSE_ACTION;
        }
        

        
        virtual void mouseMoveEvent(QMouseEvent *const event, /*Camera *const camera*/ glm::vec3 camPivotPoint, glm::vec3 &center )
        {
            std::cout<<"cameraFrame::mouseMoveEvent action = "<< action_ <<std::endl;
            switch (action_) 
            {
                case /*QGLViewer::*/TRANSLATE: 
                {
                    //const QPoint delta = prevPos_ - event->pos();
const QPoint delta = event->pos() - prevPos_;
                    glm::vec3 trans(delta.x(), -delta.y(), 0.0);
                    // Scale to fit the screen mouse displacement
                    switch (/*camera->projType()*/projType_) 
                    {
                        case /*Camera::*/PERSPECTIVE:
                            std::cout<<"perspective"<<std::endl;
                            std::cout<<"fieldOfView_"<< fieldOfView_ <<std::endl;
                            trans *= 2.0 * tan(/*camera->fieldOfView()*/fieldOfView_ / 2.0) * std::abs(( /*camera->frame()*/this->coordinatesOf(pivotPoint())).z) / screenHeight_/*camera->screenHeight()*/;
                            break;
                        case /*Camera::*/ORTHOGRAPHIC: 
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
                    center = center + inverseTransformOf( (float)translationSensitivity() * -trans);

    //                    trans = this->orientation().rotate(/*(float)translationSensitivity()*/1.0f * trans);
    //// And then down to frame
    //if (referenceFrame())
    //{
    //    std::cout<<"referenceFrame"<<std::endl;
    //  trans = referenceFrame()->transformOf(trans);
    //}
    //translate(trans);
                    break;
                }

            
                case /*QGLViewer::*/ZOOM: 
                {
                    //zoom(deltaWithPrevPos(event, camera), camera);
                    break;
                }


                case /*QGLViewer::*/ROTATE: 
                {
                    Quaternion rot;
                    if (rotatesAroundUpVector_) 
                    {
                        // Multiply by 2.0 to get on average about the same speed as with the
                        // deformed ball
                        double dx = 2.0 * rotationSensitivity() * (prevPos_.x() - event->x()) / screenWidth_ /*camera->screenWidth()*/;
                        double dy = 2.0 * rotationSensitivity() * (prevPos_.y() - event->y()) / screenHeight_ /*camera->screenHeight()*/;
                        if (constrainedRotationIsReversed_)
                            dx = -dx;
                        glm::vec3 verticalAxis = transformOf(sceneUpVector_);
                        rot = Quaternion(verticalAxis, dx) * Quaternion( glm::vec3(1.0, 0.0, 0.0), dy);
                    } 
                    else 
                    {   std::cout<<"cameraFrame::mouseMoveEvent rotate around pivot "<<std::endl;
                    std::cout<<"camPivotPoint  "<< camPivotPoint.x << " " << camPivotPoint.y << " " << camPivotPoint.z << std::endl;  
                        glm::vec3 trans = camPivotPoint;//camera->projectedCoordinatesOf(pivotPoint());
                        rot = deformedBallQuaternion(event->x(), event->y(), trans[0], trans[1]/*, camera*/);
                        
                        rotateAroundPoint(rot, pivotPoint(), center); // 27.07 @@@@@@@@
                    }
                    
                    computeMouseSpeed(event);

                    

                    break;
                }

                case /*QGLViewer::*/SCREEN_ROTATE: 
                {
                    /*glm::vec3 trans = camera->projectedCoordinatesOf(pivotPoint());

                    const double angle = atan2(event->y() - trans[1], event->x() - trans[0]) - atan2(prevPos_.y() - trans[1], prevPos_.x() - trans[0]);

                    Quaternion rot( glm::vec3(0.0, 0.0, 1.0), angle);

                    computeMouseSpeed(event);

                    updateSceneUpVector();*/
                    break;
                }

                case /*QGLViewer::*/ROLL: 
                {
                    const double angle = M_PI * (event->x() - prevPos_.x()) / screenWidth_ /*camera->screenWidth()*/;
                    Quaternion rot(glm::vec3(0.0, 0.0, 1.0), angle);
                    rotate(rot);
                    updateSceneUpVector();
                    break;
                }

                case /*QGLViewer::*/SCREEN_TRANSLATE: 
                {
                    glm::vec3 trans;
                    int dir = mouseOriginalDirection(event);
                    if (dir == 1)
                        trans = glm::vec3(prevPos_.x() - event->x(), 0.0, 0.0);
                    else if (dir == -1)
                        trans = glm::vec3(0.0, event->y() - prevPos_.y(), 0.0);

                    switch (/*camera->projType()*/projType_) 
                    {
                        case /*Camera::*/PERSPECTIVE:
                            trans *= 2.0 * tan(/*camera->fieldOfView()*/ fieldOfView_ / 2.0) * std::abs((/*camera->frame()*/this->coordinatesOf(pivotPoint())).z) / screenHeight_/*camera->screenHeight()*/;
                            break;
                        case /*Camera::*/ORTHOGRAPHIC: 
                        {
                            /*double w, h;
                            camera->getOrthoWidthHeight(w, h);
                            trans[0] *= 2.0 * w / camera->screenWidth();
                            trans[1] *= 2.0 * h / camera->screenHeight();*/
                            break;
                        }
                    }

                    translate(inverseTransformOf( (float)translationSensitivity() * trans));
                    break;
                }

                case /*QGLViewer::*/NO_MOUSE_ACTION:
                    break;
            }

            if (action_ != /*QGLViewer::*/NO_MOUSE_ACTION) 
            {
                prevPos_ = event->pos();
                if (action_ != /*QGLViewer::*/ZOOM_ON_REGION)
                {
                    // ZOOM_ON_REGION should not emit manipulated().
                    // prevPos_ is used to draw rectangle feedback.
                    Q_EMIT manipulated();
                }
            }
        }
        

        virtual void mouseDoubleClickEvent(QMouseEvent *const event/*, Camera *const camera*/, glm::vec3 camPos, glm::vec3 camViewDir, glm::vec3 camPivotPoint) 
        {
Frame *frame = new Frame();
frame->setTranslation(camPivotPoint);
            if (event->modifiers() == Qt::NoModifier)
                switch (event->button()) 
                {
                    case Qt::LeftButton:

alignWithFrame(frame, true);
                        //alignWithFrame(/*camera->frame()*/this);
                        std::cout<<"double left ++++++++++"<<std::endl;
                        break;
                    case Qt::RightButton:
                        projectOnLine(/*camera*/ /*this->position()*/ camPos, /*camera->viewDirection()*/ camViewDir/*this->inverseTransformOf(glm::vec3(0.0f, 0.0f, -1.0f))*/ );
                        std::cout<<"double right --------"<<std::endl;
                        break;
                    default:
                        break;
                }
        }


        virtual void wheelEvent(QWheelEvent *const event, /*Camera *const camera*/ glm::vec3 camPivotPoint )
        {

            if (action_ == /*QGLViewer::*/ZOOM) 
            {
                zoom(wheelDelta(event), /*camera*/ camPivotPoint);
                Q_EMIT manipulated();
            }

            if (previousConstraint_)
                setConstraint(previousConstraint_);

            action_ = /*QGLViewer::*/NO_MOUSE_ACTION;
        }



        virtual void startAction( int ma, bool withConstraint = true) // int is really a QGLViewer::MouseAction
        {

            action_ = (/*QGLViewer::*/MouseAction)(ma);

            if (withConstraint)
                previousConstraint_ = nullptr;
            else 
            {
                previousConstraint_ = constraint();
                setConstraint(nullptr);
            }

            switch (action_) 
            {
               
                case /*QGLViewer::*/SCREEN_ROTATE:
                    mouseSpeed_ = 0.0;
                    std::cout<<"screen rotate"<<std::endl;
                    break;

                case /*QGLViewer::*/SCREEN_TRANSLATE:
                    dirIsFixed_ = false;
                    std::cout<<" screen translate"<<std::endl;
                    break;

                case /*QGLViewer::*/ROTATE:
                    constrainedRotationIsReversed_ = transformOf(sceneUpVector_).y < 0.0;
                    std::cout<<" rotate"<<std::endl;
                    break;

                default:
                    break;
            }
        }

      

};

} // namespace qglviewer

#endif // QGLVIEWER_MANIPULATED_CAMERA_FRAME_H
