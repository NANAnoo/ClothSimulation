#include "RenderWidget.h"
#include <QMouseEvent>

RenderWidget::~RenderWidget()
{
}

void RenderWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
}

void RenderWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 20);
}

void RenderWidget::paintGL()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // set model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // rotation
    glRotatef(rotation_x, 1, 0, 0);
    glRotatef(rotation_y, 0, 1, 0);

    // draw axies
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
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
