#ifndef _AABB_RASTERIZER_H_
#define _AABB_RASTERIZER_H_

#include "Camera.h"
#include "AxisAlignedBoundingBox.h"
#include <stdint.h>
#define Z_MAX 10000
#define RASTER_EPS 0.00001

typedef float   RTMatrix[4][4];

void MultMatrix(RTMatrix &out, RTMatrix &m1, RTMatrix &m2)
{
	out[0][0] = m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0] + m1[0][2] * m2[2][0] + m1[0][3] * m2[3][0];
	out[0][1] = m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1] + m1[0][2] * m2[2][1] + m1[0][3] * m2[3][1];
	out[0][2] = m1[0][0] * m2[0][2] + m1[0][1] * m2[1][2] + m1[0][2] * m2[2][2] + m1[0][3] * m2[3][2];
	out[0][3] = m1[0][0] * m2[0][3] + m1[0][1] * m2[1][3] + m1[0][2] * m2[2][3] + m1[0][3] * m2[3][3];

	out[1][0] = m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0] + m1[1][2] * m2[2][0] + m1[1][3] * m2[3][0];
	out[1][1] = m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1] + m1[1][2] * m2[2][1] + m1[1][3] * m2[3][1];
	out[1][2] = m1[1][0] * m2[0][2] + m1[1][1] * m2[1][2] + m1[1][2] * m2[2][2] + m1[1][3] * m2[3][2];
	out[1][3] = m1[1][0] * m2[0][3] + m1[1][1] * m2[1][3] + m1[1][2] * m2[2][3] + m1[1][3] * m2[3][3];

	out[2][0] = m1[2][0] * m2[0][0] + m1[2][1] * m2[1][0] + m1[2][2] * m2[2][0] + m1[2][3] * m2[3][0];
	out[2][1] = m1[2][0] * m2[0][1] + m1[2][1] * m2[1][1] + m1[2][2] * m2[2][1] + m1[2][3] * m2[3][1];
	out[2][2] = m1[2][0] * m2[0][2] + m1[2][1] * m2[1][2] + m1[2][2] * m2[2][2] + m1[2][3] * m2[3][2];
	out[2][3] = m1[2][0] * m2[0][3] + m1[2][1] * m2[1][3] + m1[2][2] * m2[2][3] + m1[2][3] * m2[3][3];

	out[3][0] = m1[3][0] * m2[0][0] + m1[3][1] * m2[1][0] + m1[3][2] * m2[2][0] + m1[3][3] * m2[3][0];
	out[3][1] = m1[3][0] * m2[0][1] + m1[3][1] * m2[1][1] + m1[3][2] * m2[2][1] + m1[3][3] * m2[3][1];
	out[3][2] = m1[3][0] * m2[0][2] + m1[3][1] * m2[1][2] + m1[3][2] * m2[2][2] + m1[3][3] * m2[3][2];
	out[3][3] = m1[3][0] * m2[0][3] + m1[3][1] * m2[1][3] + m1[3][2] * m2[2][3] + m1[3][3] * m2[3][3];
}

void Mult_MatVec(Vector3 &out, RTMatrix &m, const Vector3 &c)
{
	out.v[0] = m[0][0] * c.v[0] + m[0][1] * c.v[1] + m[0][2] * c.v[2] + m[0][3];
	out.v[1] = m[1][0] * c.v[0] + m[1][1] * c.v[1] + m[1][2] * c.v[2] + m[1][3];
	out.v[2] = m[2][0] * c.v[0] + m[2][1] * c.v[1] + m[2][2] * c.v[2] + m[2][3];
	float scale = m[3][0] * c.v[0] + m[3][1] * c.v[1] + m[3][2] * c.v[2] + m[3][3];
	out.v[0] /= scale;
	out.v[1] /= scale;
	out.v[2] /= scale;
}

class Rasterizer
{
public:
	RTMatrix transform;
	RTMatrix Xsi;
	float *zbuffer;
	uint8_t *frameBuffer;
	int width, height;
	Vector3 n;
	Vector3 camera_pos;

