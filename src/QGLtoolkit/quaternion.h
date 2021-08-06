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

#ifndef QGLVIEWER_QUATERNION_H
#define QGLVIEWER_QUATERNION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

namespace qglviewer {


static float squaredNorm(glm::vec3 _vec) { return _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z; }

static glm::vec3 projectOnAxis(glm::vec3 _vec, const glm::vec3 &direction) 
{

    if ( squaredNorm(direction) < 1.0E-10)
        std::cout << "Vec::projectOnAxis: axis direction is not normalized " <<std::endl;


    return  (  glm::dot(_vec , direction)  / squaredNorm(direction) ) * direction;
}



class  Quaternion 
{

    private:

        double q[4];


    public:

        Quaternion() 
        {
            q[0] = q[1] = q[2] = 0.0;
            q[3] = 1.0;
        }

        
        void setAxisAngle(const glm::vec3 &axis, double angle) 
        {
            const double norm = glm::length(axis);
            if (axis == glm::vec3(0.0f) ) 
            {
                // Null rotation
                q[0] = 0.0;
                q[1] = 0.0;
                q[2] = 0.0;
                q[3] = 1.0;
            } 
            else 
            {
                const double sin_half_angle = sin(angle / 2.0);
                q[0] = sin_half_angle * axis[0] / norm;
                q[1] = sin_half_angle * axis[1] / norm;
                q[2] = sin_half_angle * axis[2] / norm;
                q[3] = cos(angle / 2.0);
            }
        }

        Quaternion(const glm::vec3 &axis, double angle) { setAxisAngle(axis, angle); }

        /*Quaternion(const glm::vec3 &from, const glm::vec3 &to)
        {
            const double epsilon = 1E-10;

            const double fromSqNorm = from.squaredNorm(); // @@
            const double toSqNorm = to.squaredNorm();  // @@
            // Identity Quaternion when one vector is null
            if ((fromSqNorm < epsilon) || (toSqNorm < epsilon)) 
            {
                q[0] = q[1] = q[2] = 0.0;
                q[3] = 1.0;
            } 
            else 
            {
                glm::vec3 axis = cross(from, to);
                const double axisSqNorm = axis.squaredNorm();

                // Aligned vectors, pick any axis, not aligned with from or to
                if (axisSqNorm < epsilon)
                    axis = from.orthogonalVec(); // @@

                double angle = asin(sqrt(axisSqNorm / (fromSqNorm * toSqNorm)));

                if (from * to < 0.0)
                    angle = M_PI - angle;

                setAxisAngle(axis, angle);
            }
        }*/


        Quaternion(double q0, double q1, double q2, double q3) 
        {
            q[0] = q0;
            q[1] = q1;
            q[2] = q2;
            q[3] = q3;
        }


        Quaternion(const Quaternion &Q) 
        {
            for (int i = 0; i < 4; ++i)
                q[i] = Q.q[i];
        }


        Quaternion &operator=(const Quaternion &Q) 
        {
            for (int i = 0; i < 4; ++i)
                q[i] = Q.q[i];

            return (*this);
        }


        void setValue(double q0, double q1, double q2, double q3) 
        {
            q[0] = q0;
            q[1] = q1;
            q[2] = q2;
            q[3] = q3;
        }



        void setFromRotationMatrix(const glm::mat3 m)
        {
            // Compute one plus the trace of the matrix
            const double onePlusTrace = 1.0 + m[0][0] + m[1][1] + m[2][2];

            if (onePlusTrace > 1E-5) 
            {
                // Direct computation
                const double s = sqrt(onePlusTrace) * 2.0;
                q[0] = (m[2][1] - m[1][2]) / s;
                q[1] = (m[0][2] - m[2][0]) / s;
                q[2] = (m[1][0] - m[0][1]) / s;
                q[3] = 0.25 * s;
            } 
            else 
            {
                // Computation depends on major diagonal term
                if ((m[0][0] > m[1][1]) & (m[0][0] > m[2][2])) 
                {
                    const double s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2.0;
                    q[0] = 0.25 * s;
                    q[1] = (m[0][1] + m[1][0]) / s;
                    q[2] = (m[0][2] + m[2][0]) / s;
                    q[3] = (m[1][2] - m[2][1]) / s;
                } 
                else if (m[1][1] > m[2][2]) 
                {
                    const double s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2.0;
                    q[0] = (m[0][1] + m[1][0]) / s;
                    q[1] = 0.25 * s;
                    q[2] = (m[1][2] + m[2][1]) / s;
                    q[3] = (m[0][2] - m[2][0]) / s;
                } 
                else 
                {
                    const double s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2.0;
                    q[0] = (m[0][2] + m[2][0]) / s;
                    q[1] = (m[1][2] + m[2][1]) / s;
                    q[2] = 0.25 * s;
                    q[3] = (m[0][1] - m[1][0]) / s;
                }
            }
            normalize();
        }



