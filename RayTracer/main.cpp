#include <iostream>
#include "stb_image_write.h"

#include "Vector3.h"
#include "Random.h"
#include "Ray.h"
#include "Hittable.h"
#include "Sphere.h"
#include "Triangle.h"
#include <omp.h>
#include <time.h>
#include "Camera.h"
#include "AgglomerativeClustering.h"
#include "BoundingVolumeHeirachy.h"
#include "MortonCode.h"
#include "Mesh.h"
#include "RTCommonDefines.h"
#include "RTInput.h"

#define WIDTH 480	
#define HEIGHT 480

#define AA_SAMPLES 128
#define AMBIENT_LIGHT (Vector3(0.5,0.5,0.5))

const int BUFFER_SIZE = 3 * WIDTH * HEIGHT;
uint8_t pixelBuffer[BUFFER_SIZE];

Hittable* shapeList[MAX_SHAPES];
int numShapes = 0;

//camera variables

Camera camera;// = Camera(position, lookAt, 40, WIDTH / HEIGHT, aperture, focusDistance);

Vector3 color(Ray& ray, Vector3 &hit_point) {
	HitInfo hitInfo;
	Vector3 c(1, 1, 1);
	Vector3 total_emitted = Vector3(0, 0, 0);
	Ray scatteredRay;

	if (hitList(shapeList, numShapes, ray, .001f, FLT_MAX, hitInfo)) {

		Vector3 attenuation;
		Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
		hit_point = hitInfo.position;
		if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
			//return attenuation * color(scatteredRay);
			total_emitted += c * emitted; // multiply by previous attentuation
			c *= attenuation;
			ray = scatteredRay;
		}
		else {
			return total_emitted + c * emitted; // hit a light source
		}
	}
	else {
		//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
		return total_emitted + c * AMBIENT_LIGHT;
	}

	for (int i = 1; i < 4; i++)
	{
		if (hitList(shapeList, numShapes, ray, .001f, FLT_MAX, hitInfo)) {

			Vector3 attenuation;
			Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
			if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
				//return attenuation * color(scatteredRay);
				total_emitted += c * emitted; // multiply by previous attentuation
				c *= attenuation;
				ray = scatteredRay;
			}
			else {
				return total_emitted + c * emitted; // hit a light source
			}
		}
		else {
			//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
			return total_emitted + c * AMBIENT_LIGHT;
		}
	}
	return Vector3(0, 0, 0);
}

Vector3 bvh_color(Ray& ray, Hittable* bvh_root, Vector3 &hit_point) {
	HitInfo hitInfo;
	Vector3 c(1, 1, 1);
	Vector3 total_emitted = Vector3(0, 0, 0);
	Ray scatteredRay;

	if (bvh_hit(bvh_root, numShapes, ray, .001f, FLT_MAX, hitInfo)) {

		Vector3 attenuation;
		Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
		hit_point = hitInfo.position;
		if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
			//return attenuation * color(scatteredRay);
			total_emitted += c * emitted; // multiply by previous attentuation
			c *= attenuation;
			ray = scatteredRay;
		}
		else {
			return total_emitted + c * emitted; // hit a light source
		}
	}
	else {
		//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
		return total_emitted + c * AMBIENT_LIGHT;
	}


	for (int i = 1; i < 4; i++)
	{
		if (bvh_hit(bvh_root, numShapes, ray, .001f, FLT_MAX, hitInfo)){

			Vector3 attenuation;
			Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
			if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
				//return attenuation * color(scatteredRay);
				total_emitted += c * emitted; // multiply by previous attentuation
				c *= attenuation;
				ray = scatteredRay;
			}
			else {
				return total_emitted + c * emitted; // hit a light source
			}
		}
		else {
			//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
			return total_emitted + c * AMBIENT_LIGHT;
		}
	}
	return Vector3(0, 0, 0);
}


