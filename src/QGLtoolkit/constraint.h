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

#ifndef QGLVIEWER_CONSTRAINT_H
#define QGLVIEWER_CONSTRAINT_H


#include "quaternion.h"
//#include "frame.h"
//#include "camera.h"

namespace qglviewer {
//class Frame;
//class Camera;



class  Constraint 
{
    public:

        virtual ~Constraint() {}

 
        virtual void constrainTranslation(glm::vec3 &translation/*, Frame *const frame*/) 
        {
            Q_UNUSED(translation);
            //Q_UNUSED(frame);
        }
 
        virtual void constrainRotation(Quaternion &rotation/*, Frame *const frame*/) 
        {
            Q_UNUSED(rotation);
            //Q_UNUSED(frame);
        }
};



/*
class AxisPlaneConstraint : public Constraint 
{
        
    public:

        enum Type { FREE, AXIS, PLANE, FORBIDDEN };


    private:

        Type translationConstraintType_;
        Type rotationConstraintType_;

        glm::vec3 translationConstraintDir_;
        glm::vec3 rotationConstraintDir_;


    public:

        AxisPlaneConstraint()
        : translationConstraintType_(FREE), rotationConstraintType_(FREE) 
        {
            // Do not use set since setRotationConstraintType needs a read.
        };

        virtual ~AxisPlaneConstraint() {}

  
        virtual void constrainTranslation(glm::vec3 &translation, Frame *const frame) 
        {
            Q_UNUSED(translation);
            Q_UNUSED(frame);
        };

        void setTranslationConstraint(Type type, const glm::vec3 &direction)
        {
            setTranslationConstraintType(type);
            setTranslationConstraintDirection(direction);
        }

        void setTranslationConstraintType(Type type) 
        {
            translationConstraintType_ = type;
        };

        void setTranslationConstraintDirection(const glm::vec3 &direction)
        {
            if ((translationConstraintType() != AxisPlaneConstraint::FREE) && (translationConstraintType() != AxisPlaneConstraint::FORBIDDEN)) 
            {
                const float norm = glm::length(direction);
                if (direction == glm::vec3(0.0f) ) 
                {
                    std::cerr << "AxisPlaneConstraint::setTranslationConstraintDir: null vector " << std::endl;
                    translationConstraintType_ = AxisPlaneConstraint::FREE;
                } 
                else
                    translationConstraintDir_ = direction / norm;
            }
        }

  
        Type translationConstraintType() const { return translationConstraintType_; };

        glm::vec3 translationConstraintDirection() const 
        {
            return translationConstraintDir_;
        };
  
        virtual void constrainRotation(Quaternion &rotation, Frame *const frame) 
        {
            Q_UNUSED(rotation);
            Q_UNUSED(frame);
        };

        void setRotationConstraintType(Type type)
        {
            if (rotationConstraintType() == AxisPlaneConstraint::PLANE) 
            {
                std::cerr << "AxisPlaneConstraint::setRotationConstraintType: the PLANE type cannot be used for a rotation constraints" << std::endl;
                return;
            }

            rotationConstraintType_ = type;
        }


        void setRotationConstraint(Type type, const glm::vec3 &direction)
        {
            setRotationConstraintType(type);
            setRotationConstraintDirection(direction);
        }



        void setRotationConstraintDirection(const glm::vec3 &direction)
        {
            if ((rotationConstraintType() != AxisPlaneConstraint::FREE) && (rotationConstraintType() != AxisPlaneConstraint::FORBIDDEN)) 
            {
                const float norm = glm::length(direction);
                if (direction == glm::vec3(0.0f) ) 
                {
                    std::cerr << "AxisPlaneConstraint::setRotationConstraintDir: null vector for rotation constraint" << std::endl;
                    rotationConstraintType_ = AxisPlaneConstraint::FREE;
                } 
                else
                    rotationConstraintDir_ = direction / norm;
            }
        }


        Type rotationConstraintType() const { return rotationConstraintType_; };
  
        glm::vec3 rotationConstraintDirection() const { return rotationConstraintDir_; };

};




class LocalConstraint : public AxisPlaneConstraint 
{

    public:

        virtual ~LocalConstraint(){};

        virtual void constrainTranslation(glm::vec3 &translation, Frame *const frame)
        {
            glm::vec3 proj;
            switch (translationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    proj = frame->rotation().rotate(translationConstraintDirection());
                    translation.projectOnPlane(proj);
                    break;

                case AxisPlaneConstraint::AXIS:
                    proj = frame->rotation().rotate(translationConstraintDirection());
                    translation.projectOnAxis(proj);
                    break;

                case AxisPlaneConstraint::FORBIDDEN:
                    translation = glm::vec3(0.0, 0.0, 0.0);
                    break;
            }
        }


        virtual void constrainRotation(Quaternion &rotation, Frame *const frame)
        {
            switch (rotationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    break;

                case AxisPlaneConstraint::AXIS: 
                {
                    glm::vec3 axis = rotationConstraintDirection();
                    glm::vec3 quat = glm::vec3(rotation[0], rotation[1], rotation[2]);
                    quat.projectOnAxis(axis);
                    rotation = Quaternion(quat, 2.0 * acos(rotation[3]));
                } break;

                case AxisPlaneConstraint::FORBIDDEN:
                    rotation = Quaternion(); // identity
                    break;
            }
        }


};





class WorldConstraint : public AxisPlaneConstraint 
{
    public:

        virtual ~WorldConstraint(){};

        virtual void constrainTranslation(glm::vec3 &translation, Frame *const frame)
        {
            glm::vec3 proj;
            switch (translationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    if (frame->referenceFrame()) 
                    {
                        proj = frame->referenceFrame()->transformOf(
                        translationConstraintDirection());
                        translation.projectOnPlane(proj);
                    } 
                    else
                        translation.projectOnPlane(translationConstraintDirection());
                    break;

                case AxisPlaneConstraint::AXIS:
                    if (frame->referenceFrame()) 
                    {
                        proj = frame->referenceFrame()->transformOf(translationConstraintDirection());
                        translation.projectOnAxis(proj);
                    } 
                    else
                        translation.projectOnAxis(translationConstraintDirection());
                    break;

                case AxisPlaneConstraint::FORBIDDEN:
                    translation = glm::vec3(0.0, 0.0, 0.0);
                    break;
            }
        }


        virtual void constrainRotation(Quaternion &rotation, Frame *const frame)
        {
            switch (rotationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    break;

                case AxisPlaneConstraint::AXIS: 
                {
                    glm::vec3 quat(rotation[0], rotation[1], rotation[2]);
                    glm::vec3 axis = frame->transformOf(rotationConstraintDirection());
                    quat.projectOnAxis(axis);
                    rotation = Quaternion(quat, 2.0 * acos(rotation[3]));
                    break;
                }
                case AxisPlaneConstraint::FORBIDDEN:
                    rotation = Quaternion(); // identity
                    break;
            }
        }

};


class  CameraConstraint : public AxisPlaneConstraint 
{
    private:

        const Camera *const camera_;


    public:

        explicit CameraConstraint(const Camera *const camera)
        : AxisPlaneConstraint(), camera_(camera) {}

        virtual ~CameraConstraint(){};

        const Camera *camera() const { return camera_; };

        virtual void constrainTranslation(glm::vec3 &translation, Frame *const frame)
        {
            glm::vec3 proj;
            switch (translationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    proj = camera()->frame()->inverseTransformOf(translationConstraintDirection());
                    if (frame->referenceFrame())
                        proj = frame->referenceFrame()->transformOf(proj);
                    translation.projectOnPlane(proj);
                    break;

                case AxisPlaneConstraint::AXIS:
                    proj = camera()->frame()->inverseTransformOf(translationConstraintDirection());
                    if (frame->referenceFrame())
                        proj = frame->referenceFrame()->transformOf(proj);
                    translation.projectOnAxis(proj);
                    break;

                case AxisPlaneConstraint::FORBIDDEN:
                    translation = glm::vec3(0.0, 0.0, 0.0);
                    break;
            }
        }

        virtual void constrainRotation(Quaternion &rotation, Frame *const frame)
        {
            switch (rotationConstraintType()) 
            {
                case AxisPlaneConstraint::FREE:
                    break;

                case AxisPlaneConstraint::PLANE:
                    break;

                case AxisPlaneConstraint::AXIS: 
                {
                    glm::vec3 axis = frame->transformOf( camera()->frame()->inverseTransformOf(rotationConstraintDirection()));
                    glm::vec3 quat = glm::vec3(rotation[0], rotation[1], rotation[2]);
                    quat.projectOnAxis(axis);
                    rotation = Quaternion(quat, 2.0 * acos(rotation[3]));
                } break;

                case AxisPlaneConstraint::FORBIDDEN:
                    rotation = Quaternion(); // identity
                    break;
            }
        }


        

    
};*/

} // namespace qglviewer

#endif // QGLVIEWER_CONSTRAINT_H