        void setFromRotatedBasis(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z)
        {
            glm::mat3 m(0.0f);
            double normX = glm::length(X);
            double normY = glm::length(Y);
            double normZ = glm::length(Z);

            for (int i = 0; i < 3; ++i) 
            {
                m[i][0] = X[i] / normX;
                m[i][1] = Y[i] / normY;
                m[i][2] = Z[i] / normZ;
            }

            setFromRotationMatrix(m);
        }


        void setFromRotatedBase(const glm::vec3 &X, const glm::vec3 &Y, const glm::vec3 &Z)
        {
            std::cerr << "setFromRotatedBase is deprecated, use setFromRotatedBasis instead" << std::endl;
            setFromRotatedBasis(X, Y, Z);
        }



        glm::vec3 axis() const
        {
            glm::vec3 res = glm::vec3(q[0], q[1], q[2]);
            const double sinus = glm::length(res);
            if (sinus > 1E-8)
                res /= sinus;
            return (acos(q[3]) <= M_PI / 2.0) ? res : -res;
        }


        double angle() const
        {
            const double angle = 2.0 * acos(q[3]);
            return (angle <= M_PI) ? angle : 2.0 * M_PI - angle;
        }


        void getAxisAngle(glm::vec3 &axis, double &angle) const
        {
            angle = 2.0 * acos(q[3]);
            axis = glm::vec3 (q[0], q[1], q[2]);
            const double sinus = glm::length(axis);
            if (sinus > 1E-8)
            axis /= sinus;

            if (angle > M_PI)
            {
                angle = 2.0 * double(M_PI) - angle;
                axis = -axis;
            }
        }



        double operator[](int i) const { return q[i]; }


        double &operator[](int i) { return q[i]; }
  
        friend Quaternion operator*(const Quaternion &a, const Quaternion &b) 
        {
            return Quaternion(  a.q[3] * b.q[0] + b.q[3] * a.q[0] + a.q[1] * b.q[2] - a.q[2] * b.q[1],
                                a.q[3] * b.q[1] + b.q[3] * a.q[1] + a.q[2] * b.q[0] - a.q[0] * b.q[2],
                                a.q[3] * b.q[2] + b.q[3] * a.q[2] + a.q[0] * b.q[1] - a.q[1] * b.q[0],
                                a.q[3] * b.q[3] - b.q[0] * a.q[0] - a.q[1] * b.q[1] - a.q[2] * b.q[2]);
        }

  
        Quaternion &operator*=(const Quaternion &q) 
        {
            *this = (*this) * q;
            return *this;
        }

 
        friend glm::vec3 operator*(const Quaternion &q, const glm::vec3 &v) 
        {
            return q.rotate(v);
        }


        glm::vec3 rotate(const glm::vec3 &v) const
        {
            const double q00 = 2.0 * q[0] * q[0];
            const double q11 = 2.0 * q[1] * q[1];
            const double q22 = 2.0 * q[2] * q[2];

            const double q01 = 2.0 * q[0] * q[1];
            const double q02 = 2.0 * q[0] * q[2];
            const double q03 = 2.0 * q[0] * q[3];

            const double q12 = 2.0 * q[1] * q[2];
            const double q13 = 2.0 * q[1] * q[3];

            const double q23 = 2.0 * q[2] * q[3];

            return glm::vec3( (1.0 - q11 - q22) * v[0] + (q01 - q23) * v[1] + (q02 + q13) * v[2],
                              (q01 + q23) * v[0] + (1.0 - q22 - q00) * v[1] + (q12 - q03) * v[2],
                              (q02 - q13) * v[0] + (q12 + q03) * v[1] + (1.0 - q11 - q00) * v[2] );
        }


        Quaternion inverse() const { return Quaternion(-q[0], -q[1], -q[2], q[3]); }


        void invert() 
        {
            q[0] = -q[0];
            q[1] = -q[1];
            q[2] = -q[2];
        }



        glm::vec3 inverseRotate(const glm::vec3 &v) const
        {
            return inverse().rotate(v);
        }
  


        void negate() 
        {
            invert();
            q[3] = -q[3];
        }


        double normalize() 
        {
            const double norm =  sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
            for (int i = 0; i < 4; ++i)
                q[i] /= norm;
            return norm;
        }


