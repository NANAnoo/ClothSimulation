#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <functional>
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
        buffer_handle = 0;
        is_recording = false;
    }
    // set up model
    void setupModel(ClothModel *model) {
        this->model = model;
        update();
    }
    // record_widget;
    void startRecordWidget(std::function<void(unsigned char *)> callback)
    {
        framedata_callback = callback;
        is_recording = true;
    }
    // stop recording
    void stopRecord()
    {
        is_recording = false;
        framedata_callback = nullptr;
    }

    // dealloc
    ~RenderWidget();

protected:
    ClothModel *model;
    // mouse event handler
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // frame buffer
    std::function<void(unsigned char *)> framedata_callback;
    GLuint buffer_handle;
    bool is_recording;
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
