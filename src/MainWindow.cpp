#include "MainWindow.h"

#include <iostream>
#include <fstream>
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

    load_scene_btn = new QPushButton("load scene from file", this);
    connect(load_scene_btn, &QPushButton::released, this, &MainWindow::loadScene);

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
    load_scene_btn->setGeometry(left, 10 * grid_size, size_w, size_h);
    save_as_vedio->setGeometry(left, 13 * grid_size, size_w, size_h);

    //disable
    stop_start_btn->setDisabled(true);
    save_current_frame->setDisabled(true);
    free_fall_btn->setDisabled(true);
    fall_with_ball_btn->setDisabled(true);
    fall_with_fixed_corners_btn->setDisabled(true);
    fall_with_spiningball_btn->setDisabled(true);
    wind_and_fixed_corners_btn->setDisabled(true);
    load_scene_btn->setDisabled(true);
    save_as_vedio->setDisabled(true);
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
    
        if (model->isValid()) {
            render->setupModel(model);
            free_fall_btn->setDisabled(false);
            fall_with_ball_btn->setDisabled(false);
            fall_with_fixed_corners_btn->setDisabled(false);
            fall_with_spiningball_btn->setDisabled(false);
            wind_and_fixed_corners_btn->setDisabled(false);
            load_scene_btn->setDisabled(false);
            if (!is_playing) {
                save_current_frame->setDisabled(true);
            }
        } else {
            // toast
        }
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
    save_current_frame->setDisabled(is_playing);
    save_as_vedio->setDisabled(is_playing);
}

void MainWindow::saveCurrentFrame()
{
    // open a new file picker to save file
    QFileDialog opener(this);
    opener.setFileMode(QFileDialog::AnyFile);
    opener.setNameFilter(tr("object files (*.obj)"));
    opener.setViewMode(QFileDialog::List);
    opener.setAcceptMode(QFileDialog::AcceptSave);

    QStringList filePaths;
    if (opener.exec()) {
        filePaths = opener.selectedFiles();
        if (filePaths.size() > 0) {
            model->saveAsFile(filePaths[0].toStdString());
        }
    }
}

void MainWindow::freeFall()
{
    this->model->reset();
    this->model->transilation({0, 0.5, 0});
    params.dt = 0.0015;
    params.E = 400000;
    params.kd = 50;
    params.fric = 0.7;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0;
    params.sphere_center = {0, 0, 0};
    params.omega = 0;
    params.fric_w = 0.1;
    params.fixed_corner = false;
    params.wind = {};
    stop_start_btn->setDisabled(false);
}

void MainWindow::freeFallToBall()
{
    this->model->reset();
    this->model->transilation({0, 0.2, 0});
    params.dt = 0.0015;
    params.fric = 1;
    params.E = 400000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0.1;
    params.sphere_center = {0, 0.1, 0};
    params.omega = 0;
    params.fixed_corner = false;
    params.fric_w = 0.1;
    params.wind = {};
    stop_start_btn->setDisabled(false);
}

void MainWindow::fallFixedCorners()
{
    this->model->reset();
    this->model->transilation({0, 1, 0.5});
    params.dt = 0.0015;
    params.fric = 0.7;
    params.E = 400000;
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
    stop_start_btn->setDisabled(false);
}

void MainWindow::fallToSpiningBall()
{
    this->model->reset();
    this->model->transilation({0, 0.5, 0});
    params.dt = 0.0015;
    params.fric = 0.5;
    params.E = 400000;
    params.kd = 50;
    params.gravity = {0, -1, 0};
    params.ground_normal = {0, 1, 0};
    params.ground_position = {0, 0, 0};
    params.mass = 1;
    params.sphere_r = 0.1;
    params.sphere_center = {0, 0.4, 0};
    params.omega = 10;
    params.fixed_corner = false;
    params.fric_w = 0.1;
    params.wind = {};
    stop_start_btn->setDisabled(false);
}

void MainWindow::windAndFixedCorners()
{
    this->model->reset();
    this->model->transilation({0, 1, 0});
    params.dt = 0.0015;
    params.fric = 0.7;
    params.E = 400000;
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

void MainWindow::loadScene()
{
    QFileDialog opener(this);
    opener.setFileMode(QFileDialog::ExistingFile);
    opener.setNameFilter(tr("Scene files (*.txt)"));
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
        } else {
            return;
        }
    }
    // set up default scene
    freeFall();

    // read scene from file
    std::ifstream file;
    file.open(file_path, std::ios::in);
    if (file.bad()) {
        std::cout << "File open failed, use freeBall" << std::endl;
        return;
    }
    std::string token;
    while(!file.eof())
    {
        file >> token;
        float x, y, z;
        if (token == "transilation") {
            file >> x >> y >> z;
            this->model->reset();
            this->model->transilation({x, y, z});
        } else if (token == "dt") {
            file >> params.dt; 
        } else if (token == "fric") {
            file >> params.fric;
        } else if (token == "E") {
            file >> params.E;
        } else if (token == "kd") {
            file >> params.kd;
        } else if (token == "gravity") {
            file >> x >> y >> z;
            params.gravity = {x, y, z};
        } else if (token == "ground_position") {
            file >> x >> y >> z;
            params.ground_position = {x, y, z};
        } else if (token == "ground_normal") {
            file >> x >> y >> z;
            params.ground_normal = {x, y, z};
        } else if (token == "mass") {
            file >> params.mass;
        } else if (token == "sphere_r") {
            file >> params.sphere_r;
        } else if (token == "sphere_center") {
            file >> x >> y >> z;
            params.sphere_center = {x, y, z};
        } else if (token == "omega") {
            file >> params.omega;
        } else if (token == "fric_w") {
            file >> params.fric_w;
        } else if (token == "wind") {
            file >> x >> y >> z;
            params.wind = {x, y, z};
        } else if (token == "fixed_corner") {
            file >> params.fixed_corner;
        }
    }
}

void MainWindow::saveAsvideo()
{
}
