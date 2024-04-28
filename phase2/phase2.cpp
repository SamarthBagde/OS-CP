#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

class OS
{
private:
    char M[300][4];
    char IR[4];
    char R[4];
    int SI;
    int IC;
    bool C;
    int blockCount;
    int lineNo;
    //
    int PTR;
    int TI;
    int PI;
    int EM;
    int rd;
    string function;
    int opAddress;

    class PCB
    {
    public:
        int PID;
        int TTL;
        int TLL;
        int TTC;
        int LLC;
    } pcb;

public:
    void
    init();
    void load();
    void display();
    void startExecution();
    void executeProgram();
    // int findAddress();
    void MOS();
    void read();
    void write();
    void terminate();
    ifstream readFile;
    ofstream writeFile;
    string line;
    //
    int randomNumArr[30] = {0};
    int randomNum();
    void allocateFrame();
    int addressMap(int VR);
    int find(char a, char b);
    void simulation();
    void errmsg(int EM);
    bool error;
};

void OS::errmsg(int EM)
{
    string error;

    switch (EM)
    {
    case 0:
        error = "\nNO Error";
        break;
    case 1:
        error = "\nOut Of Data";
        break;
    case 2:
        error = "\nLine Limit Exceeded";
        break;
    case 3:
        error = "\nTime Limit Exceeded";
        break;
    case 4:
        error = "\nOpration Code Error";
        break;
    case 5:
        error = "\nOprand Error";
        break;
    case 6:
        error = "\nInvalid Page Fault";
        break;
    }

    writeFile << error << endl;
    writeFile << "SI : " << SI << "\tTI : " << TI << "\tPI : " << PI << endl;
    writeFile << "TTL : " << pcb.TTL << "\tTLL : " << pcb.TLL << "\tTTC : " << pcb.TTC << "\tLLC : " << pcb.LLC << endl;
    writeFile << "IC : " << IC;
    cout << error << endl;
    cout << "SI : " << SI << "\tTI : " << TI << "\tPI : " << PI << endl;
    cout << "TTL : " << pcb.TTL << "\tTLL : " << pcb.TLL << "\tTTC : " << pcb.TTC << "\tLLC : " << pcb.LLC << endl;
    cout << "IC : " << IC << endl;
}

void OS::terminate()
{
    writeFile << "\n\n";
}

int OS::randomNum()
{

    while (true)
    {
        srand(time(0));
        int fream_no = rand() % 30;
        if (randomNumArr[fream_no] == 0)
        {
            randomNumArr[fream_no] = 1;
            return fream_no;
        }
    }
}

void OS::display()
{
    for (int i = 0; i < 300; i++)
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
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '-';
        }
    }

    SI = 0;
    TI = 0;
    PI = 0;
    error = false;

    blockCount = 0;
    lineNo = 0;
}

void OS::allocateFrame()
{
    int ptsi; // page table starting index
    ptsi = randomNum() * 10;
    cout << "Page table location (PTR) : " << ptsi << endl;
    for (int i = ptsi; i <= ptsi + 9; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '*';
        }
    }

    PTR = ptsi;
}

void OS::read()
{
    getline(readFile, line);
    // cout << line << endl;

    if (line.at(0) == '$')
    {
        errmsg(1);
        terminate();
        // exit(0);
        error = true;
    }

    IR[3] = '0';
    int address = find(IR[2], IR[3]);

    int row = rd;
    int col = 0;
    for (int i = 0; i < line.length(); i++)
    {
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
    pcb.LLC++;
    if (pcb.LLC > pcb.TLL)
    {
        errmsg(2);
        terminate();
        // exit(0);
        error = true;
        return;
    }

    int startAdd = rd;
    int endAdd = startAdd + 10;

    for (int i = startAdd; i < endAdd; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (M[i][j] != '-')
            {
                writeFile << M[i][j];
            }
        }
    }
}

void OS::MOS()
{
    if (TI == 0 && SI == 1)
    {
        read();
    }
    else if (TI == 0 && SI == 2)
    {
        write();
    }
    else if (TI == 0 && SI == 3 && PI == 0)
    {
        errmsg(0);
        terminate();
    }
    else if (TI == 2 && SI == 1)
    {
        errmsg(3);
        terminate();
        // exit(0);
        error = true;
    }
    else if (TI == 2 && SI == 2)
    {
        write();
        errmsg(3);
        terminate();
        // exit(0);
        error = true;
    }
    else if (TI == 2 && SI == 3 && PI == 0)
    {
        errmsg(0);
        terminate();
        error = true;
    }
    else if (TI == 0 && PI == 1)
    {
        errmsg(4);
        terminate();
        // exit(0);
        error = true;
    }
    else if (TI == 0 && PI == 2)
    {
        errmsg(5);
        terminate();
        // exit(0);
        error = true;
    }
    else if (TI == 0 && PI == 3)
    {
        // page fault eror
        // valid --> allocate memmory and execute user program -->> ic--;
        // invalid --> terminate and EM = 6
        if (function == "GD" || function == "SR") // valid page fault
        {
            int fream_no = randomNum();
            string progce = to_string(fream_no);
            int k = 3;
            for (int i = progce.length() - 1; i >= 0; i--)
            {
                M[PTR + (opAddress / 10)][k] = progce.at(i);
                k--;
            }
            for (int i = k; i >= 0; i--)
            {
                M[PTR + (opAddress / 10)][i] = '0';
            }
            IC--;
        }
        else if (function == "PD") // invalid page fault
        {
            errmsg(6);
            terminate();
            // exit(0);
            error = true;
        }
        else
        {
            errmsg(4);
            terminate();
            // exit(0);
            error = true;
        }
    }
    else if (TI == 2 && PI == 1)
    {
        errmsg(3);
        errmsg(4);
        terminate();
        error = true;
    }
    else if (TI == 2 && PI == 2)
    {
        errmsg(3);
        errmsg(5);
        terminate();
        error = true;
    }
    else if (TI == 2 && PI == 3)
    {
        // error msg
        errmsg(3);
        terminate();
        // exit(0);
        error = true;
    }
}

