/*********************************************************************************************************************
 *
 * frame.h
 *
 * A Frame represents a coordinate system, defined by a position and an orientation
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/

#ifndef QGLTOOLKIT_FRAME_H
#define QGLTOOLKIT_FRAME_H


#include <QObject>
#include <QString>

//#include "constraint.h"
#include "quaternion.h"


namespace qgltoolkit {


class Frame : public QObject 
{

    Q_OBJECT


    
    private:

        // P o s i t i o n   a n d   o r i e n t a t i o n
        glm::vec3 t_;
        Quaternion q_;

        // C o n s t r a i n t s
//        Constraint *constraint_;

        // F r a m e   c o m p o s i t i o n
        const Frame *referenceFrame_;


    Q_SIGNALS:
  
        void modified();
  
        void interpolated();



    public:


        Frame() : /*constraint_(NULL), */referenceFrame_(NULL) {}

        virtual ~Frame() {}

        Frame(const Frame &frame) : QObject() { (*this) = frame; }

        Frame(const glm::vec3 &position, const Quaternion &orientation)
        : t_(position), q_(orientation), /*constraint_(nullptr), */referenceFrame_(nullptr) 
        {}



        // GETTERS

//        Constraint *constraint() const { return constraint_; }

        const Frame *referenceFrame() const { return referenceFrame_; }

        Quaternion rotation() const { return q_; }

        glm::vec3 translation() const { return t_; }

        glm::vec3 position() const
        {
            if (referenceFrame_)
                return inverseCoordinatesOf( glm::vec3(0.0, 0.0, 0.0) );
            else
                return t_;
        }

        Quaternion orientation() const
        {
            Quaternion res = rotation();
            const Frame *fr = referenceFrame();
            while (fr != NULL) 
            {
                res = fr->rotation() * res;
                fr = fr->referenceFrame();
            }
            return res;
        }

        void getPosition(double &x, double &y, double &z) const
        {
            glm::vec3 p = position();
            x = p.x;
            y = p.y;
            z = p.z;
        }

        void getOrientation(double &q0, double &q1, double &q2, double &q3) const
        {
            Quaternion o = orientation();
            q0 = o[0];
            q1 = o[1];
            q2 = o[2];
            q3 = o[3];
        }


        void getTranslation(double &x, double &y, double &z) const
        {
            const glm::vec3 t = translation();
            x = t[0];
            y = t[1];
            z = t[2];
        }

        void getRotation(double &q0, double &q1, double &q2, double &q3) const
        {
            const Quaternion q = rotation();
            q0 = q[0];
            q1 = q[1];
            q2 = q[2];
            q3 = q[3];
        }



        // SET TRANSLATION

        void setTranslation(const glm::vec3 &translation) 
        {
            t_ = translation;
            Q_EMIT modified();
        }

        void setTranslation(double x, double y, double z)
        {
            setTranslation( glm::vec3(x, y, z) );
        }

        void setTranslationWithConstraint(glm::vec3 &translation)
        {
            glm::vec3 deltaT = translation - this->translation();
//            if (constraint())
//                constraint()->constrainTranslation(deltaT/*, this*/); 

            setTranslation(this->translation() + deltaT);
            translation = this->translation();
        }


        // SET ROTATION

        void setRotation(const Quaternion &rotation) 
        {
            q_ = rotation;
            Q_EMIT modified();
        }

        void setRotation(double q0, double q1, double q2, double q3)
        {
            setRotation(Quaternion(q0, q1, q2, q3));
        }

        void setRotationWithConstraint(Quaternion &rotation)
        {
            Quaternion deltaQ = this->rotation().inverse() * rotation;
//            if (constraint())
//                constraint()->constrainRotation(deltaQ/*, this*/); 

            // Prevent numerical drift
            deltaQ.normalize();

            setRotation(this->rotation() * deltaQ);
            q_.normalize();
            rotation = this->rotation();
        }


        // SET TRANSLATION AND ROTATION

        void setTranslationAndRotation(const glm::vec3 &translation, const Quaternion &rotation)
        {
            t_ = translation;
            q_ = rotation;
            Q_EMIT modified();
        }

        void setTranslationAndRotationWithConstraint(glm::vec3 &translation, Quaternion &rotation)
        {
            glm::vec3 deltaT = translation - this->translation();
            Quaternion deltaQ = this->rotation().inverse() * rotation;

//            if (constraint()) 
//            {
//                constraint()->constrainTranslation(deltaT/*, this*/);  //@@@@@@@@@@@@@@@@@@@@@@@
//                constraint()->constrainRotation(deltaQ/*, this*/);
//            }

            // Prevent numerical drift
            deltaQ.normalize();

            t_ += deltaT;
            q_ *= deltaQ;
            q_.normalize();

            translation = this->translation();
            rotation = this->rotation();

            Q_EMIT modified();
        }


        // SET POSITION

        void setPosition(const glm::vec3 &position) 
        {
            if (referenceFrame())
                setTranslation(referenceFrame()->coordinatesOf(position));
            else
                setTranslation(position);
        }


        void setPosition(double x, double y, double z)
        {
            setPosition( glm::vec3(x, y, z) );
        }


        void Frame::setPositionWithConstraint(glm::vec3 &position) 
        {
            if (referenceFrame())
                position = referenceFrame()->coordinatesOf(position);

            setTranslationWithConstraint(position);
        }


        // SET ORIENTATION

        void setOrientation(const Quaternion &orientation)
        {
            if (referenceFrame())
                setRotation(referenceFrame()->orientation().inverse() * orientation);
            else
                setRotation(orientation);
        }

        void setOrientation(double q0, double q1, double q2, double q3)
        {
            setOrientation(Quaternion(q0, q1, q2, q3));
        }

        void setOrientationWithConstraint(Quaternion &orientation)
        {
            if (referenceFrame())
                orientation = referenceFrame()->orientation().inverse() * orientation;

            setRotationWithConstraint(orientation);
        }



        // SET POSITION AND ORIENTATION

        void setPositionAndOrientation(const glm::vec3 &position, const Quaternion &orientation)
        {
            if (referenceFrame()) 
            {
                t_ = referenceFrame()->coordinatesOf(position);
                q_ = referenceFrame()->orientation().inverse() * orientation;
            } 
            else 
            {
                t_ = position;
                q_ = orientation;
            }
            Q_EMIT modified();
        }


        void setPositionAndOrientationWithConstraint(glm::vec3 &position, Quaternion &orientation)
        {
            if (referenceFrame()) 
            {
                position = referenceFrame()->coordinatesOf(position);
                orientation = referenceFrame()->orientation().inverse() * orientation;
            }
            setTranslationAndRotationWithConstraint(position, orientation);
        }



        // SET CONSTRAINT AND REF FRAME

