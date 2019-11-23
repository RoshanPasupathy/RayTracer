#ifndef _RT_INPUT_H_
#define _RT_INPUT_H_

#include <iostream>
#include <string.h>

void ReadInput(int argc, char* argv[], int &numPrimitivesInLeaves, bool &use_morton, int &numInteriorNode)
{
	std::string s_acc(argv[1]);
	if (s_acc == "bvh")
	{
		g_acctype = RT_BVH;
		if (argc > 2)
		{
			numPrimitivesInLeaves = atoi(argv[2]);
		}

		std::string inp;
		std::cout << "Use Morton code (y/n)?";
		std::cin >> inp;
		if (inp == "y" || inp == "Y")
		{
			use_morton = true;
		}
	}
	else if (s_acc == "ac")
	{
		g_acctype = RT_AC_BVH;
		if (argc > 2)
		{
			numPrimitivesInLeaves = atoi(argv[2]);
		}
		if (argc > 3)
		{
			numInteriorNode = atoi(argv[3]);
		}
	}
}

void RequestInput(int argc, char *argv[], int &numPrimitivesInLeaves, bool &use_morton, int &numInteriorNode)
{
	//std::string inp;
	int inp;
	std::cout << "Enter an acceleration type " << std::endl;

	std::cout << "None = " << RT_NONE << std::endl;
	std::cout << "Plain BVH = " << RT_BVH << std::endl;
	std::cout << "Agglomerated BVH = " << RT_AC_BVH << std::endl;
	std::cout << ">>";
	std::cin >> inp;
	g_acctype = (acctype) inp;

	if (g_acctype == RT_BVH)
	{
		std::string s_inp;
		std::cout << "Use Morton code (y/n)?";
		std::cin >> s_inp;
		if (s_inp == "y" || s_inp == "Y")
		{
			use_morton = true;
		}

		std::cout << "Number of primitives in leaf nodes: ";
		std::cin >> numPrimitivesInLeaves;
	}
	else if (g_acctype == RT_AC_BVH)
	{
		std::cout << "Number of primitives in leaf nodes: ";
		std::cin >> numPrimitivesInLeaves;

		std::cout << "Branching Factor of Interior Nodes (Enter 0 for dynammic sizing): ";
		std::cin >> numInteriorNode;
	}
}

void HandleInput(int argc, char *argv[], int &numPrimitivesInLeaves, bool &use_morton, int &numInteriorNode)
{
	if (argc > 1)
	{
		ReadInput(argc, argv, numPrimitivesInLeaves, use_morton, numInteriorNode);
	}
	else
	{
		RequestInput(argc, argv, numPrimitivesInLeaves, use_morton, numInteriorNode);
	}
}
#endif // !_RT_INPUT_H_