	Rasterizer(Camera &camera, float* _zbuffer, uint8_t *out, int _width, int _height)
	{
		float xres = camera.xres;
		float yres = camera.yres;
		width = _width;
		height = _height;

		zbuffer = _zbuffer;
		frameBuffer = out;

		camera_pos = camera.position;
		// screen space transform
		// TODO:fix sp
		RTMatrix Xsp;
		Xsp[0][0] = (float)width/xres; Xsp[0][1] = 0.0f;  Xsp[0][2] = 0.0f; Xsp[0][3] = width * 0.5;
		Xsp[1][0] = 0.0f; Xsp[1][1] = -(float)height/yres; Xsp[1][2] = 0.0f; Xsp[1][3] = height * 0.5;
		Xsp[2][0] = 0.0f; Xsp[2][1] = 0.0f;  Xsp[2][2] = 1.0f; Xsp[2][3] = 0.0f;
		Xsp[3][0] = 0.0f; Xsp[3][1] = 0.0f;  Xsp[3][2] = 0.0f; Xsp[3][3] = 1.0f;

		// maps z perspective value from 0 to -ZMAX
		float a = (float)(Z_MAX * Z_MAX) / ((float)Z_MAX - camera.focusdistance);
		float b = a * camera.focusdistance;
		RTMatrix Xpi = { {camera.focusdistance, 0.0                 , 0.0 , 0.0},
						 {0.0,                  camera.focusdistance, 0.0 , 0.0},
						 {0.0,					0.0,	                a ,   b},
						 {0.0,                  0.0,                 -1.0 , 0.0} }; // perspective transform


		n = camera.forward;
		n.normalize();
		Vector3 u = camera.up;
		u.normalize();
		Vector3 r = camera.right;
		r.normalize();

		RTMatrix Xiw = { {r.x(), r.y(), r.z(), -Vector3::dot(r,camera.position)},
						 {u.x(), u.y(), u.z(), -Vector3::dot(u,camera.position)},
						 {n.x(), n.y(), n.z(), -Vector3::dot(n,camera.position)},
						 {0, 0, 0, 1 } };

		//RTMatrix Xpw;
		//MultMatrix(Xpw, Xpi, Xiw);
		MultMatrix(Xsi, Xsp, Xpi);
		MultMatrix(transform, Xsi, Xiw);

	}

	void RasterizeBox(AABB box, const Vector3 *pixeColor)
	{
		// get all edges
		Vector3 l_min = box.m_min;
		Vector3 l_max = box.m_max;

		Vector3 furthest_point = l_min;
		if (n.x() > 0)
		{
			furthest_point.v[0] = l_max.v[0];
		}

		if (n.y() > 0)
		{
			furthest_point.v[1] = l_max.v[1];
		}

		if (n.z() > 0)
		{
			furthest_point.v[2] = l_max.v[2];
		}

		if (Vector3::dot(furthest_point - camera_pos, n) > 0)
		{
			// point is behind
			return;
		}

		RasterizeLine(Vector3(l_min.x(), l_min.y(), l_min.z()), Vector3(l_max.x(), l_min.y(), l_min.z()), pixeColor);
		RasterizeLine(Vector3(l_min.x(), l_min.y(), l_min.z()), Vector3(l_min.x(), l_max.y(), l_min.z()), pixeColor);
		RasterizeLine(Vector3(l_min.x(), l_min.y(), l_min.z()), Vector3(l_min.x(), l_min.y(), l_max.z()), pixeColor);

		RasterizeLine(Vector3(l_min.x(), l_min.y(), l_max.z()), Vector3(l_min.x(), l_max.y(), l_max.z()), pixeColor);
		RasterizeLine(Vector3(l_min.x(), l_min.y(), l_max.z()), Vector3(l_max.x(), l_min.y(), l_max.z()), pixeColor);

		RasterizeLine(Vector3(l_min.x(), l_max.y(), l_min.z()), Vector3(l_max.x(), l_max.y(), l_min.z()), pixeColor);
		RasterizeLine(Vector3(l_min.x(), l_max.y(), l_min.z()), Vector3(l_min.x(), l_max.y(), l_max.z()), pixeColor);

		RasterizeLine(Vector3(l_max.x(), l_min.y(), l_min.z()), Vector3(l_max.x(), l_max.y(), l_min.z()), pixeColor);
		RasterizeLine(Vector3(l_max.x(), l_min.y(), l_min.z()), Vector3(l_max.x(), l_min.y(), l_max.z()), pixeColor);

		RasterizeLine(Vector3(l_max.x(), l_max.y(), l_max.z()), Vector3(l_max.x(), l_max.y(), l_min.z()), pixeColor);
		RasterizeLine(Vector3(l_max.x(), l_max.y(), l_max.z()), Vector3(l_max.x(), l_min.y(), l_max.z()), pixeColor);
		RasterizeLine(Vector3(l_max.x(), l_max.y(), l_max.z()), Vector3(l_min.x(), l_max.y(), l_max.z()), pixeColor);
	}

