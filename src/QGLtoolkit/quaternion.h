/*********************************************************************************************************************
 *
 * quaternion.h
 *
 * A Quaternion class for 3D rotations and orientations
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/

#ifndef QGLTOOLKIT_QUATERNION_H
#define QGLTOOLKIT_QUATERNION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>



namespace qgltoolkit 
{



/*!
* \fn squaredNorm
* \brief Squared norm of a glm::vec3
*/
static float squaredNorm(glm::vec3 _vec) { return _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z; }

/*!
* \fn projectOnAxis
* \brief Project a glm::vec3 on a given axis
*/
static glm::vec3 projectOnAxis(glm::vec3 _vec, const glm::vec3 &direction) 
{

    if ( squaredNorm(direction) < 1.0E-10)
        std::cout << "Vec::projectOnAxis: axis direction is not normalized " <<std::endl;

    return  (  glm::dot(_vec , direction)  / squaredNorm(direction) ) * direction;
}

/*!
* \fn orthogonalVec
* \brief Builds and returns a new 3D vector orthogonal to _vec.
*/
static glm::vec3 orthogonalVec(glm::vec3 _vec) 
{
    // Find smallest component. Keep equal case for null values.
    if ((fabs(_vec.y) >= 0.9 * fabs(_vec.x)) && (fabs(_vec.z) >= 0.9 * fabs(_vec.x)))
        return glm::vec3(0.0, -_vec.z, _vec.y);
    else if ((fabs(_vec.x) >= 0.9 * fabs(_vec.y)) && (fabs(_vec.z) >= 0.9 * fabs(_vec.y)))
        return glm::vec3(-_vec.z, 0.0, _vec.x);
    else
        return glm::vec3(-_vec.y, _vec.x, 0.0);
}


/*!
* \class Quaternion
* \brief Represents 3D rotations and orientations.
*
* You can apply the rotation represented by the Quaternion to 3D points 
* using rotate() and inverseRotate().
*
* The internal representation of a Quaternion corresponding to a rotation 
* around an axis, with an angle alpha is made of four reals:  
* {m_q[0],m_q[1],m_q[2]} = sin(alpha/2) * {axis[0],axis[1],axis[2]} 
* m_q[3] = cos(alpha/2)
*
* Note that certain implementations place the cosine term in first 
* position (instead of last here).
*
* The Quaternion is always normalized, so that its inverse() is actually its conjugate.
*/
class  Quaternion 
{

    private:

        double m_q[4];    /*!< quaternion */


    public:

        /*------------------------------------------------------------------------------------------------------------+
        |                                          CONSTRUCTORS / SETTERS                                             |
        +------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn Quaternion
        * \brief Default constructor of Quaternion.
        * Creates an identity Quaternion(0,0,0,1).
        */
        Quaternion() 
        {
            m_q[0] = m_q[1] = m_q[2] = 0.0;
            m_q[3] = 1.0;
        }

        /*!
        * \fn ~Quaternion
        * \brief Destructor of Quaternion.
        */
        ~Quaternion() {}

        /*!
        * \fn setAxisAngle
        * \brief Build a Quaternion from rotation axis (non null) and angle (in radians).
        * \param _axis : 3D axis vector
        * \param _angle : angle in radians
        */
        void setAxisAngle(const glm::vec3 _axis, double _angle) 
        {
            const double norm = glm::length(_axis);
            if (_axis == glm::vec3(0.0f) ) 
            {
                // Null rotation
                m_q[0] = 0.0;
                m_q[1] = 0.0;
                m_q[2] = 0.0;
                m_q[3] = 1.0;
            } 
            else 
            {
                const double sin_half_angle = sin(_angle / 2.0);
                m_q[0] = sin_half_angle * _axis[0] / norm;
                m_q[1] = sin_half_angle * _axis[1] / norm;
                m_q[2] = sin_half_angle * _axis[2] / norm;
                m_q[3] = cos(_angle / 2.0);
            }
        }

        /*!
        * \fn Quaternion
        * \brief Constructor of Quaternion from rotation axis (non null) and angle (in radians).
        * \param _axis : 3D axis vector
        * \param _angle : angle in radians
        */
        Quaternion(const glm::vec3 &_axis, double _angle) { setAxisAngle(_axis, _angle); }

