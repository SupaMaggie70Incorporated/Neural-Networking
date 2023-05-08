

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

const char* FileName = "C:/Users/SupaM/Desktop/Othello_16x16_MCTS";

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
    FILE* f;
    fopen(FileName, "w");
    fwrite(Domain->Networks[0].NeuronBuffer, Domain->NetworkBufferSize, 1, f);
    fclose(f);
}
int main() {
    printf("Hello, world!\n");
    RunCommandlineGame();
    return 0;
}
void TestVsDefault() {
   
}
void GenerateNetwork()
{
    const int EvaluationsPerMove = 256;
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(16, 16, 2, 128, 2, 0.1, 16);

    printf("Hello World!\n");

    OthelloDLManager GameManager;
    DataType* ExecutionBuffer = Domain->ExecutionBuffer;
    int totalRuns = 0;
    int decisiveGames = 0;

    LoadNetwork(Domain);

    NeuralNetwork HistoricalNetwork;
    HistoricalNetwork.Initialize(Domain);
    time_t start = time(0);
    for(int generation = 0;;generation++) {
        Domain->Networks[0].DuplicateTo(&HistoricalNetwork);
        printf("%d games played", totalRuns);
        for (int i = 0; i < 1000;i++) {
            printf("\r%d games played", totalRuns);
            GameManager.Initialize(Domain, &(Domain->Networks[0]), &(Domain->Networks[1]), ExecutionBuffer);
            signed char result = GameManager.PlayTrainingPair(EvaluationsPerMove);
            if (result > 0) {
                Domain->Networks[0].MutateTo(&(Domain->Networks[1]));
                decisiveGames++;
            }
            else if (result < 0) {
                Domain->Networks[1].DuplicateTo(&(Domain->Networks[0]));
                Domain->Networks[1].Mutate();
                decisiveGames++;
            }
            else {
                Domain->Networks[1].Mutate();
            }
            //printf("Game %d played with winner %d in %d milliseconds, %d decisive games played so far\n", totalRuns++, result, (int)(now - last_time), decisiveGames);
            totalRuns++;
        }
        printf("\n");
        int score = 0;
        for (int i = 0; i < 10; i++) {
            GameManager.Initialize(Domain, &(Domain->Networks[0]), &HistoricalNetwork, ExecutionBuffer);
            score += GameManager.PlayTrainingPair(EvaluationsPerMove);
        }
        double seconds = difftime(time(0), start);
        printf("Generation %d beat generation %d by a score of %d. Status: of %d training games played, %d were decisive. Generation took %lf seconds\n", generation + 1, generation, score, totalRuns, decisiveGames, seconds);
        SaveNetwork(Domain);
    }
    exit(0);
}
void RunCommandlineGame() {
    BoardPosition board;
    board.Initialize();
    board.PrintToConsole();
    while (!board.GameEnded) {
        if(!board.CheckForValidMoves()) {
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
    printf("\nEnd result:\n", board.GetWinner());
}