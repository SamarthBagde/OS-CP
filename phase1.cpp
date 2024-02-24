#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class OS
{
private:
    char M[100][4];
    char IR[4];
    char R[4];
    int SI;
    int IC;
    bool C;
    int blockCount;
    int lineNo;

public:
    void init();
    void load();
    void display();
    void startExecution();
    void executeProgram();
    int findAddress();
    void MOS();
    void read();
    void write();
    void terminate();
    ifstream readFile;
    ofstream writeFile;
    string line;
};

void OS::display()
{
    for (int i = 0; i < 100; i++)
    {
        printf("%2d", i);
        for (int j = 0; j < 4; j++)
        {
            printf("%7c", M[i][j]);
        }
        printf("\n");
    }
}

void OS::init()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '-';
        }
    }

    blockCount = 0;
    lineNo = 0;
}

int OS::findAddress()
{
    int address = IR[2] - '0'; // to convert string to int
    address = address * 10 + (IR[3] - '0');
    return address;
}

void OS::read()
{
    getline(readFile, line);
    cout << line << endl;

    IR[3] = '0';
    int address = findAddress();

    int row = address;
    int col = 0;
    for (int i = 0; i < line.length(); i++)
    {
        if (i >= 40)
        {
            break;
        }
        if (col > 3)
        {
            col = 0;
            row++;
        }
        M[row][col] = line.at(i);
        col++;
    }
}

void OS::write()
{
    int address = findAddress();
    int end = address + 10;

    for (int i = address; i < end; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (M[i][j] != '-')
            {
                writeFile << M[i][j];
            }
        }
    }
    writeFile << "\n";
}

void OS::terminate()
{
    writeFile << "\n\n";
}

void OS::MOS()
{
    if (SI == 1)
    {
        // line = "";
        read();
    }
    else if (SI == 2)
    {
        write();
    }
    else if (SI == 3)
    {
        terminate();
    }
}

void OS::executeProgram()
{
    SI = 3;
    C = false;

    while (IC < lineNo)
    {

        for (int i = 0; i < 4; i++)
        {
            IR[i] = M[IC][i];
        }
        IC++;
        int address = findAddress();

        if (IR[0] == 'G' && IR[1] == 'D')
        {
            SI = 1;
            MOS();
        }
        else if (IR[0] == 'P' && IR[1] == 'D')
        {
            SI = 2;
            MOS();
        }
        else if (IR[0] == 'H' && IR[1] == ' ')
        {
            SI = 3;
            MOS();
            break;
        }
        else if (IR[0] == 'L' && IR[1] == 'R')
        {
            for (int i = 0; i < 4; i++)
            {
                IR[i] = M[address][i];
            }
        }
        else if (IR[0] == 'S' && IR[1] == 'R')
        {
            for (int i = 0; i < 4; i++)
            {
                M[address][i] = IR[i];
            }
        }
        else if (IR[0] == 'C' && IR[1] == 'R')
        {
            int count = 0;
            for (int i = 0; i < 4; i++)
            {
                if (M[address][i] == R[i])
                {
                    count++;
                }
            }
            if (count == 4)
            {
                C = true;
            }
            else
            {
                C = false;
            }
        }
        else if (IR[0] == 'B' && IR[1] == 'T')
        {
            if (C)
            {
                IC = address;
                C = false;
            }
        }
        else
        {
            cout << "Invalid Job" << endl;
            return;
        }
    }
}

void OS::startExecution()
{
    IC = 00;
    executeProgram();
}

void OS::load()
{

    do
    {
        getline(readFile, line);
        string opcode = line.substr(0, 4);
        // cout << line << endl;

        if (opcode == "$AMJ")
        {
            init();
            continue;
        }
        else if (opcode == "$END")
        {
            blockCount = 0;
            continue;
        }
        else if (opcode == "$DTA")
        {
            startExecution();
            continue;
        }
        else
        {
            int row = blockCount;
            int col = 0;
            for (int i = 0; i < line.length(); i++)
            {
                if (col > 3)
                {
                    col = 0;
                    row++;
                }
                M[row][col] = line.at(i);
                if (line.at(i) == 'H')
                {
                    M[row][++col] = ' ';
                    M[row][++col] = ' ';
                    M[row][++col] = ' ';
                }
                col++;
            }
            lineNo = row + 1;
        }
    } while (!readFile.eof());
}

int main()
{
    OS os;
    os.readFile.open("input.txt");
    os.writeFile.open("output.txt", ios::app);
    os.load();
    os.display();
    os.readFile.close();
    os.writeFile.close();

    return 0;
}
