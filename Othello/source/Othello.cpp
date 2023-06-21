
// Othello.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Othello.h"

#include <stdio.h>
#include <stdlib.h>
#include "OthelloBoard.h"
#include "OthelloDLManager.h"
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <limits>



using namespace SupaDL;

const char* FileName = "C:/Users/SupaM/Desktop/Othello_8x8_MCTS";

void LoadNetwork(DLDomain* Domain) {
    struct stat buffer;
    if (stat(FileName, &buffer) != 0) {
        printf("No network to load from file\n");
        return;
    }
    FILE* f = fopen(FileName, "r");
    char* buf = (char*)(Domain->Networks[0].NeuronBuffer);
    fclose(f);
    printf("Successfully loaded network from file %s\n", FileName);
}
void SaveNetwork(DLDomain* Domain) {
    FILE* f = NULL;
    f = fopen(FileName, "w");
    fwrite(Domain->Networks[0].NeuronBuffer, Domain->NetworkBufferSize, 1, f);
    fclose(f);
    printf("Saved network to file %s\n", FileName);
}
int main() {
    printf("Hello, world!\n");
    fflush(stdout);
    GenerateNetwork();
    //RunCommandlineGame();
    return 0;
}
void TestOptimization() {
    printf("Testing optimization\n");
    DLDomain* domain = new DLDomain(8, 8, 2, 128, 2, 0.1, 16);
   
    OptimizedNetwork optimized;
    optimized.Initialize(domain);
    printf("Initialized optimized network\n");
    optimized.Optimize(domain->Networks);
    printf("Optimized network\n");

	for(int i = 0;i < domain->InputCount;i++) {
        domain->OptimizationInputBuffer[i] = (double)(-1 + ((double)2 * (double)rand() / RAND_MAX));
    }
    DataType outputs[2];
    optimized.Execute(domain->OptimizationInputBuffer, outputs);
    printf("Executed optimized network\n");
    domain->Networks[0].Execute(domain->OptimizationInputBuffer, domain->OptimizationOutputBuffer, domain->ExecutionBuffer);
    printf("Executed unoptimized network\n");
    printf("Output from network 1: (%lf, %lf), and from network 2: (%lf, %lf) with inputs %lf, %lf etc\n",domain->OptimizationOutputBuffer[0], domain->OptimizationOutputBuffer[1], outputs[0], outputs[1], domain->OptimizationInputBuffer[0], domain->OptimizationInputBuffer[1]);
}
void TestVsDefault() {
    const int EvaluationsPerMove = 16384;
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(8, 8, 2, 128, 2, 0.1, 16);
    printf("Hello World!\n");
    OthelloDLManager GameManager;
    GameManager.InitSearchTree(EvaluationsPerMove + 5); // 5 is a random number I chose, 1 is probably good
    DataType* ExecutionBuffer = Domain->ExecutionBuffer;
    OptimizedNetwork original;
    original.Initialize(Domain);
    original.Optimize(&Domain->Networks[0]);
    
    LoadNetwork(Domain);

    OptimizedNetwork current;
    current.Initialize(Domain);
    current.Optimize(&Domain->Networks[0]);
    OthelloDLManager manager;
    manager.InitSearchTree(EvaluationsPerMove + 5);
    int score = 0;
    int wins = 0;
    int draws = 0;
    int losses = 0;
    for(int i = 0;i < 50;i++) {
        manager.Initialize(Domain, &current, &original, ExecutionBuffer);
        signed char value = manager.PlayTrainingPair(EvaluationsPerMove);
        if(value < 0) losses++;
        else if(value > 0) wins++;
        else draws++;
        score += value;
    }
    printf("Of 50 games, final score is %d, with +%d-%d=%d", score, wins, losses, draws);
}
void GenerateNetwork()
{
    const int EvaluationsPerMove = 256;
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(8, 8, 2, 128, 2, 0.1, 16);

    printf("Hello World!\n");

    OthelloDLManager GameManager;
    GameManager.InitSearchTree(EvaluationsPerMove + 5); // 5 is a random number I chose, 1 is probably good
    DataType* ExecutionBuffer = Domain->ExecutionBuffer;
    unsigned long totalGames = 0;
    unsigned long totalDecisiveGames = 0;
    unsigned long totalEvaluations = 0;

    
    NeuralNetwork Original;
    Original.Initialize(Domain);
    Domain->Networks[0].DuplicateTo(&Original);
    OptimizedNetwork OptimizedOriginal;
    OptimizedOriginal.Initialize(Domain);
    OptimizedOriginal.Optimize(&Original);

    LoadNetwork(Domain);

    OptimizedNetwork Optimized1;
    OptimizedNetwork Optimized2;
    Optimized1.Initialize(Domain);
    Optimized2.Initialize(Domain);
    Optimized1.Optimize(&Domain->Networks[0]);
    Optimized2.Optimize(&Domain->Networks[1]);

    

    NeuralNetwork HistoricalNetwork;
    OptimizedNetwork OptimizedHistorial;
    HistoricalNetwork.Initialize(Domain);
    OptimizedHistorial.Initialize(Domain);
    Optimized1.Optimize(&Domain->Networks[0]);
    Optimized2.Optimize(&Domain->Networks[1]);
    time_t actualStart = time(0);
    time_t start = actualStart;
    for(int generation = 0;;generation++) {
        Domain->Networks[0].DuplicateTo(&HistoricalNetwork);
        Optimized1.CopyTo(&OptimizedHistorial);
        int decisiveGames = 0;
        GameManager.Tree->TotalEvaluations = 0;
        
        for (int i = 0; i < 1000;i++) {
            printf("\r%d games played in this generation", i);
            fflush(stdout);
            GameManager.Initialize(Domain, &Optimized1, &Optimized2, ExecutionBuffer);
            signed char result = GameManager.PlayTrainingPair(EvaluationsPerMove);
            if (result > 0) {
                Domain->Networks[0].MutateTo(&(Domain->Networks[1]));
                Optimized2.Optimize(&Domain->Networks[1]);
                decisiveGames++;
            }
            else if (result < 0) {
                Domain->Networks[1].DuplicateTo(&(Domain->Networks[0]));
                Optimized2.CopyTo(&Optimized1);
                Domain->Networks[1].Mutate();
                Optimized2.Optimize(&Domain->Networks[1]);
                decisiveGames++;
            }
            else {
                Domain->Networks[1].Mutate();
                Optimized2.Optimize(&Domain->Networks[1]);
            }
        }
        printf("\n");
        totalGames += 1000;
        totalDecisiveGames += decisiveGames;
        totalEvaluations += GameManager.Tree->TotalEvaluations;
        int score = 0;
        for (int i = 0; i < 10; i++) {
            GameManager.Initialize(Domain, &Optimized1, &OptimizedHistorial, ExecutionBuffer);
            score += GameManager.PlayTrainingPair(EvaluationsPerMove);
        }
        int scoreVsDumb = 0;
        for(int i = 0;i < 10;i++) {
            GameManager.Initialize(Domain, &Optimized1, &OptimizedOriginal, ExecutionBuffer);
            scoreVsDumb += GameManager.PlayTrainingPair(EvaluationsPerMove);
        }
        // Ties are not worth saving
        if(score > 0 && scoreVsDumb > 0) {
            SaveNetwork(Domain);
        }
        else {
            HistoricalNetwork.DuplicateTo(&Domain->Networks[0]);
            Domain->Networks[0].MutateTo(&Domain->Networks[1]);
            OptimizedHistorial.CopyTo(&Optimized1);
            Optimized2.Optimize(&Domain->Networks[1]);
        }
        time_t now = time(0);
        int secondsSince = (int)difftime(now, start);
        int secondsSinceStart = (int)difftime(now, actualStart);
        printf("Generation %d beat generation %d by %d, and generation 0 by %d points after playing 1000 games, average %d per second, %d of which were decisive, and performing %lu evaluations in %d seconds, average of %d per second. This has been training for %d seconds total and had %lu games, average %d per second, %lu of which were decisive, and %lu evaluations, or on average %d per second\n",generation + 1, generation, score, scoreVsDumb, 1000 / secondsSince, decisiveGames, GameManager.Tree->TotalEvaluations, secondsSince, (int)(GameManager.Tree->TotalEvaluations / secondsSince), secondsSinceStart, totalGames, totalGames / secondsSinceStart, totalDecisiveGames, totalEvaluations, (int)(totalEvaluations / secondsSinceStart));
        start = now;
    }
    exit(0);
}
void RunCommandlineGame() {
    BoardPosition board;
    board.InitializeOutputBuffer();
    board.Initialize();
    board.PrintToConsole();
    while (!board.GameEnded) {
        if(!board.HasValidMoves()) {
            printf("Skipping turn because no valid moves\n");
            board.MakeMove(Point {-1, -1});
            continue;
        }
        Point p;
        char str[3];
        scanf(" %s", str, 3);
        if(str[1] == '0') {
            board.UndoMove();
            board.PrintToConsole();
            continue;
        }
        else if(str[0] == '-') {
            printf("Skipping turn because no valid moves\n");
            board.MakeMove(Point {-1, -1});
            board.PrintToConsole();
            continue;
        }
        p.x = str[0] - 'a';
        p.y = 7 - (str[1] - '1');
        int x, y;
        x = p.x;
        y = p.y;
        if(board.MakeMove(p)) {
            board.PrintToConsole();
        }
        else {
            printf("\nInvalid move %d,%d\n", x, y);
            board.UndoMove();
        }
    }
}