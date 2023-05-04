// DeepLearning.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: This is an example of a library function
namespace SupaDL {
	void Setup()
	{
		printf("\nInitializing deep learning\n");
		srand(time(NULL));
	}
}