        Quaternion normalized() const 
        {
            double Q[4];
            const double norm = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
            for (int i = 0; i < 4; ++i)
                Q[i] = q[i] / norm;
            return Quaternion(Q[0], Q[1], Q[2], Q[3]);
        }


        glm::mat4 getMatrix() const
        {
            glm::mat4 m(0.0f);

            const double q00 = 2.0 * q[0] * q[0];
            const double q11 = 2.0 * q[1] * q[1];
            const double q22 = 2.0 * q[2] * q[2];

            const double q01 = 2.0 * q[0] * q[1];
            const double q02 = 2.0 * q[0] * q[2];
            const double q03 = 2.0 * q[0] * q[3];

            const double q12 = 2.0 * q[1] * q[2];
            const double q13 = 2.0 * q[1] * q[3];

            const double q23 = 2.0 * q[2] * q[3];

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

        const glm::mat4 matrix() const
        {
            static glm::mat4 m = getMatrix();
            return m;
        }


        glm::mat3 getRotationMatrix() const
        {
            glm::mat3 m(0.0f);
            glm::mat4 mat(0.0f);
            mat = getMatrix();
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    // Beware of transposition
                    m[i][j] = double(mat[j][i]);
                }
            return m;
        }


        glm::mat4 getInverseMatrix() const
        {
            return inverse().getMatrix();
        }

        const glm::mat4 inverseMatrix() const
        {
            static glm::mat4 m = getInverseMatrix();
            return m;
        }



        glm::mat3  getInverseRotationMatrix() const
        {
            glm::mat3 m(0.0f);
            glm::mat4 mat(0.0f);
            mat = getInverseMatrix();
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                {
                    // Beware of transposition
                    m[i][j] = double(mat[j][i]);
                }
            return m;
        }



        static Quaternion slerp(const Quaternion &a, const Quaternion &b, double t, bool allowFlip = true)
        {
            double cosAngle = Quaternion::dot(a, b);

            double c1, c2;
            // Linear interpolation for close orientations
            if ((1.0 - fabs(cosAngle)) < 0.01)
            {
                c1 = 1.0 - t;
                c2 = t;
            } 
            else 
            {
                // Spherical interpolation
                double angle = acos(fabs(cosAngle));
                double sinAngle = sin(angle);
                c1 = sin(angle * (1.0 - t)) / sinAngle;
                c2 = sin(angle * t) / sinAngle;
            }

            // Use the shortest path
            if (allowFlip && (cosAngle < 0.0))
                c1 = -c1;

            return Quaternion(c1 * a[0] + c2 * b[0], c1 * a[1] + c2 * b[1], c1 * a[2] + c2 * b[2], c1 * a[3] + c2 * b[3]);
        }



        static Quaternion squad(const Quaternion &a, const Quaternion &tgA, const Quaternion &tgB, const Quaternion &b, double t)
        {
            Quaternion ab = Quaternion::slerp(a, b, t);
            Quaternion tg = Quaternion::slerp(tgA, tgB, t, false);
            return Quaternion::slerp(ab, tg, 2.0 * t * (1.0 - t), false);
        }



        static double dot(const Quaternion &a, const Quaternion &b) 
        {
            return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
        }


        Quaternion log()
        {
            double len = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);

            if (len < 1E-6)
                return Quaternion(q[0], q[1], q[2], 0.0);
            else 
            {
                double coef = acos(q[3]) / len;
                return Quaternion(q[0] * coef, q[1] * coef, q[2] * coef, 0.0);
            }
        }

        Quaternion exp()
        {
            double theta = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);

            if (theta < 1E-6)
                return Quaternion(q[0], q[1], q[2], cos(theta));
            else 
            {
                double coef = sin(theta) / theta;
                return Quaternion(q[0] * coef, q[1] * coef, q[2] * coef, cos(theta));
            }
        }

        static Quaternion lnDif(const Quaternion &a, const Quaternion &b)
        {
            Quaternion dif = a.inverse() * b;
            dif.normalize();
            return dif.log();
        }

        static Quaternion squadTangent(const Quaternion &before, const Quaternion &center, const Quaternion &after)
        {
            Quaternion l1 = Quaternion::lnDif(center, before);
            Quaternion l2 = Quaternion::lnDif(center, after);
            Quaternion e;
            for (int i = 0; i < 4; ++i)
                e.q[i] = -0.25 * (l1.q[i] + l2.q[i]);
            e = center * (e.exp());

            // if (Quaternion::dot(e,b) < 0.0)
            // e.negate();

            return e;
        }







};

} // namespace qglviewer



#endif // QGLVIEWER_QUATERNION_H