	void RasterizeLine(const Vector3 &a3d, const Vector3 &b3d, const Vector3 *pixelColor)
	{
		Vector3 a;
		Mult_MatVec(a, transform, a3d);
		Vector3 b;
		Mult_MatVec(b, transform, b3d);

		// choose to sample x or y 
		float dy = fabs(a.v[1] - b.v[1]);
		float dx = fabs(a.v[0] - b.v[0]);

		Vector3 *pa, *pb;
		if (dx > dy)
		{
			// sample x
			if (a.v[0] < b.v[0])
			{
				pa = &a;
				pb = &b;
			}
			else
			{
				pa = &b;
				pb = &a;
			}
			// left to right
			int x0 = MAX((int) ceilf(pa->v[0]), 0); // Start point 
			float dydx = (pb->v[1] - pa->v[1]) / (pb->v[0] - pa->v[0]);
			float dzdx = (pb->v[2] - pa->v[2]) / (pb->v[0] - pa->v[0]);

			for (int xi = x0; xi <= MIN((int)pb->v[0], width - 1); xi++)
			{
				float yf = dydx * ((float)xi - pa->v[0]) + pa->v[1];
				int yi = (int)yf;
				if (yf - (float)yi > 0.5) yi++;

				float z = dzdx * ((float)xi - pa->v[0]) + pa->v[2];

				if (yi > 0 && yi < height && z < 0 && z > zbuffer[yi * width + xi])
				{
					int r = pixelColor->r() * 255.99f;
					int g = pixelColor->g() * 255.99f;
					int b = pixelColor->b() * 255.99f;

					frameBuffer[3 * (yi*width + xi)] = r;
					frameBuffer[3 * (yi*width + xi) + 1] = g;
					frameBuffer[3 * (yi*width + xi) + 2] = b;
					zbuffer[yi * width + xi] = z;
				}
			}
		}
		else
		{
			// sample y
			if (a.v[1] < b.v[1])
			{
				pa = &a;
				pb = &b;
			}
			else
			{
				pa = &b;
				pb = &a;
			}
			// top to bottom
			// left to right
			int y0 = MAX((int)ceilf(pa->v[1]), 0); // Start point 
			float dxdy = (pb->v[0] - pa->v[0]) / (pb->v[1] - pa->v[1]);
			float dzdy = (pb->v[2] - pa->v[2]) / (pb->v[1] - pa->v[1]);
			for (int yi = y0; yi <= MIN((int)pb->v[1], height - 1); yi++)
			{
				float xf = dxdy * ((float)yi - pa->v[1]) + pa->v[0];
				int xi = (int)xf;
				if (xf - (float)xi > 0.5) xi++;

				float z = dzdy * ((float)yi - pa->v[1]) + pa->v[2];

				if (xi > 0 && xi < width && z < 0 && z > zbuffer[yi * width + xi])
				{
					int r = pixelColor->r() * 255.99f;
					int g = pixelColor->g() * 255.99f;
					int b = pixelColor->b() * 255.99f;

					frameBuffer[3 * (yi*width + xi)] = r;
					frameBuffer[3 * (yi*width + xi) + 1] = g;
					frameBuffer[3 * (yi*width + xi) + 2] = b;
					zbuffer[yi * width + xi] = z;
				}
			}
		}
	}

