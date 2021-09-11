

#include "trimesh.h"

#include "viewer.h"



using namespace std;




Viewer::Viewer(QWidget *parent) : qgltoolkit::QGLViewer(parent)
{ }

Viewer::Viewer() : qgltoolkit::QGLViewer()
{ }

Viewer::~Viewer()
{
//    delete m_drawMesh;
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


    glViewport(0, 0, width(), height());

    m_triMesh = new TriMesh();
    m_triMesh->readFile("../../models/misc/teapot.obj");
    m_triMesh->computeAABB();
    m_triMesh->setProgram("../../src/demo/shaders/phong.vert", "../../src/demo/shaders/phong.frag");
    m_triMesh->createVAO();


    // scene setup
    glm::vec3 bBoxMin = m_triMesh->getBBoxMin();
    glm::vec3 bBoxMax = m_triMesh->getBBoxMax();

    if(bBoxMin != bBoxMax)
    {
        // scene setup
        glm::vec3 min(bBoxMin.x, bBoxMin.y, bBoxMin.z);
        glm::vec3 max(bBoxMax.x, bBoxMax.y, bBoxMax.z);
        glm::vec3 centerCoords( (bBoxMin.x + bBoxMax.x) * 0.5f, (bBoxMin.y + bBoxMax.y) * 0.5f, (bBoxMin.z + bBoxMax.z) * 0.5f );
std::cout<<"center = "<<centerCoords.x<<" "<<centerCoords.y<<" "<<centerCoords.z<<std::endl;

        // Set scene radius and center from AABBox.
        // Pivot point is set to scene center by default, 
        // use camera()->setPivotPoint() to change it.
        this->setSceneBoundingBox(min, max);
std::cout<<"this->camera()->sceneCenter() = "<<this->camera()->sceneCenter().x<<" "<<this->camera()->sceneCenter().y<<" "<<this->camera()->sceneCenter().z<<std::endl;
std::cout<<"this->camera()->pivotPoint() = "<<this->camera()->pivotPoint().x<<" "<<this->camera()->pivotPoint().y<<" "<<this->camera()->pivotPoint().z<<std::endl;
        // camera setup
        this->camera()->setPosition( centerCoords + glm::vec3(0.0f, 0.0f, this->sceneRadius()*2.5f) );
        this->camera()->setViewDirection(centerCoords - this->camera()->position() );
        this->camera()->setUpVector( glm::vec3(0.0f, 1.0f, 0.0f) );

    }

    m_lightCol = glm::vec3(1.0f, 1.0f, 1.0f);
}


void Viewer::draw()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST); 
    

    this->camera()->computeProjectionMatrix();
//this->camera()->center_ = this->sceneCenter();
    this->camera()->computeViewMatrix();

    glm::mat4 mv = this->camera()->viewMatrix();
    glm::mat4 projection = this->camera()->projectionMatrix();
    glm::mat4 mvp = projection * mv;


    // get camera position
    glm::vec3 cam_pos(this->camera()->position().x, this->camera()->position().y, this->camera()->position().z);

    m_triMesh->draw(mv, mvp, cam_pos , m_lightCol);

}


void Viewer::closeEvent(QCloseEvent *e)
{
    // Removed: saveStateToFile(); (caused crashing when closing window)
	QOpenGLWidget::closeEvent(e);
}


std::string Viewer::helpString() const
{
    return QGLViewer::helpString();
}


void Viewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
}


void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    QGLViewer::mouseReleaseEvent(e);
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
    }
    if (e->key() == Qt::Key_H)
    {
        help();
    }

    std::cout<<this->width()<<std::endl;
    std::cout<<this->height()<<std::endl;
    QGLViewer::keyPressEvent(e);
}

