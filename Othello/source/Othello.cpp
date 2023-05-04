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

const char* FileName = "C:/Users/SupaM/Desktop/Othello_16x16v2";

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
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(16, 16, 2, 64, 2, 0.1, 16);

    printf("Hello World!\n");

    OthelloBoard* GameBoard = new OthelloBoard();
    OthelloDLManager GameManager;
    DataType* ExecutionBuffer = Domain->ExecutionBuffer;

    NeuralNetwork HistoricalNetwork;
    HistoricalNetwork.Initialize(Domain);
    Domain->Networks[0].DuplicateTo(&HistoricalNetwork);

    LoadNetwork(Domain);

    int score = 0;
    for (int i = 0; i < 100; i++) {
        GameManager.Initialize(Domain, &HistoricalNetwork, &Domain->Networks[0], ExecutionBuffer, GameBoard);
        score += GameManager.PlayPair();
    }
    printf("Current generation beat generation 0 by a score of %d.\n", score);
}
void GenerateNetwork()
{
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(16, 16, 2, 64, 2, 0.1, 16);

    printf("Hello World!\n");

    OthelloBoard* GameBoard = new OthelloBoard();
    OthelloDLManager GameManager;
    DataType* ExecutionBuffer = Domain->ExecutionBuffer;
    int totalRuns = 0;
    int decisiveGames = 0;

    LoadNetwork(Domain);

    NeuralNetwork HistoricalNetwork;
    HistoricalNetwork.Initialize(Domain);
    for(int generation = 0;;generation++) {
        Domain->Networks[0].DuplicateTo(&HistoricalNetwork);
        printf("%d games played", totalRuns);
        for (int i = 0; i < 10000;i++) {
            printf("\r%d games played", totalRuns);
            GameManager.Initialize(Domain, &(Domain->Networks[0]), &(Domain->Networks[1]), ExecutionBuffer, GameBoard);
            signed char result = GameManager.PlayPair();
            if (result > 0) {
                Domain->Networks[0].MutateTo(&(Domain->Networks[1]));
                decisiveGames++;
            }
            else if (result < 0) {
                Domain->Networks[1].MutateTo(&(Domain->Networks[0]));
                decisiveGames++;
            }
            else {
                Domain->Networks[0].Mutate();
                Domain->Networks[1].Mutate();
            }
            //printf("Game %d played with winner %d in %d milliseconds, %d decisive games played so far\n", totalRuns++, result, (int)(now - last_time), decisiveGames);
            totalRuns++;
        }
        printf("\n");
        int score = 0;
        for (int i = 0; i < 100; i++) {
            GameManager.Initialize(Domain, &(Domain->Networks[0]), &HistoricalNetwork, ExecutionBuffer, GameBoard);
            score += GameManager.PlayPair();
        }
        printf("Generation %d beat generation %d by a score of %d. Status: of %d training games played, %d were decisive. Generation took %d seconds\n", generation + 1, generation, score, totalRuns, decisiveGames, 0);
        SaveNetwork(Domain);
    }
    exit(0);
}
void RunCommandlineGame() {
    OthelloBoard* board = new OthelloBoard();
    board->PrintToConsole();
    while (board->Winner == 0) {
        Point p;
        char str[3];
        scanf(" %s", str, 3);
        p.x = str[0] - 'a';
        p.y = 7 - (str[1] - '1');
        int x, y;
        x = p.x;
        y = p.y;
        if (board->TryGetAfterMove(p)) {
            board->Move(p);
            board->PrintToConsole();
        }
        else {
            printf("\nInvalid move %d,%d\n", x, y);
        }
    }
    printf("\nPlayer %d wins!\n", board->Winner);
}
inline DataType GetTypeFromChar(char piece, char turn) {
    if (piece == turn) return 1;
    else if (piece == NOPIECE) return 0;
    else return -1;
}
void PlayVsMe() {
    SupaDL::Setup();
    DLDomain* Domain = new DLDomain(16, 16, 2, 64, 3, 0.1, 16);

    printf("Hello World!\n");

    OthelloBoard* board = new OthelloBoard();
    board->PrintToConsole();
    char lastStalled = false;
    while (board->Winner == 0) {
        char stalled;
        if (board->PlayerTurn == PLAYER1) {
            while (true) {
                Point p;
                stalled = true;
                for (char x = 0; x < 8; x++) {
                    for (char y = 0; y < 8; y++) {
                        p = { x, y };
                        if (board->TryGetAfterMove(p)) {
                            stalled = false;
                            break;
                        }
                    }
                    if (!stalled) break;
                }
                if (!stalled) {
                    char str[3];
                    scanf(" %s", str, 3);
                    p.x = str[0] - 'a';
                    p.y = 7 - (str[1] - '1');
                    int x, y;
                    x = p.x;
                    y = p.y;
                    if (board->TryGetAfterMove(p)) {
                        board->Move(p);
                        board->PrintToConsole();
                        break;
                    }
                    else {
                        printf("Invalid move\n");
                    }
                }
                else {
                    printf("No valid move, skipping to computer's turn\n");
                    break;
                }
            }
        }
        else if (board->PlayerTurn == PLAYER2) {
            Point bestSpot = { 127, 127 };
            DataType bestValue = std::numeric_limits<DataType>::max();

            DataType OutputBuffer[2];
            DataType grid[64];
            stalled = true;
            for (char x = 0; x < 8; x++) {
                for (char y = 0; y < 8; y++) {
                    Point p = { x, y };
                    if (!board->TryGetAfterMove(p)) continue;
                    stalled = false;
                    Domain->Networks[0].Execute(grid, OutputBuffer, NULL);
                    DataType value = OutputBuffer[0];
                    if (value > bestValue) {
                        bestSpot = p;
                        bestValue = value;
                    }
                }
            }
            if (!stalled) {
                board->Move(bestSpot);
                char str[3] = "a0";
                str[0] = bestSpot.x + 'a';
                str[1] = '1' + (7 - bestSpot.y);
                printf("Computer moved to %s", str);
                board->PrintToConsole();
            }
            else {
                printf("Skipped computers turn as no legal moves\n");
            }
        }
        if (stalled) {
            if (lastStalled) {
                break;
            }
            else {
                lastStalled = true;
            }
        }
        else {
            lastStalled = false;
        }
    }
    printf("Game finished as both players have no legal moves\n");
    // Count pieces
    int balance = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char piece = board->BoardState[x][y];
            if (piece == PLAYER1) balance++;
            else if (piece == PLAYER2) balance--;
        }
    }
    if (balance > 0) {
        printf("You won by %d points!\n", balance);
    }
    else if (balance < 0) {
        printf("You lost by %d points!\n", -balance);
    }
    else {
        printf("You drew!");
    }
    printf("\nPlayer %d wins!\n", board->Winner);
}