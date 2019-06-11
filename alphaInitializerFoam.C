/*---------------------------------------------------------------------------*\
|             alphaInitializerFoam | Copyright (C) 2019 Dezhi Dai             |
-------------------------------------------------------------------------------
    Cork   | Copyright (C) 2016 Gilbert Bernstein
    libigl | Copyright (C) 2018 Alec Jacobson, Daniele Panozzo and others.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    alphaInitializerFoam

Description
    Initialize the alpha (VOF) field with an input shape surface mesh
    in .stl format. The previous fluid shape will be retained.

    Reference:
        \verbatim
            Dai, Dezhi (2019).
            alphaInitializerFoam
            Mendeley Data, V1
            doi 10.17632/wg4sx7sc57.1
            url http://dx.doi.org/10.17632/wg4sx7sc57.1
        \endverbatim

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "igl/readSTL.h"
#include "igl/remove_duplicate_vertices.h"
#include "igl/copyleft/cork/mesh_boolean.h"
#include "igl/readSTL.h"
#include "igl/volume.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Initialize the alpha (VOF) field with an input shape surface mesh "
        "in .stl format. The previous fluid shape will be retained.\n"
    );

    // Disable and add some options
    argList::noParallel();          // Disable parallel function
    argList::noFunctionObjects();   // Don't use function objects
    argList::addArgument("stl-file", "The input surface mesh in .stl format");
    Foam::argList::addBoolOption
    (
        "gas",
        "Initialize the shape for gas phase"
    );


    #include "setRootCase.H"
    #include "createTime.H"
    #include "createDynamicFvMesh.H"


    Info << "\nReading field alpha.water\n" << endl;
    volScalarField alpha1
    (
        IOobject
        (
            "alpha.water",
            "0",
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        ),
        mesh
    );


    Info << "Reading shape file " << args[1] << "\n" << endl;
    Eigen::MatrixXd temp_V, V, N;
    Eigen::MatrixXi temp_F, VI, VJ, F;
    bool success = igl::readSTL(args[1], temp_V, temp_F, N);
    if (success)
    {
        igl::remove_duplicate_vertices(temp_V, temp_F, 1e-6, V, VI, VJ, F);
    }


    Info << "Initializing field alpha.water\n" << endl;
    const scalarField& cellVolumes = mesh.V();
    const cellList& cellFaces = mesh.cells();
    const faceList& faces = mesh.faces();
    const pointField& points = mesh.points();
    const labelList& own = mesh.faceOwner();

    forAll(alpha1, cellI)
    {
        scalar fluidVol(0.0);

        // Localize cell point labels
        const cell& currentCell(cellFaces[cellI]);
        const labelList& globalCellPtLabels(currentCell.labels(faces));
        pointField localPts;
        forAll(globalCellPtLabels, pointI)
        {
            localPts.append(points[globalCellPtLabels[pointI]]);
        }


        // Localize cell face labels
        labelListList localFaces;
        forAll(currentCell, faceI)
        {
            const face& fa = faces[currentCell[faceI]];

            localPts.append(fa.centre(points));

            edgeList edges = fa.edges();

            forAll(edges, edgeI)
            {
                labelList localTriface;
                label c0 = localPts.size() - 1;
                localTriface.append(c0);
                label c1 = edges[edgeI].start();
                label c2 = edges[edgeI].end();

                forAll(globalCellPtLabels, pointI)
                {
                    if(c1 == globalCellPtLabels[pointI])
                    {
                        c1 = pointI;
                        break;
                    }
                }

                forAll(globalCellPtLabels, pointI)
                {
                    if(c2 == globalCellPtLabels[pointI])
                    {
                        c2 = pointI;
                        break;
                    }
                }

                if(cellI == own[currentCell[faceI]])
                {
                    localTriface.append(c1);
                    localTriface.append(c2);
                }
                else
                {
                    localTriface.append(c2);
                    localTriface.append(c1);
                }

                localFaces.append(localTriface);
            }
        }

        Eigen::MatrixXd VC(localPts.size(), 3);
        forAll(localPts, pointI)
        {
            VC(pointI, 0) = localPts[pointI].x();
            VC(pointI, 1) = localPts[pointI].y();
            VC(pointI, 2) = localPts[pointI].z();
        }

        Eigen::MatrixXi FC(localFaces.size(), 3);
        forAll(localFaces, faceI)
        {
            const labelList& fa(localFaces[faceI]);
            FC(faceI, 0) = fa[0];
            FC(faceI, 1) = fa[1];
            FC(faceI, 2) = fa[2];
        }


        // Calculate intersection of the shape surface mesh and cellI
        Eigen::MatrixXd V_bool;
        Eigen::MatrixXi F_bool;

        igl::copyleft::cork::mesh_boolean
        (
            VC, FC, V, F, igl::MESH_BOOLEAN_TYPE_INTERSECT, V_bool, F_bool
        );


        // Calculate intersected volume
        if (F_bool.rows() > 0)
        {
            Eigen::MatrixXd V2(V_bool.rows() + 1, V_bool.cols());
            V2.topRows(V_bool.rows()) = V_bool;
            V2.bottomRows(1).setZero();
            Eigen::MatrixXi T(F_bool.rows(), 4);
            T.leftCols(3) = F_bool;
            T.rightCols(1).setConstant(V_bool.rows());
            Eigen::VectorXd vol;
            igl::volume(V2, T, vol);

            fluidVol = mag(vol.sum());

            scalar alphaFluid(fluidVol / cellVolumes[cellI]);
            alphaFluid = min(scalar(1.0), max(scalar(0.0), alphaFluid));

            if (args.found("gas"))
            {
                alpha1.ref()[cellI] = 1.0 - alphaFluid;
            }
            else
            {
                alpha1.ref()[cellI] = alphaFluid;
            }
        }
    }


    Info << "Writing field alpha.water\n" << endl;
    alpha1.write();

    return 0;
}