//        void setConstraint(Constraint *const constraint) { constraint_ = constraint; }

        bool settingAsReferenceFrameWillCreateALoop(const Frame *const frame) 
        {
            const Frame *f = frame;
            while (f != NULL) 
            {
                if (f == this)
                    return true;
                f = f->referenceFrame();
            }
            return false;
        }

        void setReferenceFrame(const Frame *const refFrame)
        {
            if (settingAsReferenceFrameWillCreateALoop(refFrame))
                std::cerr << "Frame::setReferenceFrame would create a loop in Frame hierarchy" <<std::endl;
            else 
            {
                bool identical = (referenceFrame_ == refFrame);
                referenceFrame_ = refFrame;
                if (!identical)
                Q_EMIT modified();
            }
        }



        // OPERATOR

        Frame &operator=(const Frame &frame)
        {
            // Automatic compiler generated version would not emit the modified() signals
            // as is done in setTranslationAndRotation.
            setTranslationAndRotation(frame.translation(), frame.rotation());
//            setConstraint(frame.constraint());
            setReferenceFrame(frame.referenceFrame());
            return *this;
        }

        // TRANSLATE / ROTATE

        void translate(const glm::vec3 &t) 
        {
            glm::vec3 tbis = t;
            translate(tbis);
        }

        void translate(glm::vec3 &t) 
        {
//            if (constraint())
//                constraint()->constrainTranslation(t/*, this*/);  //@@@@@@@@@@@@@@@@@@@@@@@
            t_ += t;
            Q_EMIT modified();
        }

        void translate(double x, double y, double z) 
        {
            glm::vec3 t(x, y, z);
            translate(t);
        }

        void translate(double &x, double &y, double &z) 
        {
            glm::vec3 t(x, y, z);
            translate(t);
            x = t[0];
            y = t[1];
            z = t[2];
        }

        void rotate(const Quaternion &q) 
        {
            Quaternion qbis = q;
            rotate(qbis);
        }

        void rotate(Quaternion &q) 
        {
//            if (constraint())
//                constraint()->constrainRotation(q/*, this*/);  //@@@@@@@@@@@@@@@@@@@@@@@
            q_ *= q;
            q_.normalize(); // Prevents numerical drift
            Q_EMIT modified();
        }

        void rotate(double &q0, double &q1, double &q2, double &q3) 
        {
            Quaternion q(q0, q1, q2, q3);
            rotate(q);
            q0 = q[0];
            q1 = q[1];
            q2 = q[2];
            q3 = q[3];
        }

        void rotate(double q0, double q1, double q2, double q3) 
        {
            Quaternion q(q0, q1, q2, q3);
            rotate(q);
        }

        void rotateAroundPoint(Quaternion &rotation, const glm::vec3 &point, glm::vec3 &center) 
        {
//            if (constraint())
//                constraint()->constrainRotation(rotation/*, this*/);  //@@@@@@@@@@@@@@@@@@@@@@@

            q_ *= rotation;
            q_.normalize(); // Prevents numerical drift
            glm::vec3 trans = point + Quaternion(inverseTransformOf(rotation.axis()), rotation.angle()).rotate(position() - point) - t_;

glm::vec3 trans2 = point + Quaternion(inverseTransformOf(rotation.axis()), rotation.angle()).rotate(center - point) - center;
center = center + trans2;


 //           if (constraint())
//                constraint()->constrainTranslation(trans/*, this*/);  //@@@@@@@@@@@@@@@@@@@@@@@

            t_ += trans;

            
            Q_EMIT modified();
        }

        void rotateAroundPoint(const Quaternion &rotation, const glm::vec3 &point) 
        {
            Quaternion rot = rotation;
            rotateAroundPoint(rot, point);
        }



        // COORDS TRANSFORM

        glm::vec3 coordinatesOf(const glm::vec3 &src) const 
        {
            if (referenceFrame())
            {
                return localCoordinatesOf(referenceFrame()->coordinatesOf(src));
            }
            else
            {
                return localCoordinatesOf(src);
            }
        }

        glm::vec3 inverseCoordinatesOf(const glm::vec3 &src) const 
        {
            const Frame *fr = this;
            glm::vec3 res = src;
            while (fr != nullptr) 
            {
                res = fr->localInverseCoordinatesOf(res);
                fr = fr->referenceFrame();
            }
            return res;
        }

        glm::vec3 localCoordinatesOf(const glm::vec3 &src) const
        {
            return rotation().inverseRotate(src - translation());
        }

        glm::vec3 localInverseCoordinatesOf(const glm::vec3 &src) const 
        {
            return rotation().rotate(src) + translation();
        }

        glm::vec3 coordinatesOfIn(const glm::vec3 &src, const Frame *const in) const 
        {
            const Frame *fr = this;
            glm::vec3 res = src;
            while ((fr != nullptr) && (fr != in)) 
            {
                res = fr->localInverseCoordinatesOf(res);
                fr = fr->referenceFrame();
            }

            if (fr != in)
            {
                // in was not found in the branch of this, res is now expressed in the world
                // coordinate system. Simply convert to in coordinate system.
                res = in->coordinatesOf(res);
            }

            return res;
        }

        glm::vec3 coordinatesOfFrom(const glm::vec3 &src, const Frame *const from) const 
        {
            if (this == from)
                return src;
            else if (referenceFrame())
                return localCoordinatesOf(referenceFrame()->coordinatesOfFrom(src, from));
            else
                return localCoordinatesOf(from->inverseCoordinatesOf(src));
        }

        void getCoordinatesOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            const glm::vec3 r = coordinatesOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getInverseCoordinatesOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            const glm::vec3 r = inverseCoordinatesOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getLocalCoordinatesOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            const glm::vec3 r = localCoordinatesOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getLocalInverseCoordinatesOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            const glm::vec3 r = localInverseCoordinatesOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getCoordinatesOfIn(const glm::vec3 src, glm::vec3 &res, const Frame *const in) const 
        {
            const glm::vec3 r = coordinatesOfIn(glm::vec3(src), in);
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getCoordinatesOfFrom(const glm::vec3 src, glm::vec3 &res, const Frame *const from) const 
        {
            const glm::vec3 r = coordinatesOfFrom(glm::vec3(src), from);
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }
  


        glm::vec3 transformOf(const glm::vec3 src) const 
        {
            if (referenceFrame())
                return localTransformOf(referenceFrame()->transformOf(src));
            else
                return localTransformOf(src);
        }

        glm::vec3 inverseTransformOf(const glm::vec3 src) const 
        {
            const Frame *fr = this;
            glm::vec3 res = src;
            while (fr != nullptr) 
            {
                res = fr->localInverseTransformOf(res);
                fr = fr->referenceFrame();
            }
            return res;
        }

        glm::vec3 localTransformOf(const glm::vec3 src) const 
        {
            return rotation().inverseRotate(src);
        }

        glm::vec3 localInverseTransformOf(const glm::vec3 src) const 
        {
            return rotation().rotate(src);
        }

        glm::vec3 transformOfIn(const glm::vec3 src, const Frame *const in) const 
        {
            const Frame *fr = this;
            glm::vec3 res = src;
            while ((fr != nullptr) && (fr != in)) 
            {
                res = fr->localInverseTransformOf(res);
                fr = fr->referenceFrame();
            }

            if (fr != in)
            {
                // in was not found in the branch of this, res is now expressed in the world
                // coordinate system. Simply convert to in coordinate system.
                res = in->transformOf(res);
            }

            return res;
        }

        glm::vec3 transformOfFrom(const glm::vec3 src, const Frame *const from) const 
        {
            if (this == from)
                return src;
            else if (referenceFrame())
                return localTransformOf(referenceFrame()->transformOfFrom(src, from));
            else
                return localTransformOf(from->inverseTransformOf(src));
        }

        void getTransformOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            glm::vec3 r = transformOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getInverseTransformOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            glm::vec3 r = inverseTransformOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getLocalTransformOf(const glm::vec3 src , glm::vec3 &res) const 
        {
            glm::vec3 r = localTransformOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getLocalInverseTransformOf(const glm::vec3 src, glm::vec3 &res) const 
        {
            glm::vec3 r = localInverseTransformOf(glm::vec3(src));
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getTransformOfIn(const glm::vec3 src , glm::vec3 &res, const Frame *const in) const 
        {
            glm::vec3 r = transformOfIn(glm::vec3(src), in);
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }

        void getTransformOfFrom(const glm::vec3 src, glm::vec3 &res, const Frame *const from) const 
        {
            glm::vec3 r = transformOfFrom(glm::vec3(src), from);
            for (int i = 0; i < 3; ++i)
                res[i] = r[i];
        }


        // MISC


        void alignWithFrame(const Frame *const frame, bool move = false, double threshold = 0.0)
        {
            glm::vec3 directions[2][3];
            for (unsigned short d = 0; d < 3; ++d) 
            {
                glm::vec3 dir((d == 0) ? 1.0 : 0.0, (d == 1) ? 1.0 : 0.0, (d == 2) ? 1.0 : 0.0);
                if (frame)
                    directions[0][d] = frame->inverseTransformOf(dir);
                else
                    directions[0][d] = dir;
                directions[1][d] = inverseTransformOf(dir);
            }

            double maxProj = 0.0;
            double proj;
            unsigned short index[2];
            index[0] = index[1] = 0;
            for (unsigned short i = 0; i < 3; ++i)
                for (unsigned short j = 0; j < 3; ++j)
                {
                    proj = std::abs( glm::dot(directions[0][i] , directions[1][j]) );
                    if ( proj  >= maxProj ) 
                    {
                        index[0] = i;
                        index[1] = j;
                        maxProj = proj;
                    }
                }

            Frame old;
            old = *this;

            double coef = glm::dot( directions[0][index[0]] , directions[1][index[1]] );
            if (std::abs(coef) >= threshold) 
            {
                const glm::vec3 axis = glm::cross(directions[0][index[0]], directions[1][index[1]]);
                double angle = asin(glm::length(axis));
                if (coef >= 0.0)
                    angle = -angle;
                rotate(rotation().inverse() * Quaternion(axis, angle) * orientation());

                // Try to align an other axis direction
                unsigned short d = (index[1] + 1) % 3;
                glm::vec3 dir((d == 0) ? 1.0 : 0.0, (d == 1) ? 1.0 : 0.0, (d == 2) ? 1.0 : 0.0);
                dir = inverseTransformOf(dir);

                double max = 0.0;
                for (unsigned short i = 0; i < 3; ++i) 
                {
                    double proj = std::abs( glm::dot(directions[0][i] , dir) );
                    if (proj > max) 
                    {
                        index[0] = i;
                        max = proj;
                    }
                }

                if (max >= threshold) 
                {
                    const glm::vec3 axis = glm::cross(directions[0][index[0]], dir);
                    double angle = asin(glm::length(axis));
                    if ( glm::dot(directions[0][index[0]] , dir) >= 0.0)
                        angle = -angle;
                    rotate(rotation().inverse() * Quaternion(axis, angle) * orientation());
                }
            }

            if (move) 
            {
                glm::vec3 center;
                if (frame)
                    center = frame->position();

                translate(center - orientation().rotate(old.coordinatesOf(center)) - translation());
            }
        }



        void projectOnLine(const glm::vec3 &origin, const glm::vec3 &direction) 
        {
            const glm::vec3 shift = origin - position();
            glm::vec3 proj = shift;
            proj = projectOnAxis(proj, direction);
            translate(shift - proj);
        }



        // MATRICES

        glm::mat4 getMatrix() const 
        {
            glm::mat4 m = q_.getMatrix();

            m[3][0] = t_[0];
            m[3][1] = t_[1];
            m[3][2] = t_[2];
            /*m[12] = t_[0];
            m[13] = t_[1];
            m[14] = t_[2];*/
            return m;
        }


        const glm::mat4 matrix() const 
        {
            static glm::mat4 m = getMatrix();
            return m;
        }

        const glm::mat4 worldMatrix() const
        {
            // This test is done for efficiency reasons (creates lots of temp objects
            // otherwise).
            if (referenceFrame()) 
            {
                static Frame fr;
                fr.setTranslation(position());
                fr.setRotation(orientation());
                return fr.matrix();
            } 
            else
                return matrix();
        }

        glm::mat4 getWorldMatrix() const
        {
            glm::mat4 mat = worldMatrix();
            return mat;
        }


        void setFromMatrix(const glm::mat4 m)
        {
            if (std::abs(m[3][3]) < 1E-8) 
            {
                std::cerr << "Frame::setFromMatrix: Null homogeneous coefficient" <<std::endl;
                return;
            }

            glm::mat3 rot;
            for (int i = 0; i < 3; ++i) 
            {
                t_[i] = m[3][i] / m[3][3];
                for (int j = 0; j < 3; ++j)
                {
                    // Beware of the transposition (OpenGL to European math)
                    rot[i][j] = m[j][i] / m[3][3];
                }
            }
            q_.setFromRotationMatrix(rot);
            Q_EMIT modified();
        }

  
        Frame inverse() const
        {
            Frame fr(-(q_.inverseRotate(t_)), q_.inverse());
            fr.setReferenceFrame(referenceFrame());
            return fr;
        }
 
        Frame worldInverse() const 
        {
            return Frame(-(orientation().inverseRotate(position())), orientation().inverse());
        }
 
  


};

} // namespace qgltoolkit

#endif // QGLTOOLKIT_FRAME_H
