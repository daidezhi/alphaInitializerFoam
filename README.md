# alphaInitializerFoam

---

:warning: **IMPORTANT** :warning: The `alphaInitializerFoam` has been renamed as **`setVofField`** and is available at [geometricVofExt](https://github.com/daidezhi/geometricVofExt).

---

## Introduction

```alphaInitializerFoam``` is a pre-processing utility designed to initialize the alpha (VOF) field with an input shape surface mesh in .stl format. It works on polyhedral unstructured meshes. The previous fluid shape will be retained.

Another copy of alphaInitializerFoam and demo cases can be found in:

Dai, Dezhi (2019), “alphaInitializerFoam”, Mendeley Data, v2

http://dx.doi.org/10.17632/wg4sx7sc57.2


## Compatibility

The source code of ```alphaInitializerFoam``` is developed and maintained for the latest OpenFOAM-plus release (currently ```OpenFOAM-v1812```). A script for generating code for other releases and old versions will be provided in the near future.

*Please notify me via the email address below if you found any errors or bugs, and I will fix them as soon as possible.*


## Methodology

The basic idea of ```alphaInitializerFoam``` follows:

* first compute the intersection of the surface mesh (*blue*), which represents the liquid or gas shape, and each cell (*red*) in the mesh,

<img src="https://upload.wikimedia.org/wikipedia/commons/4/4a/Boolean_union.PNG" width="200"><img src="https://upload.wikimedia.org/wikipedia/commons/0/0b/Boolean_intersect.PNG" width="200">*Figures are from [Wikipedia](https://en.wikipedia.org/wiki/Constructive_solid_geometry).*

* then initialize the fraction value by normalizing the intersection volume with the cell one.

The .stl surface mesh loading and intersection operation in ```alphaInitializerFoam``` are from [libigl](https://libigl.github.io/) and [Cork](https://github.com/gilbo/cork) libs, respectively.

*It should be noted that the copies of [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page), [libigl](https://libigl.github.io/) and [Cork](https://github.com/gilbo/cork) that provided together with ```alphaInitializerFoam``` are **ONLY** used for simplifying the installation.*

## Installation

**Before compiling ```alphaInitializerFoam```, make sure that the OpenFOAM environment has been set properly.**

1. Download ```alphaInitializerFoam``` from this page or http://dx.doi.org/10.17632/wg4sx7sc57.2

2. Build ```libcork.a```
```bash
cd cork
make
```

3. Build ```alphaInitializerFoam```
```bash
cd ..
wmake
```

*All of the compiling commands above have been integrated into ```Allwmake``` script.*

## Usage

The usage of ```alphaInitializerFoam``` is simple:

```
Usage: alphaInitializerFoam [OPTIONS] <stl-file>
Arguments:
  <stl-file>        The input surface mesh in .stl format
Options:
  -case <dir>       Specify case directory to use (instead of the cwd)
  -gas              Initialize the shape for gas phase
  -doc              Display documentation in browser
  -doc-source       Display source code in browser
  -help             Display short help and exit
  -help-compat      Display compatibility options and exit
  -help-full        Display full help and exit

Initialize the alpha (VOF) field with an input shape surface mesh in .stl
format. The previous fluid shape will be retained.

Using: OpenFOAM-v1812 (1812) (see www.OpenFOAM.com)
Build: v1812
Arch:  LSB;label=32;scalar=64
```

**It should be noted that the parallel option has been disabled.**


## Demos

Three demo cases are available in http://dx.doi.org/10.17632/wg4sx7sc57.2 (```alphaInitializerFoamTut.tar.gz```).

**The commands are integrated into the ```Allrun``` script for each case.**

### ```damBreak```

<img src="https://i.imgur.com/xQNh65O.png">

<img src="https://i.imgur.com/B8hqsuZ.png">

<img src="https://i.imgur.com/qhpNmFU.png">


### ```dualPiBubble```

<img src="https://i.imgur.com/utTNzC1.png">


### ```dualPiDroplet```

<img src="https://i.imgur.com/Dmr2J6h.png">


## Attribution

If you use ```alphaInitializerFoam``` in your academic projects, please cite this utility in general, you could use this BibTex entry:

```css
@misc{alphaInitializerFoam,
  doi = {10.17632/wg4sx7sc57.2},
  url = {http://dx.doi.org/10.17632/wg4sx7sc57.2},
  author = {Dai, Dezhi},
  title = {alphaInitializerFoam},
  publisher = {Mendeley Data, v2},
  year = {2019}
}
```


## Change Log

### 06/20/2019

* Add support to wave model boundaries. Thanks for [guohuiqun](http://cfd-china.com/user/guohuiqun) @ [cfd-china](http://cfd-china.com/).

### 06/11/2019

* Initial release.


## Contributors

* Dezhi Dai, UT Arlington, dezhi.dai@mavs.uta.edu (Developer)