int OS::addressMap(int VR)
{
    int pte, rd;
    if (VR >= 0 && VR <= 99)
    {
        pte = PTR + VR / 10;
        if (M[pte][0] == '*')
        {
            PI = 3;
            return -1;
        }
        else
        {
            string sfNo = ""; // start frame number
            for (int i = 0; i < 4; i++)
            {
                sfNo += M[pte][i];
            }
            int fNo = stoi(sfNo);
            rd = fNo * 10 + VR % 10;
            return rd;
        }
    }
    else
    {
        PI = 2;
        line = "";
        MOS();
    }
}

void OS::simulation()
{
    if (pcb.TTC > pcb.TTL)
    {
        TI = 2;
    }
}
int OS::find(char a, char b)
{ // function for return oprand Address

    if (isdigit(a) && isdigit(b))
    {
        int tempop = a - 48;
        tempop = tempop * 10;
        return tempop;
    }
    else
    {
        return -1;
    }
}

void OS::executeProgram()
{
    while (true)
    {

        if (error)
        {
            return;
        }
        SI = 3;
        PI = 0;
        TI = 0;

        int RA = addressMap(IC);
        if (RA == -1) // Invalid page fault
        {
            terminate();
            break;
        }
        if (M[RA][0] == '-')
        {
            break;
        }

        // load instruction from memory to Instruction Register
        for (int i = 0; i < 4; i++)
        {
            IR[i] = M[RA][i];
        }
        IC++;

        function = "";
        function = function + IR[0];
        function = function + IR[1];
        int operandAddress;

        if (IR[0] != 'H')
        {
            opAddress = find(IR[2], IR[3]);
            operandAddress = addressMap(opAddress);
            if (error)
            {
                return;
            }
        }

        if (operandAddress == -1 && IR[0] != 'H')
        {
            PI = 3;
            simulation();
            MOS();
            continue;
        }

        if (function == "GD")
        {
            SI = 1;
            pcb.TTC += 2;
            rd = operandAddress;
        }
        else if (function == "PD")
        {
            SI = 2;
            pcb.TTC++;
            rd = operandAddress;
        }
        else if (function == "H ")
        {
            SI = 3;
            pcb.TTC++;
            simulation();
            MOS();
            break;
        }
        else if (function == "LR")
        {
            pcb.TTC++;
            // cout << "In LR" << endl;
            int rCount = 0;
            for (int j = 0; j < 4; j++)
            {
                R[rCount++] = M[operandAddress][j];
            }
            SI = 0;
        }
        else if (function == "SR")
        {
            pcb.TTC += 2;
            // cout << "In SR" << endl;
            int rCount = 0;
            for (int j = 0; j < 4; j++)
            {
                M[operandAddress][j] = R[rCount++];
            }
            SI = 0;
        }
        else if (function == "CR")
        {
            pcb.TTC++;
            int count = 0;
            for (int i = 0; i < 4; i++)
            {
                if (M[operandAddress][i] == R[i])
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
            SI = 0;
        }
        else if (function == "BT")
        {
            pcb.TTC++;
            if (C)
            {
                IC = operandAddress + (IR[3] - 48);
                C = false;
            }
            SI = 0;
        }
        else
        {
            SI = 0;
            PI = 1;
        }
        simulation();
        MOS();
    }
}

void OS::startExecution()
{
    IC = 0;
    executeProgram();
}

void OS::load()
{

    do
    {
        getline(readFile, line);
        string opcode = line.substr(0, 4);
        if (opcode == "$AMJ")
        {
            init();
            allocateFrame();
            string pid = line.substr(4, 4);
            string ttl = line.substr(8, 4);
            string tll = line.substr(12, 4);
            pcb.PID = stoi(pid);
            pcb.TTL = stoi(ttl);
            pcb.TLL = stoi(tll);
            pcb.TTC = 0;
            pcb.LLC = 0;
            continue;
        }
        else if (opcode == "$END")
        {
            display();
            continue;
        }
        else if (opcode == "$DTA")
        {
            startExecution();
            continue;
        }
        else
        {
            int fream_no = randomNum();
            // cout << "Instruction : " << fream_no << endl;
            int row = fream_no * 10;
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
            // updating page table
            string num = to_string(fream_no);
            int k = 3;
            for (int i = num.length() - 1; i >= 0; i--)
            {
                M[PTR][k] = num.at(i);
                k--;
            }
            for (int i = k; i >= 0; i--)
            {
                M[PTR][i] = '0';
            }
        }
    } while (!readFile.eof());
}

int main()
{
    OS os;
    os.readFile.open("input.txt");
    os.writeFile.open("output.txt", ios::app);
    os.load();
    // os.display();
    os.readFile.close();
    os.writeFile.close();

    return 0;
}
