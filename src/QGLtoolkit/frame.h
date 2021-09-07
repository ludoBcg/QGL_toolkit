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


#include "quaternion.h"


namespace qgltoolkit 
{


/*!
* \class Frame
* \brief A coordinate system, defined by a position and an orientation.
*
* A Frame is a 3D coordinate system, represented by a position() and an
* orientation(). The order of these transformations is important: 
* the Frame is first translated and  then rotated around the new translated origin.
*/
class Frame : public QObject 
{

    Q_OBJECT


    private:

        glm::vec3 m_t;   /*!< position (i.e., translation vector) */
        Quaternion m_q;  /*!< orientation (i.e., quaternion rotation) */


    Q_SIGNALS:
  
        /*!
        * \fn modified
        * \brief Signal to send when Frame is modified.
        */
        void modified();



    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                                CONSTRUCTORS                                                 |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn Frame
        * \brief Destructor of Frame.
        */
        Frame() {}

        /*!
        * \fn ~Frame
        * \brief Destructor of Frame.
        */
        ~Frame() {}

        /*!
        * \fn Frame
        * \brief Constructor of Frame from position and orientation.
        * \param _position : position as 3D vector
        * \param _orientation : orientation as quaternion
        */
        Frame(const glm::vec3 &_position, const Quaternion &_orientation)
        : m_t(_position), m_q(_orientation)
        {}
                
        /*!
        * \fn operator=
        * \brief Equal operator.
        */
        Frame &operator=(const Frame &frame)
        {
            // Automatic compiler generated version would not emit the modified() signals
            // as is done in setTranslationAndRotation.
            setTranslationAndRotation(frame.translation(), frame.rotation());

            return *this;
        }

        /*!
        * \fn Frame
        * \brief Copy constructor of Frame.
        */
        Frame(const Frame &_frame) : QObject() { (*this) = _frame; }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                  GETTERS                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn translation
        * \brief Returns the Frame translation.
        * Similar to position().
        */
        glm::vec3 translation() const { return m_t; }

        /*!
        * \fn position
        * \brief Returns the Frame translation.
        * Similar to translation().
        */
        glm::vec3 position() const { return translation(); }

        /*!
        * \fn rotation
        * \brief Returns the Frame rotation.
        * Similar to orientation().
        */
        Quaternion rotation() const { return m_q; }

        /*!
        * \fn orientation
        * \brief Returns the Frame orientation.
        * Similar to rotation().
        */
        Quaternion orientation() const { return rotation(); }



        /*------------------------------------------------------------------------------------------------------------+
        |                                                  SETTERS                                                    |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn setTranslation
        * \brief Sets the translation of the frame.
        * Similar to setPosition()
        * \param _translation: translation 3D vector.
        */
        void setTranslation(const glm::vec3 _translation) 
        {
            m_t = _translation;
            Q_EMIT modified();
        }

        /*!
        * \fn setTranslation
        * \brief Sets the translation of the frame.
        * Similar to setPosition()
        * \param _x, _y, _z: coordinates of 3D translation vector.
        */
        void setTranslation(double _x, double _y, double _z)
        {
            setTranslation( glm::vec3(_x, _y, _z) );
        }

//        void setTranslationWithConstraint(glm::vec3 &translation) //@@@
//        {
//            glm::vec3 deltaT = translation - this->translation();
////            if (constraint())
////                constraint()->constrainTranslation(deltaT/*, this*/); 
//
//            setTranslation(this->translation() + deltaT);
//            translation = this->translation();
//        }

        /*!
        * \fn setRotation
        * \brief Sets the rotation of the frame.
        * Similar to setOrientation().
        * \param _rotation: rotation quaternion.
        */
        void setRotation(const Quaternion &_rotation) 
        {
            m_q = _rotation;
            Q_EMIT modified();
        }

