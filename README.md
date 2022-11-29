# [ClothSimulation](https://github.com/NANAnoo/ClothSimulation)
    ** This is my own git repository link, which is private now and only used for code management !! **

- Description
  - A mass-string model to simulate clothes
  - Severial examples are given, u can customise a scene with a .txt file as below:

```txt
# transilate the object
transilation 0 1 0.5

# time step
dt 0.0015

# friction fractor
fric 0.7

# string Young's Module fractor
E 500000

# damped fractor
kd 50

# direction of gravity
gravity 0 -1 0

# gound informations
ground_normal 0 1 0
ground_position 0 0 0

# mass of each joint
mass 1

# add a sphere into scene
sphere_r 0
sphere_center 0 0 0

# angular velocity of the sphere, along Y+ axis
omega 0

# enable corner fix
fixed_corner 1

# friction fractor with the wind or air
fric_w 0.5

# the velosity of the wind
wind 0 0.2 -1
```

- How to build
  - environment require:
    - opencv
    - opengl+glut+glew
    - QT 5.15.0
  - commands:
  
```bash
% mkdir build && cd build
% qmake ../
% # if M1 mac, should change the compile architecture
% sed -i \"\" 's/x86_64/arm64/' Makefile
% # endif
% make -j20
```

- 3rd Librarys References
  - [tiny_obj_loader](https://github.com/tinyobjloader/tinyobjloader"), load obj file and materials
  - [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h), load image file as texture
  - [opencv](https://github.com/opencv/opencv), write a video
  
- Resources References
  - [texture image file](https://www.pexels.com/photo/photo-of-acrylic-paint-2832432/), as the texture of the square mesh