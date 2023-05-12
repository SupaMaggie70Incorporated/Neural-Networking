#ifndef OthelloHeader
#define OthelloHeader

#include <DeepLearning.h>

using namespace SupaDL;

int main();

void TestOptimization();
void TestVsDefault();
void GenerateNetwork();
void RunCommandlineGame();
void PlayVsMe();
void SaveNetwork(DLDomain* Domain);
void LoadNetwork(DLDomain* Domain);

#endif