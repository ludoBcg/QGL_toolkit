/*********************************************************************************************************************
 *
 * constraint.h
 *
 * A Constraint acts as a filter on the translation and rotation Frame increments
 * 
 * QGL_toolkit
 * Ludovic Blache
 *
 *
 * Based on the libQGLViewer library by Gilles Debunne
 * http://www.libqglviewer.com
 *
 *********************************************************************************************************************/

#ifndef QGLTOOLKIT_CONSTRAINT_H
#define QGLTOOLKIT_CONSTRAINT_H


#include "quaternion.h"


namespace qgltoolkit {



class Constraint 
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


} // namespace qgltoolkit

#endif // QGLTOOLKIT_CONSTRAINT_H
