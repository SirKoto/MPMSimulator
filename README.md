# README

Projecte de Pol Martín Garcia de final de carrera d'informàtica.

Simulació de materials deformables amb MPM-MLS.

## Estat actual
Simulador 2D dissenyat unicament per a Windows, amb visual studio.

Simulador 3D multiplataforma. Simulació, visualització, simulació i exportació.

## Videos
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/C2.gif?raw=true" width="410px"> <img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/superDeffCor.gif?raw=true" width="410px">
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/cmp4.gif?raw=true" width="410px"> <img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/snow2.gif?raw=true" width="410px">
<img src="https://github.com/SirKoto/MPMSimulator/blob/master/images/hoursand1.gif?raw=true" width="410px">

## Organització de codi 3D
- _Simulator\_3D_: Classe que inclou el simulador escencial de materials deformables.
- _EntryPoint_: Main del software. Inclou la carrega de simulacions i el bucle de comportament.
- _ParticleStructures_: Header amb les creacions d'estructures de partícules per a simular.
- _Utils_: Classe amb funcionalitats utils, per separar dependències.
- _SimVisualizer_: Visualitzador gràfic. S'usa de:
    - _Shader.h_: Header per tractar els shaders.
    - _Camera.h_: Header amb les funcionalitats d'una camera per openGL.
    - *shaders*: Carpeta amb les shaders normals:
    - *shaderShadows*: Carpeta amb les shaders compatibles amb el shadow map.
- _IO/WriteSBF_: Classe d'escriptura de simulacions en binari al disc.
- _IO/ReadSBF_: Classe de lectura d'arxius SBF.
- _IO/FrameSBF_: Fotogrames compatibles amb ReadSBF, per a emmagatzemar la informació.
- _glad/glad.h_ : Header per compatibilitat del projecte amb GLAD.

## Carpetes
Es crearan automaticament les carpetes `sim_files` per la escriptura i lectura de totes les simulacions, i `gifs` per l'exportació de simulacions.

## DEPENDENCIES
* GLFW-3.3
* GLM
* GLAD
* stb_image
* OpenGL: min v3.3
* Eigen
* Boost
* Boost::filesystem

## Instal·lació
Es requereix de les dependencies anteriors, d'on Boost::filesystem es una llibreria compilada de sistema. I glfw3 ha de ser compilada previament.
Aquestes s'han de linkar.

La resta de llibreries es poden trobar a mode de headers, s'adjunten per comoditat.

El Simulador 3D inclou un arxiu `CMakeLists` per a la facil compilació amb aquesta eina.


## Links interessants

* [Tutorial MPM](https://nialltl.neocities.org/articles/mpm_guide.html)
* [GitHub Yuan MLS-MPM](https://github.com/yuanming-hu/taichi_mpm/)
* [Curs MPM](https://www.seas.upenn.edu/~cffjiang/research/mpmcourse/mpmcourse.pdf)
* [Snow Disney](http://alexey.stomakhin.com/research/siggraph2013_snow.pdf)
* [Tesis Jiang](https://www.math.ucla.edu/~jteran/student_thesis/jiang.pdf)


