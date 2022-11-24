#include "MainWindow.h"

#include <iostream>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    render = new RenderWidget(this);
    load_obj_btn = new QPushButton("load", this);
    connect(load_obj_btn, &QPushButton::released, this, &MainWindow::loadObj);

    stop_start_btn = new QPushButton("start", this);
    connect(stop_start_btn, &QPushButton::released, this, &MainWindow::startStop);

    save_current_frame = new QPushButton("save current frame", this);
    connect(save_current_frame, &QPushButton::released, this, &MainWindow::saveCurrentFrame);

    free_fall_btn = new QPushButton("free fall", this);
    connect(free_fall_btn, &QPushButton::released, this, &MainWindow::freeFall);

    fall_with_ball_btn = new QPushButton("fall to ball", this);
    connect(fall_with_ball_btn, &QPushButton::released, this, &MainWindow::freeFallToBall);

    fall_with_fixed_corners_btn = new QPushButton("fixed corners", this);
    connect(fall_with_fixed_corners_btn, &QPushButton::released, this, &MainWindow::fallFixedCorners);

    fall_with_spiningball_btn = new QPushButton("fall to spinball", this);
    connect(fall_with_spiningball_btn, &QPushButton::released, this, &MainWindow::fallToSpiningBall);

    wind_and_fixed_corners_btn = new QPushButton("fixed corners with wind", this);
    connect(wind_and_fixed_corners_btn, &QPushButton::released, this, &MainWindow::windAndFixedCorners);

    save_as_vedio = new QPushButton("save as video", this);
    connect(save_as_vedio, &QPushButton::released, this, &MainWindow::saveAsvideo);

    model = nullptr;
    is_playing = false;
    play_timer = new QTimer();
    play_timer->setInterval(1000.f / 60);
    play_timer->setTimerType(Qt::TimerType::PreciseTimer);
    play_timer->connect(play_timer, &QTimer::timeout, [=]()
                        { this->render->update(); });
    play_timer->start();
    cal_timer = new QTimer();
    cal_timer->setInterval(1);
    cal_timer->connect(cal_timer, &QTimer::timeout, [=]
                       { this->model->update(this->params); });
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadContent()
{
    // initialize
    render->setGeometry(10, 10, width() / 4 * 3 - 20, height() - 20);

    int left = width() / 4 * 3 + 10;
    int size_h = height() / 32;
    int size_w = width() / 4 - 20;
    int grid_size = height() / 16;
    // layout
    load_obj_btn->setGeometry(left, grid_size, size_w, size_h);
    stop_start_btn->setGeometry(left, 2 * grid_size, size_w, size_h);
    save_current_frame->setGeometry(left, 3 * grid_size, size_w, size_h);
    free_fall_btn->setGeometry(left, 5 * grid_size, size_w, size_h);
    fall_with_ball_btn->setGeometry(left, 6 * grid_size, size_w, size_h);
    fall_with_fixed_corners_btn->setGeometry(left, 7 * grid_size, size_w, size_h);
    fall_with_spiningball_btn->setGeometry(left, 8 * grid_size, size_w, size_h);
    wind_and_fixed_corners_btn->setGeometry(left, 9 * grid_size, size_w, size_h);
    save_as_vedio->setGeometry(left, 13 * grid_size, size_w, size_h);
}

void MainWindow::loadObj()
{
    // open a new file picker to open file
    QFileDialog opener(this);
    opener.setFileMode(QFileDialog::ExistingFile);
    opener.setNameFilter(tr("OBJ files (*.obj)"));
    opener.setViewMode(QFileDialog::Detail);
    opener.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList filePaths;
    std::string file_path;
    if (opener.exec())
    {
        filePaths = opener.selectedFiles();
        if (filePaths.size() > 0)
        {
            file_path = filePaths[0].toStdString();
        }
    }
    if (file_path.size() > 0)
    {
        // get file path
        if (model != nullptr)
        {
            delete model;
        }
        // create new object
        model = new ClothModel(file_path);
        render->setupModel(model);
    }
}

void MainWindow::startStop()
{
    if (is_playing)
    {
        cal_timer->stop();
        this->stop_start_btn->setText("start");
    }
    else
    {
        cal_timer->start();
        this->stop_start_btn->setText("stop");
    }
    is_playing = !is_playing;
}

void MainWindow::saveCurrentFrame()
{
}

void MainWindow::freeFall()
{
    this->model->reset();
    this->model->transilation({0, 0.5, 0});
    params.dt = 0.001;
    params.E = 100000;
    params.kd = 50;
    params.fric = 0.7;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0;
    params.omega = 0;
    params.fric_w = 0.1;
    params.wind = {};
}

void MainWindow::freeFallToBall()
{
    this->model->reset();
    this->model->transilation({0, 0.5, 0});
    params.dt = 0.001;
    params.fric = 1;
    params.E = 100000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0.15;
    params.sphere_center = {0, 0.15, 0};
    params.omega = 0;
    params.fixed_corner = false;
    params.fric_w = 0.2;
    params.wind = {};
}

void MainWindow::fallFixedCorners()
{
    this->model->reset();
    this->model->transilation({0, 1, 0.5});
    params.dt = 0.001;
    params.fric = 0.7;
    params.E = 100000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0;
    params.sphere_center = {0, 0, 0};
    params.omega = 0;
    params.fixed_corner = true;
    params.fric_w = 0.1;
    params.wind = {};
}

void MainWindow::fallToSpiningBall()
{
    this->model->reset();
    this->model->transilation({0, 0.5, 0});
    params.dt = 0.001;
    params.fric = 0.5;
    params.E = 100000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0.15;
    params.sphere_center = {0, 0.3, 0};
    params.omega = 10;
    params.fixed_corner = false;
    params.fric_w = 0.1;
    params.wind = {};
}

void MainWindow::windAndFixedCorners()
{
    this->model->reset();
    this->model->transilation({0, 1, 0});
    params.dt = 0.001;
    params.fric = 0.7;
    params.E = 100000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0;
    params.sphere_center = {0, 0, 0};
    params.omega = 0;
    params.fixed_corner = true;
    params.fric_w = 0.1;
    params.wind = {0, 0, 10};
}

void MainWindow::saveAsvideo()
{
}