        /*!
        * \fn Quaternion
        * \brief Constructor of Quaternion that will rotate from the _from direction to the _to direction.
        * Note that this rotation is not uniquely defined. 
        * The selected axis is usually orthogonal to _from and _to, minimizing the rotation angle. 
        * This method is robust and can handle small or almost identical vectors.
        * \param _from : initial position before rotation
        * \param _to : final position after rotation
        */
        Quaternion::Quaternion(const glm::vec3 _from, const glm::vec3 _to) 
        {
            const qreal epsilon = 1E-10;

            const qreal fromSqNorm = squaredNorm(_from);
            const qreal toSqNorm = squaredNorm(_to);
            // Identity Quaternion when one vector is null
            if ((fromSqNorm < epsilon) || (toSqNorm < epsilon)) 
            {
                m_q[0] = m_q[1] = m_q[2] = 0.0;
                m_q[3] = 1.0;
            } 
            else 
            {
                glm::vec3  axis = glm::cross(_from, _to);
                const qreal axisSqNorm = squaredNorm(axis);

                // Aligned vectors, pick any axis, not aligned with from or to
                if (axisSqNorm < epsilon)
                axis = orthogonalVec(_from);

                qreal angle = asin(sqrt(axisSqNorm / (fromSqNorm * toSqNorm)));

                if ( glm::dot(_from , _to ) < 0.0)
                angle = M_PI - angle;

                setAxisAngle(axis, angle);
            }
        }

        /*!
        * \fn setValue
        * \brief Set four values of a Quaternion. 
        * First three values are axis*sin(angle/2) and last one is cos(angle/2).
        * The identity Quaternion is Quaternion(0,0,0,1).
        * \param _q0, _q1, _q2, _q3 : quaternion values
        */
        void setValue(double _q0, double _q1, double _q2, double _q3) 
        {
            m_q[0] = _q0;
            m_q[1] = _q1;
            m_q[2] = _q2;
            m_q[3] = _q3;
        }

        /*!
        * \fn Quaternion
        * \brief Constructor of Quaternion from the four values of a Quaternion. 
        * First three values are axis*sin(angle/2) and last one is cos(angle/2).
        * The identity Quaternion is Quaternion(0,0,0,1).
        * \param _q0, _q1, _q2, _q3 : quaternion values
        */
        Quaternion(double _q0, double _q1, double _q2, double _q3) { setValue(_q0, _q1, _q2, _q3); }

        /*!
        * \fn Quaternion
        * \brief Copy constructor of Quaternion.
        */
        Quaternion(const Quaternion &_Q) 
        {
            for (int i = 0; i < 4; ++i)
                m_q[i] = _Q.m_q[i];
        }

        /*!
        * \fn operator=
        * \brief Equal operator.
        */
        Quaternion &operator=(const Quaternion &_Q) 
        {
            for (int i = 0; i < 4; ++i)
                m_q[i] = _Q.m_q[i];

            return (*this);
        }

        /*!
        * \fn setFromRotationMatrix
        * \brief Set the Quaternion from a (supposedly correct) 3x3 rotation matrix. 
        * 
        * The matrix is expressed in European format: its three columns are the
        * images by the rotation of the three vectors of an orthogonal basis. Note that
        * OpenGL uses a symmetric representation for its matrices.
        *
        * setFromRotatedBasis() sets a Quaternion from the three axis of a rotated
        * frame. It actually fills the three columns of a matrix with these rotated
        * basis vectors and calls this method.
        *
        * \param _m : glm 3x3 matrix
        */
        void setFromRotationMatrix(const glm::mat3 _m)
        {
            // Compute one plus the trace of the matrix
            const double onePlusTrace = 1.0 + _m[0][0] + _m[1][1] + _m[2][2];

            if (onePlusTrace > 1E-5) 
            {
                // Direct computation
                const double s = sqrt(onePlusTrace) * 2.0;
                m_q[0] = (_m[2][1] - _m[1][2]) / s;
                m_q[1] = (_m[0][2] - _m[2][0]) / s;
                m_q[2] = (_m[1][0] - _m[0][1]) / s;
                m_q[3] = 0.25 * s;
            } 
            else 
            {
                // Computation depends on major diagonal term
                if ((_m[0][0] > _m[1][1]) & (_m[0][0] > _m[2][2])) 
                {
                    const double s = sqrt(1.0 + _m[0][0] - _m[1][1] - _m[2][2]) * 2.0;
                    m_q[0] = 0.25 * s;
                    m_q[1] = (_m[0][1] + _m[1][0]) / s;
                    m_q[2] = (_m[0][2] + _m[2][0]) / s;
                    m_q[3] = (_m[1][2] - _m[2][1]) / s;
                } 
                else if (_m[1][1] > _m[2][2]) 
                {
                    const double s = sqrt(1.0 + _m[1][1] - _m[0][0] - _m[2][2]) * 2.0;
                    m_q[0] = (_m[0][1] + _m[1][0]) / s;
                    m_q[1] = 0.25 * s;
                    m_q[2] = (_m[1][2] + _m[2][1]) / s;
                    m_q[3] = (_m[0][2] - _m[2][0]) / s;
                } 
                else 
                {
                    const double s = sqrt(1.0 + _m[2][2] - _m[0][0] - _m[1][1]) * 2.0;
                    m_q[0] = (_m[0][2] + _m[2][0]) / s;
                    m_q[1] = (_m[1][2] + _m[2][1]) / s;
                    m_q[2] = 0.25 * s;
                    m_q[3] = (_m[0][1] - _m[1][0]) / s;
                }
            }
            normalize();
        }