// agglomerated
Vector3 ac_bvh_color(Ray& ray, Hittable* bvh_root, Vector3 &hit_point) {
	HitInfo hitInfo;
	Vector3 c(1, 1, 1);
	Vector3 total_emitted = Vector3(0, 0, 0);
	Ray scatteredRay;

	if (AC_bvh_hit(bvh_root, ray, .001f, FLT_MAX, hitInfo)) {

		Vector3 attenuation;
		Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
		hit_point = hitInfo.position;
		if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
			//return attenuation * color(scatteredRay);
			total_emitted += c * emitted; // multiply by previous attentuation
			c *= attenuation;
			ray = scatteredRay;
		}
		else {
			return total_emitted + c * emitted; // hit a light source
		}
	}
	else {
		//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
		return total_emitted + c * AMBIENT_LIGHT;
	}


	for (int i = 1; i < 4; i++)
	{
		if (AC_bvh_hit(bvh_root, ray, .001f, FLT_MAX, hitInfo)) {

			Vector3 attenuation;
			Vector3 emitted = hitInfo.pMaterial->emitt(0, 0, Vector3(0, 0, 0));
			if (hitInfo.pMaterial->scatter(ray, hitInfo, attenuation, scatteredRay)) {
				//return attenuation * color(scatteredRay);
				total_emitted += c * emitted; // multiply by previous attentuation
				c *= attenuation;
				ray = scatteredRay;
			}
			else {
				return total_emitted + c * emitted; // hit a light source
			}
		}
		else {
			//return c * Vector3::lerp(Vector3(1, 1, 1), Vector3(.5, .7, 1), .5f * (Vector3::normalized(ray.direction).y() + 1.0f));
			return total_emitted + c * AMBIENT_LIGHT;
		}
	}
	return Vector3(0, 0, 0);
}


#define RND (random())

void addObjects(Hittable** d_list)
{
	int i = 0;
	//d_list[i++] = new Sphere(Vector3(0, -1000.0, -1), 1000,
	//	new Lambertian(Vector3(.8, .8, 0)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = RND;
			Vector3 center(a + RND, 0.2, b + RND);
			if (choose_mat < 0.8f) {
				d_list[i++] = new Sphere(center, 0.2,
					new Lambertian(Vector3(RND*RND, RND*RND, RND*RND)));
			}
			else if (choose_mat < 0.95f) {
				d_list[i++] = new Sphere(center, 0.2,
					new Metal(Vector3(0.5f*(1.0f + RND), 0.5f*(1.0f + RND), 0.5f*(1.0f + RND)), 0.5f*RND));
			}
			else {
				d_list[i++] = new Sphere(center, 0.2, new Dialectric(1.5));
			}
		}
	}
	d_list[i++] = new Sphere(Vector3(0, 1, 0), 1.0, new Dialectric(1.5));
	d_list[i++] = new Sphere(Vector3(-4, 1, 0), 1.0, new Lambertian(Vector3(0.4, 0.2, 0.1)));
	d_list[i++] = new Sphere(Vector3(4, 1, 0), 1.0, new Metal(Vector3(0.7, 0.6, 0.5), 0.0));
	d_list[i++] = new Sphere(Vector3(100.0f, 100.0f, -10.f), 100.0f, new Diffuse_Light(Vector3(1, 1, 1)));
	numShapes = i;

	Vector3 position = Vector3(19.5, 2, 4.5);
	Vector3 lookAt = Vector3(0, 0, 0);
	float focusDistance = 2.0;
	float aperture = 0.0001;//0.1f;
	camera = Camera(position, lookAt, 40, WIDTH / HEIGHT, aperture, focusDistance);
}

