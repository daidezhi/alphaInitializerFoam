# alphaInitializerFoam

## Introduction

```alphaInitializerFoam``` is a pre-processing utility designed to initialize the alpha (VOF) field with an input shape surface mesh in .stl format. It works on  polyhedral unstructured meshes. The previous fluid shape will be retained.

Another copy of alphaInitializerFoam and demo cases can be found in:

Dai, Dezhi (2019), “alphaInitializerFoam”, Mendeley Data, v1

http://dx.doi.org/10.17632/wg4sx7sc57.1


---

## Methodology

The basic idea of ```alphaInitializerFoam``` follows:

* compute the intersection of the surface mesh (blue), which represents the liquid or gas shape, and each cell (red) in the mesh,
![shape surface mesh and cell](https://en.wikipedia.org/wiki/Constructive_solid_geometry#/media/File:Boolean_union.PNG)
![intersection](https://en.wikipedia.org/wiki/Constructive_solid_geometry#/media/File:Boolean_intersect.PNG)
* initialize the fraction value by normalizing the intersection volume with the cell one.




---

## Compatibility

The source code of ```alphaInitializerFoam``` is developed and maintained for the latest OpenFOAM-plus release (currently ```OpenFOAM-v1812```). A script for generating code for other releases and old versions will be provided in the near future.

Please notify me via the email address below if you found any errors or bugs, and I will fix them as soon as possible.


---

## Installation



---

## Contributors

* Dezhi Dai, UT Arlington, dezhi.dai@mavs.uta.edu (developer)