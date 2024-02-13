#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// char M[100][4]; // Memory
// char R[40];     // genaralRegister
// int IC = 0;
// char IR[4];
// bool C = false; // toggle

int mCount = 0; // memory block count
int noIn = 0;   // memory card row count

// Memory initialization
void init(char M[][4])
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '-';
        }
    }
}

// display memory
void displayM(char M[][4])
{
    for (int i = 0; i < 100; i++)
    {
        if (i % 10 == 0)
        {
            printf("\nBlock %d\n\n", (i / 10 + 1));
        }
        printf("%2d", i);
        for (int j = 0; j < 4; j++)
        {
            printf("%7c", M[i][j]);
        }
        printf("\n");
    }
}

int findMemoryIndex(char n)
{
    switch (n)
    {

    case '0':
        return 0;
        break;
    case '1':
        return 10;
        break;
    case '2':
        return 20;
        break;
    case '3':
        return 30;
        break;
    default:
        return 10;
        break;
    }

    // or
    //  int x = n - '0'; // - '0' convert char to int
    //  return x * 10;
}

// read data

void read(string data, char IR[], char M[][4])
{
    int row = findMemoryIndex(IR[2]);
    int col = 0;
    for (int i = 0; i < data.length(); i++)
    {
        if (col == 4)
        {
            row++;
            col = 0;
        }
        M[row][col] = data.at(i);
        col++;
    }
    mCount += 10;
}

// write

void write(char IR[], char M[][4])
{
    int startIndex = findMemoryIndex(IR[2]);

    ofstream writeFile("output.txt", ios::app);

    for (int i = startIndex; i <= (startIndex + 9); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (M[i][j] != '-')
            {
                writeFile << M[i][j];
            }
        }
    }
    writeFile.close();
}

// Terminate

void Terminate()
{
    ofstream writeFile("output.txt", ios::app);
    writeFile << "\n\n"
              << endl;
    writeFile.close();
}

// MOS

void MOS(string data, int SI, char M[][4], char IR[])
{

    switch (SI)
    {
    case 1:
        read(data, IR, M);
        break;
    case 2:
        write(IR, M);
        break;
    case 3:
        Terminate();
        break;
    default:
        return;
        break;
    }
}

// execute program
void executeProgram(string data, int IC, char M[][4])
{
    int SI = 3;
    int C = 0;
    char IR[4];

    while (IC < noIn)
    {
        SI = 3;
        for (int i = 0; i < 4; i++)
        {
            IR[i] = M[IC][i];
        }
        IC++;
        char R[40];

        string operation = "";
        operation += IR[0];
        operation += IR[1];

        int memoryIndex = findMemoryIndex(IR[2]);

        if (operation == "LR")
        {
            SI = 0;
            int rCol = 0;
            for (int i = memoryIndex; i <= (memoryIndex + 9); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    R[rCol++] = M[i][j];
                }
            }
        }
        else if (operation == "SR")
        {
            SI = 0;
            int rCol = 0;
            for (int i = memoryIndex; i <= (memoryIndex + 9); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    M[i][j] = R[rCol++];
                }
            }
        }
        else if (operation == "CR")
        {
            int rCol = 0;
            for (int i = memoryIndex; i <= (memoryIndex + 9); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (R[rCol++] != M[i][j])
                    {
                        cout << "Not Equal";
                        break;
                    }
                }
            }
        }
        else if (operation == "BT")
        {
            if (C == 1)
            {
                IC = memoryIndex;
            }
        }
        else if (operation == "GD")
        {
            SI = 1;
        }
        else if (operation == "PD")
        {
            SI = 2;
        }
        else if (operation == "H-")
        {
            SI = 3;
        }
        else
        {
            cout << "Invalid Job" << endl;
            return;
        }
        MOS(data, SI, M, IR);
    }
}

// start execution
void startExecution(string data, char M[][4])
{
    int IC = 0;
    executeProgram(data, IC, M);
}

// loading data to memory
void load(char M[][4])
{
    ifstream readFile;
    string line;

    readFile.open("input.txt");

    while (getline(readFile, line))
    {
        string opcode = line.substr(0, 4);

        if (opcode == "$AMJ")
        {
            continue;
        }
        else if (opcode == "$END")
        {
            mCount = 0;
            continue;
        }
        else if (opcode == "$DTA")
        {
            if (getline(readFile, line))
            {
                string data = line;
                startExecution(data, M);
            }
            else
            {
                cout << "Error" << endl;
                break;
            }
        }
        else
        {
            int row = mCount;
            int col = 0;
            for (int i = 0; i < line.length(); i++)
            {
                if (col == 4)
                {
                    row++;
                    col = 0;
                }
                M[row][col] = line.at(i);
                col++;
            }

            noIn = row + 1;
        }
    }
    readFile.close();
}

int main()
{
    char M[100][4];
    init(M);
    load(M);
    displayM(M);

    return 0;
}