        /*!
        * \fn setRotation
        * \brief Sets the rotation of the frame.
        * Similar to setOrientation().
        * \param _q0, _q1, _q2, _q3: coordinates of rotation quaternion.
        */
        void setRotation(double _q0, double _q1, double _q2, double _q3)
        {
            setRotation(Quaternion(_q0, _q1, _q2, _q3));
        }

//        void setRotationWithConstraint(Quaternion &rotation) //@@@
//        {
//            Quaternion deltaQ = this->rotation().inverse() * rotation;
////            if (constraint())
////                constraint()->constrainRotation(deltaQ/*, this*/); 
//
//            // Prevent numerical drift
//            deltaQ.normalize();
//
//            setRotation(this->rotation() * deltaQ);
//            m_q.normalize();
//            rotation = this->rotation();
//        }


        /*!
        * \fn setTranslationAndRotation
        * \brief Sets translation and rotation of the frame.
        * Similar to setPositionAndOrientation().
        * \param _translation: translation 3D vector.
        * \param _rotation: rotation quaternion.
        */
        void setTranslationAndRotation(const glm::vec3 &_translation, const Quaternion &_rotation)
        {
            m_t = _translation;
            m_q = _rotation;
            Q_EMIT modified();
        }

//        void setTranslationAndRotationWithConstraint(glm::vec3 &translation, Quaternion &rotation) //@@@
//        {
//            glm::vec3 deltaT = translation - this->translation();
//            Quaternion deltaQ = this->rotation().inverse() * rotation;
//
//            // Prevent numerical drift
//            deltaQ.normalize();
//
//            m_t += deltaT;
//            m_q *= deltaQ;
//            m_q.normalize();
//
//            translation = this->translation();
//            rotation = this->rotation();
//
//            Q_EMIT modified();
//        }


        /*!
        * \fn setPosition
        * \brief Sets the position of the frame.
        * Similar to setTranslation().
        * \param _position: position 3D vector.
        */
        void setPosition(const glm::vec3 &_position) 
        {
            setTranslation(_position);
        }

        /*!
        * \fn setPosition
        * \brief Sets the position of the frame.
        * Similar to setTranslation().
        * \param _x, _y, _z: coordinates of 3position 3D vector.
        */
        void setPosition(double _x, double _y, double _z)
        {
            setPosition( glm::vec3(_x, _y, _z) );
        }

        //void Frame::setPositionWithConstraint(glm::vec3 &position)  //@@@
        //{
        //    if (referenceFrame())
        //        position = referenceFrame()->coordinatesOf(position);

        //    setTranslationWithConstraint(position);
        //}

        /*!
        * \fn setOrientation
        * \brief Sets the orientation of the frame.
        * Similar to setRotation().
        * \param _rotation: orientation quaternion.
        */
        void setOrientation(const Quaternion &_orientation)
        {
            setRotation(_orientation);
        }

        /*!
        * \fn setOrientation
        * \brief Sets the orientation of the frame.
        * Similar to setRotation().
        * \param _q0, _q1, _q2, _q3: coordinates of orientation quaternion.
        */
        void setOrientation(double _q0, double _q1, double _q2, double _q3)
        {
            setOrientation(Quaternion(_q0, _q1, _q2, _q3));
        }

        //void setOrientationWithConstraint(Quaternion &orientation) //@@@
        //{
        //    if (referenceFrame())
        //        orientation = referenceFrame()->orientation().inverse() * orientation;

        //    setRotationWithConstraint(orientation);
        //}

        /*!
        * \fn setPositionAndOrientation
        * \brief Sets position and orientation of the frame.
        * Similar to setTranslationAndRotation().
        * \param _position: position 3D vector.
        * \param _orientation: orientation quaternion.
        */
        void setPositionAndOrientation(const glm::vec3 &_position, const Quaternion &_orientation)
        {

            m_t = _position;
            m_q = _orientation;

            Q_EMIT modified();
        }