void basicObjectsAdd()
{
	//ading shapes
	shapeList[numShapes++] = new Sphere(Vector3(0, 0, -1), .5f, new Lambertian(Vector3(.8, .3, .3)));
	shapeList[numShapes++] = new Sphere(Vector3(0, -100.5f, -1), 100, new Lambertian(Vector3(.8, .8, 0)));

	Vector3 triv[3] = { Vector3(0,0,-1), Vector3(1,2, -1), Vector3(2, 0, -1) };
	Vector3 trin[3] = { Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1) };
	shapeList[numShapes++] = new Triangle(triv, trin, new Lambertian(Vector3(.3, .3, .8)));
	shapeList[numShapes++] = new Sphere(Vector3(-1, 0, -1), .5f, new Dialectric(1.5f));

	Vector3 triv2[3] = { Vector3(-2,-.25,-1), Vector3(-1, .75, -1), Vector3(0,-.25, -1) };
	Vector3 trin2[3] = { Vector3(0,0,1), Vector3(0,0,1), Vector3(0,0,1) };
	shapeList[numShapes++] = new Triangle(triv2, trin2, new Lambertian(Vector3(0, 0, .9)));

	shapeList[numShapes++] = new Sphere(Vector3(0, 1.4, -1), .5f, new Diffuse_Light(Vector3(1,1,1)));

	Vector3 position = Vector3(0, 0, 5);
	Vector3 lookAt = Vector3(0, 0, -1);
	float aperture = 0.001f;
	float focusDistance = (position - lookAt).length();
	camera = Camera(position, lookAt, 40, WIDTH / HEIGHT, aperture, focusDistance);
}

bool teapotAdd()
{
	if (!ReadMesh(shapeList, "pot4.asc", numShapes))
	{
		std::cout << "Failed to read mesh!" << std::endl;
		return false;
	}
	shapeList[numShapes++] = new Sphere(Vector3(-10.0f, 10.0f, -20.0f), 5.0f, new Diffuse_Light(Vector3(.3, .3, .3)));
	shapeList[numShapes++] = new Sphere(Vector3(-10, -100.5f, -1), 100, new Lambertian(Vector3(.8, .8, 0)));
	shapeList[numShapes++] = new Sphere(Vector3(-1, 1, -3), 0.5f, new Diffuse_Light(Vector3(1, 1, 1)));// new Metal(Vector3(0.7, 0.6, 0.5), 0.0));//new Lambertian(Vector3(.3, .3, .8)));
	camera = Camera(Vector3(-10, 5, -5), Vector3(0, 0, 0), 35.0, WIDTH / HEIGHT, 0.0001, 1);
	return true;
}


int main(int argc, char *argv[]) {
	srand(1);
	std::cout << "First rand value: " << rand() << std::endl;
	bool use_morton = false;
	clock_t start, stop;

	int numPrimitivesInLeaves = 8;
	int numInterNodes = 2;

	HandleInput(argc, argv, numPrimitivesInLeaves, use_morton, numInterNodes);

	// FOR BOX DEBUGGING
	//shapeList[numShapes++] = new Sphere(Vector3(-1, 0, -1), .5f, new Lambertian(Vector3(.8, .3, .3)));
	//Vector3 position = Vector3(0, 3, 5);
	//Vector3 lookAt = Vector3(0, 0, -1);
	//float aperture = 0.001f;
	//float focusDistance = (position - lookAt).length();
	//camera = Camera(position, lookAt, 40, WIDTH / HEIGHT, aperture, 2);


	//addObjects(shapeList);
	if (!teapotAdd())
	{
		return -1;
	}
	//basicObjectsAdd();

	// Morton code debug
	Hittable_Init(shapeList, numShapes);
	//mcode_print_params();
	//std::cout << _mortoncode(Vector3{ 3,2,1 }) << std::endl;
	// end morton code debug

	float *zbuffer = new float[WIDTH * HEIGHT];
	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		zbuffer[i] = -Z_MAX;
	}
	Rasterizer rt(camera, zbuffer, pixelBuffer, WIDTH, HEIGHT);

	Hittable* bvh_root = NULL;
	std::cout << "\n------------------------------------------------------\n";
	if (g_acctype == RT_BVH)
	{
		std::cout << "Using Bounding Volume Hierarchy. Number of primitives in leaf: " << numPrimitivesInLeaves << std::endl;
		if (use_morton)
			std::cout << "Sorting using morton code " << std::endl;
		
		std::cout << "Starting constructing BVH\n";
		start = clock();
		bvh_root = constructBVH(shapeList, numShapes, numPrimitivesInLeaves, use_morton);
		stop = clock();
		double secs = ((double)(stop - start) / CLOCKS_PER_SEC);
		std::cout << "Finished construction\t\t Took " << secs << " seconds\n" << std::endl;

	}
	else if (g_acctype == RT_AC_BVH)
	{
		std::cout << "Using Agglomerated Bounding Volume Hierarchy. Number of primitives in leaf: " << numPrimitivesInLeaves;
		std::cout << " .Branching Factor: ";
		if (numInterNodes > 0)
		{
			std::cout << numInterNodes << std::endl;
		}
		else
		{
			std::cout << "Dynammic" << std::endl;
		}
		
		std::cout << "Starting constructing AC BVH. This may take a while ...\n";
		start = clock();
		if (numInterNodes > 0)
			bvh_root = AC_constructBVH(shapeList, numShapes, numPrimitivesInLeaves, numInterNodes);
		else
			bvh_root = AC_constructBVH(shapeList, numShapes, numPrimitivesInLeaves);

		stop = clock();
		double secs = ((double)(stop - start) / CLOCKS_PER_SEC);
		std::cout << "Finished construction\t\t Took " << secs << " seconds\n" << std::endl;

	}
	else
	{
		// change materials for comparison
		int dummy = 0;
		//Hittable** tmp = (Hittable **)getLeafNodes(shapeList, numShapes, numPrimitivesInLeaves, dummy, use_morton);
		//delete[] tmp;
	}

