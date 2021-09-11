#ifndef WINDOW_H
#define WINDOW_H


#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QToolTip>
#include <QDropEvent>


class Viewer;

/*!
* \class Window
* \brief Main application window
* Inherit from QWidget
*/
class Window : public QWidget
{
    Q_OBJECT

    public:
        Window();
        ~Window();

    private:



        QVBoxLayout* m_globalLayout;        /*!< Global vertical layout of the window */

        Viewer* m_glViewer;                 /*!< QGLViewer, contains the OpenGL context */
 


    public slots:

        
};

#endif // WINDOW_H