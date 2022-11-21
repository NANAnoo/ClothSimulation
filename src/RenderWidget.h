#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>

#include "ClothModel.h"

class RenderWidget : public QOpenGLWidget
{
public:
    RenderWidget(QWidget *parent) : QOpenGLWidget(parent){
        isDraging = false;
        last_mouse_position_x = 0;
        last_mouse_position_y = 0;
        rotation_x = 0;
        rotation_y = 0;
    }
    // set up model
    void setupModel(ClothModel *model) {
        this->model = model;
        update();
    }
    // dealloc
    ~RenderWidget();

protected:
    ClothModel *model;
    // mouse event handler
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // gl functions
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    bool isDraging;
    int last_mouse_position_x;
    int last_mouse_position_y;
    float rotation_x;
    float rotation_y;
};

#endif // RENDERWIDGET_H
