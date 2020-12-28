# README

Informatics Engineering final thesis.

Simulation of deformable materials with MPM-MLS.

Thesis document can also be found on [UPCommons](https://upcommons.upc.edu/handle/2117/190540).

Awarded best informatics engineering bachelor final thesis 2019-2020 on the Barcelona School of Informatics  (FIB).

## Current State
2D simulator, working only for Windows.

3D simulator (multi-platform). Simulation, visualization, interaction and storage.
- CMakeLists needs update to account for the CUDA new implementation

## Videos
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/C2.gif?raw=true" width="410px"> <img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/superDeffCor.gif?raw=true" width="410px">
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/cmp4.gif?raw=true" width="410px"> <img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/snow2.gif?raw=true" width="410px">
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/hoursand1.gif?raw=true" width="410px">

## 3D simulator code organization
- _Simulator\_3D_: CPU 3D simulator.
- _EntryPoint_: Main of the program. Game loop and loading of simulations.
- _ParticleStructures_: Header with the different particle distributions to simulate.
- _Utils_: Constants, IO, and utility functions.
- _SimVisualizer_: Graphic utilities. Uses:
    - _Shader.h_: Header to process shaders.
    - _Camera.h_: Header to update the camera and generate its information.
    - *shaders*: folder with the actual GLSL shaders:
    - *shaderShadows*: folder with the shadow mapping GLSL shaders.
- _IO/WriteSBF_: Writing of simulation binary files to disk.
- _IO/ReadSBF_: Reading of simulation binary files to disk.
- _IO/FrameSBF_: Frames compatible with the simulations, to store and read these.
- _glad/glad.h_: GLAD Header.
- _cuda/MinMath.h_: Custom linear algebra library adapted for CUDA.
- _cuda/Simulator_3D_GPU.h_: Interface to run the simulation on the GPU and retrieve the information.
- _cuda/Sim3D.cu_: CUDA kernels of the simulation.
- _cuda/svd3_cuda.cuh_: CUDA header with the SVD implementation (by Kui Wu)

## Folders
The folders `sim_files` and `gifs` will be automatically created to store and read simulations, and for their exportation as gif files.

## DEPENDENCES
* GLFW-3.3
* GLM
* GLAD
* stb_image
* OpenGL: min v3.3
* Eigen
* Boost
* Boost::filesystem
* CUDA

## Install
The previous dependences are required. In particular `Boost::filesystem` is a system dependent part of the Boost library, and `OpenGL`, `glfw3` and `CUDA` must also be in the system previously.

The other libraries are included on the project.

## Interesting links

* [Tutorial MPM](https://nialltl.neocities.org/articles/mpm_guide.html)
* [GitHub Yuan MLS-MPM](https://github.com/yuanming-hu/taichi_mpm/)
* [Curs MPM](https://www.seas.upenn.edu/~cffjiang/research/mpmcourse/mpmcourse.pdf)
* [Snow Disney](http://alexey.stomakhin.com/research/siggraph2013_snow.pdf)
* [Tesis Jiang](https://www.math.ucla.edu/~jteran/student_thesis/jiang.pdf)