        /*!
        * \fn setFromRotatedBasis
        * \brief Sets the Quaternion from the three rotated vectors of an orthogonal basis.
        *
        * The three vectors do not have to be normalized but must be orthogonal and
        * direct (X^Y=k*Z, with k>0).
        *
        * As a result, q.rotate(Vec(1,0,0)) == X and q.inverseRotate(X) == Vec(1,0,0).
        * Same goes for Y and Z with Vec(0,1,0) and Vec(0,0,1).
        *
        * \param _X, _Y, _Z : three vectors of orthognonal basis
        */
        void setFromRotatedBasis(const glm::vec3 &_X, const glm::vec3 &_Y, const glm::vec3 &_Z)
        {
            glm::mat3 m(0.0f);
            double normX = glm::length(_X);
            double normY = glm::length(_Y);
            double normZ = glm::length(_Z);

            for (int i = 0; i < 3; ++i) 
            {
                m[i][0] = _X[i] / normX;
                m[i][1] = _Y[i] / normY;
                m[i][2] = _Z[i] / normZ;
            }

            setFromRotationMatrix(m);
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                  GETTERS                                                    |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn axis
        * \brief Returns the normalized axis direction of the rotation represented 
        * by the Quaternion.
        * It is null for an identity Quaternion. 
        */
        glm::vec3 axis() const
        {
            glm::vec3 res = glm::vec3(m_q[0], m_q[1], m_q[2]);
            const double sinus = glm::length(res);
            if (sinus > 1E-8)
                res /= sinus;
            return (acos(m_q[3]) <= M_PI / 2.0) ? res : -res;
        }

        /*!
        * \fn angle
        * \brief Returns the angle (in radians) of the rotation represented 
        * by the Quaternion.
        * This value is always in the range [0-pi]. Larger rotational angles are obtained
        * by inverting the axis() direction.
        */
        double angle() const
        {
            const double angle = 2.0 * acos(m_q[3]);
            return (angle <= M_PI) ? angle : 2.0 * M_PI - angle;
        }

        /*!
        * \fn getAxisAngle
        * \brief Returns the axis vector and the angle (in radians) of the rotation
        * represented by the Quaternion.
        * \param _axis : reference to axis vector to return
        * \param _angle : reference to angle to return
        */
        void getAxisAngle(glm::vec3 &_axis, double &_angle) const
        {
            _angle = 2.0 * acos(m_q[3]);
            _axis = glm::vec3 (m_q[0], m_q[1], m_q[2]);
            const double sinus = glm::length(_axis);
            if (sinus > 1E-8)
            _axis /= sinus;

            if (_angle > M_PI)
            {
                _angle = 2.0 * double(M_PI) - _angle;
                _axis = -_axis;
            }
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                 OPERATORS                                                   |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn operator[]
        * \brief Bracket operator, with a constant return value. 
        * _i must range in [0..3].
        */
        double operator[](int _i) const { return m_q[_i]; }

        /*!
        * \fn &operator[]
        * \brief Bracket operator returning an l-value.
        * _i must range in [0..3].
        */
        double &operator[](int _i) { return m_q[_i]; }
  
        /*!
        * \fn operator*
        * \brief Rotation computations.
        * Returns the composition of the _a and _b rotations.
        * The order is important. Tthe resulting Quaternion acts as if _b was applied 
        * first and then _a.
        * Note that _a * _b usually differs from _b * _a.
        * For efficiency reasons, the resulting Quaternion is not normalized. 
        * Use normalize() in case of numerical drift with small rotation composition.
        * \param _a, _b : quaternions to multiply
        * \return result as a new quaternion
        */
        friend Quaternion operator*(const Quaternion &_a, const Quaternion &_b) 
        {
            return Quaternion(  _a.m_q[3] * _b.m_q[0] + _b.m_q[3] * _a.m_q[0] + _a.m_q[1] * _b.m_q[2] - _a.m_q[2] * _b.m_q[1],
                                _a.m_q[3] * _b.m_q[1] + _b.m_q[3] * _a.m_q[1] + _a.m_q[2] * _b.m_q[0] - _a.m_q[0] * _b.m_q[2],
                                _a.m_q[3] * _b.m_q[2] + _b.m_q[3] * _a.m_q[2] + _a.m_q[0] * _b.m_q[1] - _a.m_q[1] * _b.m_q[0],
                                _a.m_q[3] * _b.m_q[3] - _b.m_q[0] * _a.m_q[0] - _a.m_q[1] * _b.m_q[1] - _a.m_q[2] * _b.m_q[2]);
        }

        /*!
        * \fn &operator*=
        * \brief Quaternion rotation is composed with _q
        */
        Quaternion &operator*=(const Quaternion &_q) 
        {
            *this = (*this) * _q;
            return *this;
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                              MATH OPERATIONS                                                |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn operator*
        * \brief Returns the image of _v by the current Quaternion rotation.
        * \param _v: 3D vector to rotate
        * \return new rotated vector
        */
        glm::vec3 rotate(const glm::vec3 &_v) const
        {
            const double q00 = 2.0 * m_q[0] * m_q[0];
            const double q11 = 2.0 * m_q[1] * m_q[1];
            const double q22 = 2.0 * m_q[2] * m_q[2];

            const double q01 = 2.0 * m_q[0] * m_q[1];
            const double q02 = 2.0 * m_q[0] * m_q[2];
            const double q03 = 2.0 * m_q[0] * m_q[3];

            const double q12 = 2.0 * m_q[1] * m_q[2];
            const double q13 = 2.0 * m_q[1] * m_q[3];

            const double q23 = 2.0 * m_q[2] * m_q[3];

            return glm::vec3( (1.0 - q11 - q22) * _v[0] + (q01 - q23) * _v[1] + (q02 + q13) * _v[2],
                              (q01 + q23) * _v[0] + (1.0 - q22 - q00) * _v[1] + (q12 - q03) * _v[2],
                              (q02 - q13) * _v[0] + (q12 + q03) * _v[1] + (1.0 - q11 - q00) * _v[2] );
        }

        /*!
        * \fn operator*
        * \brief Returns the image of _v by the rotation _q.
        * Same as q.rotate(v).
        * \param _q: rotation quaternion
        * \param _v: 3D vector to rotate
        * \return new rotated vector
        */
        friend glm::vec3 operator*(const Quaternion &_q, const glm::vec3 &_v) 
        {
            return _q.rotate(_v);
        }

        /*!
        * \fn inverse
        * \brief Returns the inverse Quaternion (inverse rotation).
        * Result has a negated axis() direction and the same angle(). 
        * A composition (see operator*()) of a Quaternion and its inverse() results in
        * an identity function.
        * Use invert() to actually modify the Quaternion.
        * \return copy of inverse Quaternion 
        */
        Quaternion inverse() const { return Quaternion(-m_q[0], -m_q[1], -m_q[2], m_q[3]); }

        /*!
        * \fn invert
        * \brief Inverses the Quaternion (same rotation angle(), but negated axis()).
        */
        void invert() 
        {
            m_q[0] = -m_q[0];
            m_q[1] = -m_q[1];
            m_q[2] = -m_q[2];
        }

        /*!
        * \fn inverseRotate
        * \brief Returns the image of _v by the Quaternion inverse() rotation.
        * rotate() performs an inverse transformation. Same as inverse().rotate(v).
        * \param _v: 3D vector to transform 
        * \return result 3D vector 
        */
        glm::vec3 inverseRotate(const glm::vec3 &_v) const
        {
            return inverse().rotate(_v);
        }
  
        /*!
        * \fn negate
        * \brief Negates all the coefficients of the Quaternion.
        *
        * This results in an other representation of the same rotation
        * (opposite rotation angle, but with a negated axis direction: the two cancel out). 
        * However, note that the results of axis() and angle() are unchanged
        * after a call to this method since angle() always returns a value in [0,pi].
        *
        * This method is mainly useful for Quaternion interpolation, so that the
        * spherical interpolation takes the shortest path on the unit sphere. See
        * slerp() for details.
        */
        void negate() 
        {
            invert();
            m_q[3] = -m_q[3];
        }

        /*!
        * \fn normalize
        * \brief Normalizes the Quaternion coefficients.
        * This method should not need to be called since we only deal with unit
        * Quaternions. This is however useful to prevent numerical drifts, especially
        * with small rotational increments.
        */
        double normalize() 
        {
            const double norm =  sqrt(m_q[0] * m_q[0] + m_q[1] * m_q[1] + m_q[2] * m_q[2] + m_q[3] * m_q[3]);
            for (int i = 0; i < 4; ++i)
                m_q[i] /= norm;
            return norm;
        }

        /*!
        * \fn normalized
        * \brief Returns a normalized version of the Quaternion.
        * See also normalize().
        */
        Quaternion normalized() const 
        {
            double Q[4];
            const double norm = sqrt(m_q[0] * m_q[0] + m_q[1] * m_q[1] + m_q[2] * m_q[2] + m_q[3] * m_q[3]);
            for (int i = 0; i < 4; ++i)
                Q[i] = m_q[i] / norm;
            return Quaternion(Q[0], Q[1], Q[2], Q[3]);
        }

        /*!
        * \fn dot
        * \brief Returns the "dot" product of _a and _b: 
        * _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2] + _a[3] * _b[3].
        * \param _a, _b: Quaternions to multiply using dot product 
        * \return result of dot product as a real number 
        */
        static double dot(const Quaternion &_a, const Quaternion &_b) 
        {
            return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2] + _a[3] * _b[3];
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                               BUILD MATRIX                                                  |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn getMatrix
        * \brief Build a 4x4 matrix representation of the Quaternion rotation.
        * \return rotation as a glm 4x4 matrix
        */
        glm::mat4 getMatrix() const
        {
            glm::mat4 m(0.0f);

            const double q00 = 2.0 * m_q[0] * m_q[0];
            const double q11 = 2.0 * m_q[1] * m_q[1];
            const double q22 = 2.0 * m_q[2] * m_q[2];

            const double q01 = 2.0 * m_q[0] * m_q[1];
            const double q02 = 2.0 * m_q[0] * m_q[2];
            const double q03 = 2.0 * m_q[0] * m_q[3];

            const double q12 = 2.0 * m_q[1] * m_q[2];
            const double q13 = 2.0 * m_q[1] * m_q[3];

            const double q23 = 2.0 * m_q[2] * m_q[3];

            m[0][0] = 1.0 - q11 - q22;
            m[1][0] = q01 - q23;
            m[2][0] = q02 + q13;

            m[0][1] = q01 + q23;
            m[1][1] = 1.0 - q22 - q00;
            m[2][1] = q12 - q03;

            m[0][2] = q02 - q13;
            m[1][2] = q12 + q03;
            m[2][2] = 1.0 - q11 - q00;

            m[0][3] = 0.0;
            m[1][3] = 0.0;
            m[2][3] = 0.0;

            m[3][0] = 0.0;
            m[3][1] = 0.0;
            m[3][2] = 0.0;
            m[3][3] = 1.0;

            return m;
        }

        /*!
        * \fn getInverseMatrix
        * \brief Returns the inverse() rotation matrix.
        * \return inverse matrix as a glm 4x4 matrix
        */
        glm::mat4 getInverseMatrix() const
        {
            return inverse().getMatrix();
        }


        /*------------------------------------------------------------------------------------------------------------+
        |                                                   MISC.                                                     |
        +-------------------------------------------------------------------------------------------------------------*/

        /*!
        * \fn slerp
        * \brief Returns the slerp interpolation of Quaternions _a and _b, at time _t.
        * _t should range in [0,1]. Result is _a when _t=0 and _b when _t=1.
        * When _allowFlip is true (default) the slerp interpolation will always use
        * the "shortest path" between the Quaternions' orientations, by "flipping" the
        * source Quaternion if needed (see negate()).
        * \param _a, _b: Quaternions to interpolate between
        * \param _t: interpolation factor in [0,1]
        * \param _allowFlip: return shortest path if true
        * \return result of interpolation as a new Quaternion
        */
        static Quaternion slerp(const Quaternion &_a, const Quaternion &_b, double _t, bool _allowFlip = true)
        {
            double cosAngle = Quaternion::dot(_a, _b);

            double c1, c2;
            // Linear interpolation for close orientations
            if ((1.0 - fabs(cosAngle)) < 0.01)
            {
                c1 = 1.0 - _t;
                c2 = _t;
            } 
            else 
            {
                // Spherical interpolation
                double angle = acos(fabs(cosAngle));
                double sinAngle = sin(angle);
                c1 = sin(angle * (1.0 - _t)) / sinAngle;
                c2 = sin(angle * _t) / sinAngle;
            }

            // Use the shortest path
            if (_allowFlip && (cosAngle < 0.0))
                c1 = -c1;

            return Quaternion(c1 * _a[0] + c2 * _b[0], c1 * _a[1] + c2 * _b[1], c1 * _a[2] + c2 * _b[2], c1 * _a[3] + c2 * _b[3]);
        }

        /*!
        * \fn squad
        * \brief  Returns the slerp interpolation of the two Quaternions _a and _b, 
        * at time _t, using tangents _tgA and _tgB.
        * The resulting Quaternion is "between" _a and _b (result is _a when _t=0 and _b for _t=1).
        * Use squadTangent() to define the Quaternion tangents _tgA and _tgB.
        * \param _a, _b: Quaternions to interpolate between
        * \param _tgA, _tgB: tangents
        * \param _t: interpolation factor in [0,1]
        * \return result of interpolation as a new Quaternion
        */
        static Quaternion squad(const Quaternion &_a, const Quaternion &_tgA, const Quaternion &_tgB, const Quaternion &_b, double _t)
        {
            Quaternion ab = Quaternion::slerp(_a, _b, _t);
            Quaternion tg = Quaternion::slerp(_tgA, _tgB, _t, false);
            return Quaternion::slerp(ab, tg, 2.0 * _t * (1.0 - _t), false);
        }

        /*!
        * \fn log
        * \brief Returns the logarithm of the Quaternion. See also exp().
        */
        Quaternion log()
        {
            double len = sqrt(m_q[0] * m_q[0] + m_q[1] * m_q[1] + m_q[2] * m_q[2]);

            if (len < 1E-6)
                return Quaternion(m_q[0], m_q[1], m_q[2], 0.0);
            else 
            {
                double coef = acos(m_q[3]) / len;
                return Quaternion(m_q[0] * coef, m_q[1] * coef, m_q[2] * coef, 0.0);
            }
        }

        /*!
        * \fn exp
        * \brief Returns the exponential of the Quaternion. See also log().
        */
        Quaternion exp()
        {
            double theta = sqrt(m_q[0] * m_q[0] + m_q[1] * m_q[1] + m_q[2] * m_q[2]);

            if (theta < 1E-6)
                return Quaternion(m_q[0], m_q[1], m_q[2], cos(theta));
            else 
            {
                double coef = sin(theta) / theta;
                return Quaternion(m_q[0] * coef, m_q[1] * coef, m_q[2] * coef, cos(theta));
            }
        }

        /*!
        * \fn lnDif
        * \brief Returns log(_a. inverse() * _b). Useful for squadTangent().
        */
        static Quaternion lnDif(const Quaternion &_a, const Quaternion &_b)
        {
            Quaternion dif = _a.inverse() * _b;
            dif.normalize();
            return dif.log();
        }

        /*!
        * \fn squadTangent
        * \brief Returns a tangent Quaternion for _center, defined by _before and _after Quaternions.
        * Useful for smooth spline interpolation of Quaternion with squad() and slerp().
        */
        static Quaternion squadTangent(const Quaternion &_before, const Quaternion &_center, const Quaternion &_after)
        {
            Quaternion l1 = Quaternion::lnDif(_center, _before);
            Quaternion l2 = Quaternion::lnDif(_center, _after);
            Quaternion e;

            for (unsigned int i = 0; i < 4; ++i)
                e.m_q[i] = -0.25 * (l1.m_q[i] + l2.m_q[i]);

            e = _center * (e.exp());

            // if (Quaternion::dot(e,b) < 0.0)
            // e.negate();

            return e;
        }


};

} // namespace qgltoolkit



#endif // QGLTOOLKIT_QUATERNION_H
