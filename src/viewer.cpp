
//#include <QMouseEvent>
//#include <QTimer>
//#include <QElapsedTimer>
//#include <QImage>

//#define QT_NO_OPENGL_ES_2
//#include <GL/glew.h>

#include "drawablemesh.h"
#include "trimeshsoup.h"

#include "viewer.h"



using namespace std;




Viewer::Viewer(QWidget *parent) : qgltoolkit::QGLViewer(parent)
{ }

Viewer::Viewer() : qgltoolkit::QGLViewer()
{ }

Viewer::~Viewer()
{
    delete m_drawMesh;
    delete m_triMesh;
    std::cout << std::endl << "Bye!" << std::endl;
}





void Viewer::init()
{

    // Load OpenGL functions
    glewExperimental = true;
    GLenum status = glewInit();
    if (status != GLEW_OK) 
    {
        std::cerr << "Error: " << glewGetErrorString(status) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << std::endl
              << "Welcome to Mesh_viewer" << std::endl << std::endl 
              << "Press H for help" << std::endl
              << "OpenGL version: " << glGetString(GL_VERSION) << std::endl
              << "Vendor: " << glGetString(GL_VENDOR) << std::endl << std::endl
              << "Log:" << std::endl;
              
        glGenVertexArrays(1, &m_defaultVAO);
    glBindVertexArray(m_defaultVAO);

      glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);


  glViewport(0, 0, width(), height());

    m_triMesh = new TriMeshSoup(true, false, false);
    m_triMesh->readFile("../../models/misc/teapot.obj");
    m_triMesh->computeAABB();

    m_drawMesh = new DrawableMesh;
    m_drawMesh->setProgram("../../src/shaders/phong_2.vert", "../../src/shaders/phong_2.frag");
    m_drawMesh->createVAO(m_triMesh);


    // scene setup
    glm::vec3 bBoxMin = m_triMesh->getBBoxMin();
    glm::vec3 bBoxMax = m_triMesh->getBBoxMax();
    std::cout << "00" << std::endl;
    if(bBoxMin != bBoxMax)
    {
        // scene setup
        glm::vec3 min(bBoxMin.x, bBoxMin.y, bBoxMin.z);
        glm::vec3 max(bBoxMax.x, bBoxMax.y, bBoxMax.z);
        glm::vec3 centerCoords( (bBoxMin.x + bBoxMax.x) * 0.5f, (bBoxMin.y + bBoxMax.y) * 0.5f, (bBoxMin.z + bBoxMax.z) * 0.5f );
std::cout<<"center = "<<centerCoords.x<<" "<<centerCoords.y<<" "<<centerCoords.z<<std::endl;
        std::cout << "0" << std::endl;
        this->setSceneBoundingBox(min, max);
        std::cout << "1" << std::endl;
        // camera setup
        this->camera()->setPosition( centerCoords + glm::vec3(0.0f, 0.0f, this->sceneRadius()*2.5f) );
        this->camera()->setViewDirection(centerCoords - this->camera()->position() );
        this->camera()->setUpVector( glm::vec3(0.0f, 1.0f, 0.0f) );

        //update();
    }
    std::cout << "11" << std::endl;

    m_lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
}


void Viewer::draw()
{
    
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    //std::cout<<"draw"<<std::endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST); // ensures that polygons overlap correctly
    //glDepthFunc(GL_LESS); 

    

    this->camera()->computeProjectionMatrix();
//this->camera()->center_ = this->sceneCenter();
    this->camera()->computeViewMatrix();

    glm::mat4 mv = this->camera()->getViewMatrix();
    glm::mat4 projection = this->camera()->getProjectionMatrix();
    glm::mat4 mvp = projection * mv;



    // get camera position
    glm::vec3 cam_pos(this->camera()->position().x, this->camera()->position().y, this->camera()->position().z);
//std::cout<<"cam position = " << cam_pos.x << " "<< cam_pos.y << " "<< cam_pos.z<< std::endl;
//std::cout<<"mvp = " << mvp[0][0] << " "<<  mvp[0][1] << " "<<  mvp[0][2]<< " "<<  mvp[0][3]<< 
//                " " << mvp[1][0] << " "<<  mvp[1][1] << " "<<  mvp[1][2]<< " "<<  mvp[1][3]<<
//                " " << mvp[2][0] << " "<<  mvp[2][1] << " "<<  mvp[2][2]<< " "<<  mvp[2][3]<< 
//                " " << mvp[3][0] << " "<<  mvp[3][1] << " "<<  mvp[3][2]<< " "<<  mvp[3][3]<< std::endl;
    m_drawMesh->draw(mv, mvp, /*cam_pos*/glm::vec3(0,0,4), m_lightCol);

    //update();
}


void Viewer::closeEvent(QCloseEvent *e)
{
    // Removed: saveStateToFile(); (caused crashing when closing window)
	QOpenGLWidget::closeEvent(e);
}


QString Viewer::helpString() const
{
    QString text("<h2>S i m p l e V i e w e r</h2>");
    text += "Use the mouse to move the camera around the object. ";
    text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
    text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
    text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
    text += "Simply press the function key again to restore it. Several keyFrames define a ";
    text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
    text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
    text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
    text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
    text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
    text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
    text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
    text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
    text += "Press <b>Escape</b> to exit the viewer.";
    return text;
}


void Viewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
}


void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    QGLViewer::mouseReleaseEvent(e);
    // avoid camera spinning
    //camera()->frame()->stopSpinning();
}


void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    QGLViewer::mouseMoveEvent(e);  
}


void Viewer::resizeGL(int width, int height)
{
    QGLViewer::resizeGL(width, height);
    glViewport(0, 0, width, height);
}


void Viewer::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_F)
    {
        std::cout<<"pressed F"<<std::endl;
        std::cout<<this->width()<<std::endl;
        std::cout<<this->height()<<std::endl;
    }
     std::cout<<this->width()<<std::endl;
        std::cout<<this->height()<<std::endl;
    QGLViewer::keyPressEvent(e);
}

