/*********************************************************************************************************************
 *
 * window.cpp
 *
 * Qt window
 * 
 * QGL_toolkit demo
 * Ludovic Blache
 *
 *********************************************************************************************************************/

#include <QFileDialog>
#include <QColorDialog>
#include <QMimeData>


#include "window.h"

#include "viewer.h"


Window::Window() : QWidget()
{


    this->setWindowTitle("QGL_toolkit demo");
    this->move(300,100);

    /*******************************************************************************************/
    /************************************** Global Layout **************************************/
    /*******************************************************************************************/

    m_globalLayout = new QVBoxLayout(this);



    /**************************************** GL Viewer ****************************************/

    m_glViewer = new Viewer(this);
    m_glViewer->setMinimumSize(1024, 768);

    m_globalLayout->addWidget(m_glViewer);


}

Window::~Window()
{

    delete m_glViewer;
   
    delete m_globalLayout;
}