#pragma omp parallel
	{
		if (omp_get_thread_num() == 0)
		{
			std::cout << "Number of threads: " << omp_get_num_threads() << std::endl;
		}
	}
	
	start = clock();


	//rendering
	std::cout << "------------------------------------------------------\n";
	std::cout << "Starting rendering of " << numShapes << " objects\n";

	if (g_acctype == RT_BVH)
	{
#pragma omp parallel for schedule(dynamic)
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				Vector3 pixelColor(0, 0, 0);
				Vector3 avg_hit_point = Vector3(0, 0, 0);
				int num_hits = 0;
				for (int sample = 0; sample < AA_SAMPLES; sample++) {
					float v = ((float)y + random()) / HEIGHT;
					float u = ((float)x + random()) / WIDTH;
					Ray ray = camera.getRay(u, v);

					Vector3 hit_point = Vector3(0, 0, -Z_MAX);
					pixelColor += bvh_color(ray, bvh_root, hit_point);

					if ((int)hit_point.v[2] != -Z_MAX)
					{
						avg_hit_point += hit_point;
						num_hits++;
					}
				}

				if (num_hits > 0)
				{
					avg_hit_point /= (float)num_hits;
					zbuffer[y*WIDTH + x] = rt.trasnformHitPoint(avg_hit_point);
				}

				pixelColor /= AA_SAMPLES;
				pixelColor = Vector3(sqrt(pixelColor.x()), sqrt(pixelColor.y()), sqrt(pixelColor.z()));
				int r = pixelColor.r() * 255.99f;
				int g = pixelColor.g() * 255.99f;
				int b = pixelColor.b() * 255.99f;

				pixelBuffer[3 * (y*WIDTH + x)] = r;
				pixelBuffer[3 * (y*WIDTH + x) + 1] = g;
				pixelBuffer[3 * (y*WIDTH + x) + 2] = b;
			}
			//if (x % 10 == 0) std::cout << "Completed x: " << x << std::endl;
		}
	}
	else if (g_acctype == RT_AC_BVH)
	{
#pragma omp parallel for schedule(dynamic)
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				Vector3 pixelColor(0, 0, 0);
				Vector3 avg_hit_point = Vector3(0, 0, 0);
				int num_hits = 0;
				for (int sample = 0; sample < AA_SAMPLES; sample++) {
					float v = ((float)y + random()) / HEIGHT;
					float u = ((float)x + random()) / WIDTH;
					Ray ray = camera.getRay(u, v);

					Vector3 hit_point = Vector3(0, 0, -Z_MAX);
					pixelColor += ac_bvh_color(ray, bvh_root, hit_point);

					if ((int)hit_point.v[2] != -Z_MAX)
					{
						avg_hit_point += hit_point;
						num_hits++;
					}
				}

				if (num_hits > 0)
				{
					avg_hit_point /= (float)num_hits;
					zbuffer[y*WIDTH + x] = rt.trasnformHitPoint(avg_hit_point);
				}

				pixelColor /= AA_SAMPLES;
				pixelColor = Vector3(sqrt(pixelColor.x()), sqrt(pixelColor.y()), sqrt(pixelColor.z()));
				int r = pixelColor.r() * 255.99f;
				int g = pixelColor.g() * 255.99f;
				int b = pixelColor.b() * 255.99f;

				pixelBuffer[3 * (y*WIDTH + x)] = r;
				pixelBuffer[3 * (y*WIDTH + x) + 1] = g;
				pixelBuffer[3 * (y*WIDTH + x) + 2] = b;
			}
			//if (x % 10 == 0) std::cout << "Completed x: " << x << std::endl;
		}
	}
	else
	{
#pragma omp parallel for schedule(dynamic)
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				Vector3 pixelColor(0, 0, 0);
				Vector3 avg_hit_point = Vector3(0, 0, 0);
				int num_hits = 0;
				for (int sample = 0; sample < AA_SAMPLES; sample++) {
					float v = ((float)y + random()) / HEIGHT;
					float u = ((float)x + random()) / WIDTH;
					
					Vector3 hit_point = Vector3(0,0,-Z_MAX);
					
					Ray ray = camera.getRay(u, v);
					pixelColor += color(ray, hit_point);
					if ((int) hit_point.v[2] != -Z_MAX)
					{
						avg_hit_point += hit_point;
						num_hits++;
					}
				}
				if (num_hits > 0)
				{
					avg_hit_point /= (float)num_hits;
					zbuffer[y*WIDTH + x] = rt.trasnformHitPoint(avg_hit_point);
				}


				pixelColor /= AA_SAMPLES;
				pixelColor = Vector3(sqrt(pixelColor.x()), sqrt(pixelColor.y()), sqrt(pixelColor.z()));
				int r = pixelColor.r() * 255.99f;
				int g = pixelColor.g() * 255.99f;
				int b = pixelColor.b() * 255.99f;

				pixelBuffer[3 * (y*WIDTH + x)] = r;
				pixelBuffer[3 * (y*WIDTH + x) + 1] = g;
				pixelBuffer[3 * (y*WIDTH + x) + 2] = b;
			}
		}
	}

	stop = clock();
	double secs = ((double)(stop - start) / CLOCKS_PER_SEC);
	std::cout << "Finished render\t\t Took " << secs << " seconds\n";

	stbi_write_png("output.png", WIDTH, HEIGHT, 3, pixelBuffer, 3 * WIDTH);
	
	// draw bounding boxes
	//for (int i = 0; i < numShapes; i++)
	//{
	//	rt.RasterizeBox(shapeList[i]->aabb, &Vector3(0, 0, 1));
	//}
	if (g_acctype == RT_AC_BVH || g_acctype == RT_BVH)
	{
		std::cout << "------------------------------------------------------\n";
		std::cout << "\nDrawing bounding boxes... \n";
		int max_depth = bvh_root->getTreeNodeHeight();
		bvh_root->drawBoundingBox(rt, 0, max_depth);
		stbi_write_png("output_box.png", WIDTH, HEIGHT, 3, pixelBuffer, 3 * WIDTH);
		std::cout << "Done.	Max Tree depth: " << max_depth << std::endl;
	}
	return 0;
}