	void RasterizeLine2(const Vector3 &a3d, const Vector3 &b3d, const Vector3 *pixelColor)
	{
		Vector3 ra;
		Mult_MatVec(ra, transform, a3d);
		Vector3 rb;
		Mult_MatVec(rb, transform, b3d);

		Vector3 *a = &ra;
		Vector3 *b = &rb;
		if (b->y() < a->y())
		{
			Vector3* tmp = a;
			a = b;
			b = tmp;
		}

		float dy = (b->y() - a->y());
		float dx = (b->x() - a->x());
		if (dy > RASTER_EPS)
		{
			float dxdy = dx / dy;
			float dzdy = (b->z() - a->z()) / dy;
			for (int y = MAX((int)a->y(), 0); y <= MIN((int)b->y(), height - 1); y++)
			{
				float x = dxdy * ((float)y - a->y()) + a->x();
				int xi = (int)x;
				if (x - (float)xi > 0.5f)
				{
					xi++;
				}
				float z = dzdy * ((float)y - a->y()) + a->z();
				if (xi > 0 && xi < width && z < 0 && z > zbuffer[y * width + xi])
				{
					int r = 0;//pixelColor->r() * 255.99f;
					int g = 0;//pixelColor->g() * 255.99f;
					int b = 255.99f;// pixelColor->b() * 255.99f;

					frameBuffer[3 * (y*width + xi)] = r;
					frameBuffer[3 * (y*width + xi) + 1] = g;
					frameBuffer[3 * (y*width + xi) + 2] = b;
					zbuffer[y * width + xi] = z;
				}
				//else
				//{
				//	int dbg = 0;
				//}
			}
		}
		else
		{
			if (dx < 0)
			{
				Vector3* tmp = a;
				a = b;
				b = tmp;
				dx = -dx;
				dy = -dy;
			}
			if (dx > RASTER_EPS)
			{
				float dydx = dy / dx;
				float dzdx = (b->z() - a->z()) / dx;
				for (int x = MAX((int)a->x(), 0); x <= MIN((int)b->x(), width - 1); x++)
				{
					float y = dydx * ((float)x - a->x()) + a->y();
					int yi = (int)y;
					if (y - (float)yi > 0.5f)
					{
						yi++;
					}
					float z = dzdx * ((float)x - a->x()) + a->z();
					if (yi > 0 && yi <= height - 1 && z < 0 && z > zbuffer[yi * width + x])
					{
						int r = 255.99f;//pixelColor->r() * 255.99f;
						int g = 0;//pixelColor->g() * 255.99f;
						int b = 0;// pixelColor->b() * 255.99f;

						frameBuffer[3 * (yi*width + x)] = r;
						frameBuffer[3 * (yi*width + x) + 1] = g;
						frameBuffer[3 * (yi*width + x) + 2] = b;
						zbuffer[yi * width + x] = z;
					}
					//else
					//{
					//	int dbg = 0;
					//}
				}
			}
			//else
			//{
			//	int dbg = 0;
			//}
		}
	}

	float trasnformHitPoint(Vector3 &point)
	{
		float z = Vector3::dot(point, Vector3(transform[2][0], transform[2][1], transform[2][2])) + transform[2][3];
		z /= Vector3::dot(point, Vector3(transform[3][0], transform[3][1], transform[3][2])) + transform[3][3];
		return z;
	}
};

// need better colour function
Vector3 getColorBasic(int i, int max_val)
{

	float ratio = 2 * (float) i/(float) max_val;
	float b = fmax(0, 1 - ratio);
	float r = fmax(0, ratio - 1);
	float g = 1 - b - r;
	std::cout << "Level: " << i << ", Max_depth: " << max_val << ", Colour: " << r << "," << g << "," << b << std::endl;
	return Vector3(r, g, b);
}


// VIBGYOR
const Vector3 colorRange[] = { Vector3(148,0,211) / 255.0f,
							   Vector3(75,0,130) / 255.0f,
							   Vector3(0,0,255) / 255.0f,
							   Vector3(0,255,0) / 255.0f,
							   Vector3(255,255,0) / 255.0f,
							   Vector3(255,127,0) / 255.0f,
							   Vector3(255,0,0) / 255.0f };


const int numColours = sizeof(colorRange) / sizeof(Vector3);

// need better colour function
Vector3 getColor(int i, int max_val)
{
	float float_index = ((float)i / (float)max_val);
	float_index *= (numColours - 1);
	int ind = (int)float_index;
	float f = float_index - (float)ind;
	if (f < FLT_EPSILON)
	{
		//std::cout << "Level: " << i << ", Max_depth: " << max_val << ", Colour: " << colorRange[ind].r();
		//std::cout << "," << colorRange[ind].g() << "," << colorRange[ind].b() << std::endl;
		return colorRange[ind];
	}
	// interpolate
	Vector3 chosenColor = Vector3::lerp(colorRange[ind], colorRange[ind + 1], f);
	//std::cout << "Level: " << i << ", Max_depth: " << max_val << ", Colour: " << chosenColor.r();
	//std::cout << "," << chosenColor.g() << "," << chosenColor.b() << std::endl;
	return chosenColor;
}


#endif