        //void setPositionAndOrientationWithConstraint(glm::vec3 &position, Quaternion &orientation) //@@@
        //{
        //    if (referenceFrame()) 
        //    {
        //        position = referenceFrame()->coordinatesOf(position);
        //        orientation = referenceFrame()->orientation().inverse() * orientation;
        //    }
        //    setTranslationAndRotationWithConstraint(position, orientation);
        //}


        /*------------------------------------------------------------------------------------------------------------+
        |                                              TRANSFORMATIONS                                                |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn translate
        * \brief Translates frame by a given vector.
        * \param _t: translation vector
        */
        void translate(glm::vec3 &_t) 
        {
            m_t += _t;
            Q_EMIT modified();
        }

        /*!
        * \fn translate
        * \brief Translates frame by a given vector.
        * \param _x, _y, _z: coordinates of translation vector
        */
        void translate(double _x, double _y, double _z) 
        {
            glm::vec3 t(_x, _y, _z);
            translate(t);
        }

        /*!
        * \fn rotate
        * \brief Rotates frame by a given quaternion.
        * \param _q: rotation quaternion
        */
        void rotate(Quaternion &_q) 
        {
            m_q *= _q;
            m_q.normalize(); // Prevents numerical drift
            Q_EMIT modified();
        }

        /*!
        * \fn rotate
        * \brief Rotates frame by a given quaternion.
        * \param _q0, _q1, _q2, _q3: coordinates of rotation quaternion.
        */
        void rotate(double _q0, double _q1, double _q2, double _q3) 
        {
            Quaternion q(_q0, _q1, _q2, _q3);
            rotate(q);
        }

        /*!
        * \fn transformOf
        * \brief Returns the Frame transform of a vector _src defined in the world
        * coordinate system (converts vectors from world to Frame).
        * inverseTransformOf() performs the inverse transformation. 
        * coordinatesOf() converts 3D coordinates instead of 3D vectors 
        * (here only the rotational part of the transformation is taken into account).
        */
        glm::vec3 transformOf(const glm::vec3 _src) const
        {
            return rotation().inverseRotate(_src);
        }

        /*!
        * \fn inverseTransformOf
        * \brief Returns the world transform of the vector whose coordinates in the Frame
        * coordinate system is _src (converts vectors from Frame to world).
        * transformOf() performs the inverse transformation. 
        * Use inverseCoordinatesOf() to transform 3D coordinates instead of 3D vectors.
        */
        glm::vec3 inverseTransformOf(const glm::vec3 _src) const
        {
            return rotation().rotate(_src);
        }

        /*!
        * \fn coordinatesOf
        * \brief Returns the Frame coordinates of a point _src defined in the world
        * coordinate system (converts from world to Frame).
        * inverseCoordinatesOf() performs the inverse convertion. 
        * transformOf() converts 3D vectors instead of 3D coordinates.
        */
        glm::vec3 coordinatesOf(const glm::vec3 &_src) const 
        {
            return rotation().inverseRotate(_src - translation());
        }

        /*!
        * \fn inverseCoordinatesOf
        * \brief Returns the world coordinates of the point whose position in the Frame
        * coordinate system is _src (converts from Frame to world).
        * coordinatesOf() performs the inverse convertion. 
        * Use inverseTransformOf() to transform 3D vectors instead of 3D coordinates.
        */
        glm::vec3 inverseCoordinatesOf(const glm::vec3 &_src) const 
        {
            return rotation().rotate(_src) + translation();
        }

