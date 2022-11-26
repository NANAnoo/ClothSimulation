#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

#include <QPushButton>
#include <QTimer>
#include <string>

#include "RenderWidget.h"
#include "ClothModel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // set layout
    void loadContent();

    RenderWidget * render;

private:
    QTimer *cal_timer;
    QTimer *play_timer;
    ClothModel::SimulationParameters params;
    QPushButton *load_obj_btn;
    void loadObj();

    QPushButton *stop_start_btn;
    void startStop();
    bool is_playing;

    QPushButton *save_current_frame;
    void saveCurrentFrame();

    // all scenes:
    QPushButton *free_fall_btn;
    void freeFall();
    QPushButton *fall_with_ball_btn;
    void freeFallToBall();
    QPushButton *fall_with_fixed_corners_btn;
    void fallFixedCorners();
    QPushButton *fall_with_spiningball_btn;
    void fallToSpiningBall();
    QPushButton *wind_and_fixed_corners_btn;
    void windAndFixedCorners();
    QPushButton *load_scene_btn;
    void loadScene();

    QPushButton *save_as_vedio;
    void saveAsvideo();

    // model
    ClothModel *model;
};

#endif // MAINWINDOW_H