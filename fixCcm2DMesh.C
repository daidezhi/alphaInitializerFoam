/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
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
    fixCcm2DMesh

Description
    Fix the STAR-CCM 2D mesh before extruding in z direction.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "addOverwriteOption.H"
    #include "addTimeOptions.H"
    
    #include "setRootCase.H"

    //- Create object of class Time: runTime
    #include "createTime.H"

    
    #include "createNamedMesh.H"
    const word oldInstance("constant");
    
    const bool overwrite = args.optionFound("overwrite");
    
	pointField points(mesh.points());
    faceList faces(mesh.faces());
    cellList cells(mesh.cells());

	labelList fo(mesh.faceOwner());
	labelList fn(mesh.faceNeighbour());
	
	forAll(fo, faceI)
	{
	    vector v1(points[faces[faceI][1]][1]-points[faces[faceI][0]][1], points[faces[faceI][0]][0]-points[faces[faceI][1]][0], 0);
	    
	    scalar s1(0);
	    
	    if(faceI < fn.size())
	    {
	        s1 = v1 & (mesh.C()[fn[faceI]] - mesh.C()[fo[faceI]]);
	    }
	    else
	    {
	        s1 = v1 & (mesh.Cf()[faceI] - mesh.C()[fo[faceI]]);
	    }
	    
	    if(s1 < 0)
	    {
	        label l1(faces[faceI][0]);
	        faces[faceI][0] = faces[faceI][1];
	        faces[faceI][1] = l1;
	    }
	}
	
    
    const polyBoundaryMesh& patches = mesh.boundaryMesh();
    labelList patchSizes(patches.size());
    labelList patchStarts(patches.size());
    
    forAll(patches, patchi)
    {
        patchSizes[patchi] = patches[patchi].size();
        patchStarts[patchi] = patches[patchi].start();
    }
    
    mesh.resetPrimitives
    (
        xferMove(points),
        xferMove(faces),
        xferMove(fo),
        xferMove(fn),
        patchSizes,
        patchStarts,
        true
    );
    
    
    if (!overwrite)
    {
        runTime++;
    }
    
    if (overwrite)
    {
        mesh.setInstance(oldInstance);
    }
    else
    {
        mesh.setInstance(runTime.timeName());
    }
    
    Info<< "Writing mesh to " << mesh.facesInstance() << endl;
    
    mesh.write();


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    return 0;
}

// ************************************************************************* //
