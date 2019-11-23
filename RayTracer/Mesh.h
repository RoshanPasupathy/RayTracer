#ifndef _MESH_H_
#define _MESH_H_

#include <stdio.h>
#include "Vector3.h"
#include "Triangle.h"

bool ReadMesh(Hittable** shapeList, const char* meshfilename, int &numShapes)
{
	FILE *meshfile;
	if (fopen_s(&meshfile, meshfilename, "r") != 0 )
	{
		return false;
	}
	char dummys[256];
	Vector3 pos[3], norms[3];
	float dummy;
	while (fscanf_s(meshfile, "%s", dummys, 256) == 1) { 	/* read in tri word */
		// vertex 0
		fscanf_s(meshfile, "%f %f %f %f %f %f %f %f",
			&(pos[0].v[0]), &(pos[0].v[1]), &(pos[0].v[2]),
			&(norms[0].v[0]), &(norms[0].v[1]), &(norms[0].v[2]),
			&dummy, &dummy);
		// vertex 1
		fscanf_s(meshfile, "%f %f %f %f %f %f %f %f",
			&(pos[1].v[0]), &(pos[1].v[1]), &(pos[1].v[2]),
			&(norms[1].v[0]), &(norms[1].v[1]), &(norms[1].v[2]),
			&dummy, &dummy);
		// vertex 0
		fscanf_s(meshfile, "%f %f %f %f %f %f %f %f",
			&(pos[2].v[0]), &(pos[2].v[1]), &(pos[2].v[2]),
			&(norms[2].v[0]), &(norms[2].v[1]), &(norms[2].v[2]),
			&dummy, &dummy);
		shapeList[numShapes++] = new Triangle(pos, norms, new Lambertian(Vector3(.1, .8, .1))); //new Metal(Vector3(0.7, 0.6, 0.5), 0.0)
	}
	return true;
}

#endif // !_MESH_H_

