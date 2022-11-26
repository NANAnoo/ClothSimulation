#include "RenderWidget.h"
#include <QMouseEvent>
#include <GLUT/glut.h>

RenderWidget::~RenderWidget()
{
}

void RenderWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    model = nullptr;
}

void RenderWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -10, 10);
}

void RenderWidget::paintGL()
{
    // clear buffer
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // set model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // transilation
    glTranslatef(0, -0.5, 0);

    // rotation
    glRotatef(rotation_x, 1, 0, 0);
    glRotatef(rotation_y, 0, 1, 0);

    glColor3f(0.8, 0.8, 1);
    for (unsigned int i = 0; i < 21; i ++) {
        float w = 0.05 * i - 0.5;
        if (i % 5 == 0) {
            glLineWidth(2);
        } else {
            glLineWidth(1);
        }
        glBegin(GL_LINES);
        glVertex3f(w, -0.01, -0.5);
        glVertex3f(w, -0.01, 0.5);
        glVertex3f(-0.5, -0.01, w);
        glVertex3f(0.5, -0.01, w);
        glEnd();
    }

    // light
    glEnable(GL_LIGHTING);
    GLfloat light_position[] = { 0.0, 20.0, 0.0, 0.0 }; 
    GLfloat light_ambient [] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
    glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
    glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse ); 
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
    glEnable(GL_LIGHT0);
    light_position[1] = -20.0;
    glLightfv(GL_LIGHT1, GL_POSITION, light_position); 
    glLightfv(GL_LIGHT1, GL_AMBIENT , light_ambient );
    glLightfv(GL_LIGHT1, GL_DIFFUSE , light_diffuse ); 
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular); 
    glEnable(GL_LIGHT1);
   
    // update model
    if (model != nullptr && model->isValid()) {
        model->render();
    }
}

// mouse handle
void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    // prepare rotation
    isDraging = true;
    // record current position
    last_mouse_position_x = event->x();
    last_mouse_position_y = event->y();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isDraging) {
        // find the movement
        float d_x = static_cast<float>(event->x() - last_mouse_position_x);
        float d_y = static_cast<float>(event->y() - last_mouse_position_y);
        // update rotation
        rotation_y += d_x;
        rotation_x += d_y;
        // update recorded mouse position
        last_mouse_position_x = event->x();
        last_mouse_position_y = event->y();
        // repaint
        update();
    }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // end rotation
    isDraging = false;
}
