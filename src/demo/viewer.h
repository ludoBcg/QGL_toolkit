/*********************************************************************************************************************
 *
 * viewer.h
 *
 * 3D viewer widget for Qt, based on QGLViewer
 * 
 * QGL_toolkit demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#include "QGLtoolkit/qglviewer.h"


class DrawableMesh;
class TriMesh;



class Viewer : public qgltoolkit::QGLViewer
{
    Q_OBJECT

    public:


        Viewer(QWidget *parent);
        Viewer();
        ~Viewer();


        

    protected:
        GLuint m_defaultVAO; 
        TriMesh* m_triMesh;
        DrawableMesh* m_drawMesh;

        glm::vec3 m_backCol;
        glm::vec3 m_lightPos;
        glm::vec3 m_lightCol;


        virtual void init();
        virtual void draw();
        void closeEvent(QCloseEvent *e);
        virtual std::string helpString() const;
        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e);
        void mouseMoveEvent(QMouseEvent *e);
        void resizeGL(int width, int height);
        void keyPressEvent(QKeyEvent *e);


};