        /*!
        * \fn rotateAroundPoint
        * \brief Rotates frame around a given point.
        * _point is defined in the world coordinate system, while the _rotation axis
        * is defined in the Frame coordinate system.
        * \param _rotation: rotation quaternion
        * \param _point: rotation quaternion
        * \param _center: rotation quaternion
        */
        void rotateAroundPoint(Quaternion &_rotation, const glm::vec3 &_point, glm::vec3 &_center) // @@@ center ?
        {
            m_q *= _rotation;
            m_q.normalize(); // Prevents numerical drift
            glm::vec3 trans = _point + Quaternion(inverseTransformOf(_rotation.axis()), _rotation.angle()).rotate(position() - _point) - m_t;

glm::vec3 trans2 = _point + Quaternion(inverseTransformOf(_rotation.axis()), _rotation.angle()).rotate(_center - _point) - _center;
_center = _center + trans2;


            m_t += trans;

            
            Q_EMIT modified();
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                   MISC.                                                     |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn alignWithFrame
        * \brief Aligns the Frame with _frame, so that two of their axis are parallel.
        *
        * If one of the X, Y and Z axis of the Frame is almost parallel to any of the X,
        * Y, or Z axis of _frame, the Frame is rotated so that these two axis actually
        * become parallel.
        * If, after this first rotation, two other axis are also almost parallel, 
        * a second alignment is performed. The two frames then have identical orientations, 
        * up to 90 degrees rotations.
        *
        * _threshold measures how close two axis must be to be considered parallel. 
        * It is compared with the absolute values of the dot product of the normalized axis.
        * As a result, useful range is sqrt(2)/2 (systematic alignment) to 1 (no alignment).
        *
        * When _move is set to true, the Frame position() is also affected by the alignment. 
        * The new Frame's position() is such that the _frame position (computed with coordinatesOf(), 
        * in the Frame coordinates system) does not change.
        *
        * _frame may be NULL and then represents the world coordinate system 
        *
        * \param _frame: frame to align with
        * \param _move: defines if frame position is affected or not
        * \param _threshold: compared with dot product to define when vectors are considered close
        */
        void alignWithFrame(const Frame *const _frame, bool _move = false, double _threshold = 0.0)
        {
            glm::vec3 directions[2][3];
            for (unsigned short d = 0; d < 3; ++d) 
            {
                glm::vec3 dir((d == 0) ? 1.0 : 0.0, (d == 1) ? 1.0 : 0.0, (d == 2) ? 1.0 : 0.0);
                if (_frame)
                    directions[0][d] = _frame->inverseTransformOf(dir);
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
            if (std::abs(coef) >= _threshold) 
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

                if (max >= _threshold) 
                {
                    const glm::vec3 axis = glm::cross(directions[0][index[0]], dir);
                    double angle = asin(glm::length(axis));
                    if ( glm::dot(directions[0][index[0]] , dir) >= 0.0)
                        angle = -angle;
                    rotate(rotation().inverse() * Quaternion(axis, angle) * orientation());
                }
            }

            if (_move) 
            {
                glm::vec3 center;
                if (_frame)
                    center = _frame->position();

                translate(center - orientation().rotate(old.coordinatesOf(center)) - translation());
            }
        }

        /*!
        * \fn projectOnLine
        * \brief Translates the Frame so that its position() lies on the line defined 
        * by _origin and _direction (defined in the world coordinate system).
        * Simply uses an orthogonal projection. 
        * _direction does not need to be normalized.
        * \param _origin: origin point of line
        * \param _direction: direction vector of line
        */
        void projectOnLine(const glm::vec3 &_origin, const glm::vec3 &_direction) 
        {
            const glm::vec3 shift = _origin - position();
            glm::vec3 proj = shift;
            proj = projectOnAxis(proj, _direction);
            translate(shift - proj);
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                  MATRICES                                                   |
        +------------------------------------------------------------------------------------------------------------*/

        //glm::mat4 getMatrix() const 
        //{
        //    glm::mat4 m = m_q.getMatrix();
        //    m[3][0] = m_t[0];
        //    m[3][1] = m_t[1];
        //    m[3][2] = m_t[2];

        //    /*m[12] = m_t[0];
        //    m[13] = m_t[1];
        //    m[14] = m_t[2];*/
        //    return m;
        //}


        //const glm::mat4 matrix() const 
        //{
        //    static glm::mat4 m = getMatrix();
        //    return m;
        //}

};


} // namespace qgltoolkit

#endif // QGLTOOLKIT_FRAME_H
