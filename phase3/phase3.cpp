#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <queue>
#include <string.h>

using namespace std;

class supervisiorMemory
{
public:
    queue<int> EmptyBuffers;
    queue<int> InputFullBuffers;
    queue<int> OutputFullBuffers;
    char buffer[10][40];

    supervisiorMemory()
    {
        buffer[0][0] = 0;
        for (int i = 0; i < 10; i++)
            EmptyBuffers.push(i);
    }

    int allotEmptyBuffer()
    {
        int t;
        if (!EmptyBuffers.empty())
        {
            t = EmptyBuffers.front();
            EmptyBuffers.pop();
            return t;
        }
        else
            return -1;
    }

    void addInputBuffer(int buff_no)
    {
        InputFullBuffers.push(buff_no);
    }

    void addOutputBuffer(int buff_no)
    {
        OutputFullBuffers.push(buff_no);
    }
    void addEmptyBuffer(int buff_no)
    {
        EmptyBuffers.push(buff_no);
    }

    int returnIFB()
    {
        if (!InputFullBuffers.empty())
        {
            int t;
            t = InputFullBuffers.front();
            InputFullBuffers.pop();
            return t;
        }
        return -1;
    }
};

class drumMemory
{

public:
    char drum[500][4];
    queue<int> EmptyTracks;
    drumMemory()
    {
        for (int i = 0; i < 50; i++)
        {
            EmptyTracks.push(i);
        }

        for (int i = 0; i < 500; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                drum[i][j] = '-';
            }
        }
    }

    int allocateTrack()
    {
        int t;
        t = EmptyTracks.front();
        EmptyTracks.pop();
        return t;
    }

    void FreeTrack(int tno)
    {
        for (int i = tno; i < tno + 10; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                drum[i][j] = '-';
            }
        }
        EmptyTracks.push(tno);
    }

    void displayDrum()
    {
        for (int i = 0; i < 500; i++)
        {
            printf("%2d", i);
            for (int j = 0; j < 4; j++)
            {
                printf("%7c", drum[i][j]);
            }
            printf("\n");
        }
    }
};

struct PCB
{
    int PID;
    int TTL;
    int TLL;
    int TTC;
    int LLC;

    int dataCards;             // no. of data cards
    int programCards;          // no. of program cards
    int outputLines;           // no. of output lines
    int dataCardAddress;       // starting address of data cards
    int programCardAddress;    // starting address of program cards
    int outputlineCardAddress; // starting address of output lines

    int PTR;
    bool codeFlag;
    int SIState;
    int IC = 0;
    string error;
};

struct PCB *pcb1;
struct PCB *pcb2;

class OS
{
private:
    char M[300][4];
    char IR[4];
    char R[4];
    int SI;

    bool C;
    int TI;
    int PI;
    int EM; // error msg
    int rd;
    string function;
    int opAddress;

    //
    supervisiorMemory sm;
    drumMemory dm;
    int globalTime = 0;

    int CH1_TS = 5;
    int CH2_TS = 5;
    int CH3_TS = 2;

    bool CH1_flag = false;
    bool CH2_flag = false;
    bool CH3_flag = false;

    queue<PCB *> LQ;
    queue<PCB *> RQ;
    queue<PCB *> IOQ;
    queue<PCB *> TQ;
    int IOI = 0;
    string CH3Task;

public:
    void
    init();
    void load();
    void display();
    void startExecution();
    void executeProgram();
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
    int allocateFrame();
    int addressMap(int VR, PCB *p);
    int find(char a, char b);
    void simulation(PCB *pcb);
    string errmsg(int EM);
    bool error;

    void startCHi();

    void channel1();
    void channel2();
    void channel3();
    void clearbuffer(char buffer[][40], int n);

    void loadPCTOMemory();
    void clearMainMemory();
};

void OS::clearMainMemory()
{
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '-';
        }
    }
}

void OS::clearbuffer(char buffer[][40], int n)
{
    for (int i = 0; i < strlen(buffer[n]); i++)
    {
        buffer[n][i] = '\0';
    }
}

string OS::errmsg(int EM)
{
    string error;

    switch (EM)
    {
    case 0:
        error = "\nNO Error\n";
        break;
    case 1:
        error = "\nOut Of Data\n";
        break;
    case 2:
        error = "\nLine Limit Exceeded\n";
        break;
    case 3:
        error = "\nTime Limit Exceeded\n";
        break;
    case 4:
        error = "\nOpration Code Error\n";
        break;
    case 5:
        error = "\nOprand Error\n";
        break;
    case 6:
        error = "\nInvalid Page Fault\n";
        break;
    }
    return error;
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

    IOI = 0;

    for (int i = 0; i < 10; i++)
    {
        clearbuffer(sm.buffer, i);
    }
}

int OS::allocateFrame()
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

    return ptsi;
}

void OS::read()
{

    pcb2 = IOQ.front();

    if (dm.drum[pcb2->dataCardAddress][0] == '$')
    {
        errmsg(1);
        terminate();
        // exit(0);
        error = true;
    }

    IR[3] = '0';
    int address = find(IR[2], IR[3]);
    int drumAdd = pcb2->dataCardAddress;
    int drumAddCopy = drumAdd;
    int row = rd;
    int col = 0;
    for (int i = 0; i < 40; i++)
    {
        if (col > 3)
        {
            col = 0;
            row++;
            drumAdd++;
        }
        M[row][col] = dm.drum[drumAdd][col];
        col++;
    }
    dm.FreeTrack(drumAddCopy);
    RQ.push(IOQ.front());
    IOQ.pop();
}

void OS::write()
{
    pcb2 = IOQ.front();
    pcb2->LLC++;
    if (pcb2->LLC > pcb2->TLL)
    {
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(2);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        // exit(0);
        error = true;
        return;
    }

    int startAdd = rd;
    int endAdd = startAdd + 10;
    int drumAddress = dm.allocateTrack() * 10;
    int drumAddressCopy = drumAddress;
    for (int i = startAdd; i < endAdd; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (M[i][j] != '-')
            {
                dm.drum[drumAddress][j] = M[i][j];
            }
        }
        drumAddress++;
    }
    pcb2->outputLines++;
    if (pcb2->outputlineCardAddress == -1)
    {
        pcb2->outputlineCardAddress = drumAddressCopy;
    }
    RQ.push(IOQ.front());
    IOQ.pop();
}

void OS::MOS()
{
    if (TI == 0 && SI == 1)
    {
        // Move PCB, RQ → IOQ (Read)
        pcb2 = RQ.front();
        pcb2->SIState = 1;
        IOQ.push(RQ.front());
        RQ.pop();
        // read();
    }
    else if (TI == 0 && SI == 2)
    {
        // Move PCB, RQ → IOQ (Write)
        pcb2 = RQ.front();
        pcb2->SIState = 2;
        IOQ.push(RQ.front());
        RQ.pop();
        // write();
    }
    else if (TI == 0 && SI == 3 && PI == 0)
    {
        // Move PCB, RQ → TQ (Terminate [0])

        // terminate();
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(0);
        TQ.push(RQ.front());
        RQ.pop();
    }
    else if (TI == 2 && SI == 1)
    {
        // Move PCB, RQ → TQ (Terminate [3])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(3);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (TI == 2 && SI == 2)
    {
        // Move PCB, RQ → IOQ(Write) then TQ(Terminate[3])
        pcb2 = RQ.front();
        pcb2->SIState = 2;
        IOQ.push(RQ.front());
        RQ.pop();
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(3);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (TI == 2 && SI == 3 && PI == 0)
    {
        // Move PCB, RQ → TQ (Terminate [0])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(0);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (TI == 0 && PI == 1)
    {
        // Move PCB, RQ → TQ (Terminate [4])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(4);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (TI == 0 && PI == 2)
    {
        // Move PCB, RQ → TQ (Terminate [5])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(5);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (TI == 0 && PI == 3)
    {
        // cout << "RQ : " << RQ.size();
        pcb2 = RQ.front();
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
                M[pcb2->PTR + (opAddress / 10)][k] = progce.at(i);
                k--;
            }

            for (int i = k; i >= 0; i--)
            {
                M[pcb2->PTR + (opAddress / 10)][i] = '0';
            }
            pcb2->IC--;
        }
        else if (function == "PD") // invalid page fault
        {
            pcb2 = RQ.front();
            pcb2->SIState = 3;
            pcb2->error = errmsg(6);
            TQ.push(RQ.front());
            RQ.pop();
            terminate();
            error = true;
        }
        else
        {
            pcb2 = RQ.front();
            pcb2->SIState = 3;
            pcb2->error = errmsg(4);
            TQ.push(RQ.front());
            RQ.pop();
            terminate();
            error = true;
        }
    }
    else if (TI == 2 && PI == 1)
    {
        // Move PCB, RQ → TQ (TERMINATE [3,4])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        TQ.push(RQ.front());
        RQ.pop();
        errmsg(3);
        errmsg(4);
        terminate();
        error = true;
    }
    else if (TI == 2 && PI == 2)
    {
        // Move PCB, RQ → TQ (Terminate [3,5])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        TQ.push(RQ.front());
        RQ.pop();
        errmsg(3);
        errmsg(5);
        terminate();
        error = true;
    }
    else if (TI == 2 && PI == 3)
    {
        // Move PCB, RQ → TQ (Terminate [3])
        pcb2 = RQ.front();
        pcb2->SIState = 3;
        pcb2->error = errmsg(3);
        TQ.push(RQ.front());
        RQ.pop();
        terminate();
        error = true;
    }
    else if (IOI == 1)
    {
        // IR1
        // printf("IR1\n");
        globalTime += 5;
        channel1();
    }
    else if (IOI == 2)
    {
        // IR2
        // printf("IR2\n");
        globalTime += 5;
        channel2();
    }
    else if (IOI == 3)
    {
        // IR2, IR1
        // printf("IR2 IR1\n");
        globalTime += 5;
    }
    else if (IOI == 4)
    {
        // IR3
        // printf("IR3\n");
        globalTime += 2;
        channel3();
    }
    else if (IOI == 5)
    {
        // IR1, IR3
        // printf("IR1 IR3\n");
        globalTime += 5;
        channel1();
        channel3();
    }
    else if (IOI == 6)
    {
        // IR3, IR2
        // printf("IR3 IR2\n");
        channel3();
        channel2();
        globalTime += 5;
    }
    else if (IOI == 7)
    {
        // IR2, IR1, IR3
        // printf("IR2 IR1 IR3\n");
        channel1();
        channel2();
        channel3();
        globalTime += 5;
    }
}

int OS::addressMap(int VR, PCB *p)
{
    int pte, rd;
    if (VR >= 0 && VR <= 99)
    {
        pte = p->PTR + VR / 10;
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

void OS::simulation(PCB *pcb)
{
    if (pcb->TTC > pcb->TTL)
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

    pcb2 = RQ.front();
    while (true)
    {
        if (error)
        {
            // cout << "Fuck error \n";
            // cout << "TQ : " << TQ.size() << endl;
            // cout << "RQ : " << RQ.size() << endl;
            // cout << "LQ : " << LQ.size() << endl;
            // cout << "IOQ : " << IOQ.size() << endl;
            // exit(0);
            return;
        }
        SI = 3;
        PI = 0;
        TI = 0;

        int RA = addressMap(pcb2->IC, pcb2);

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
        pcb2->IC++;

        function = "";
        function = function + IR[0];
        function = function + IR[1];

        int operandAddress;

        if (IR[0] != 'H')
        {
            opAddress = find(IR[2], IR[3]);
            operandAddress = addressMap(opAddress, pcb2);
            if (error)
            {
                return;
            }
        }

        if (operandAddress == -1 && IR[0] != 'H')
        {
            PI = 3;
            simulation(pcb2);
            MOS();
            continue;
        }
        if (function == "GD")
        {
            SI = 1;
            pcb2->TTC += 2;
            rd = operandAddress;
        }
        else if (function == "PD")
        {
            SI = 2;
            pcb2->TTC++;
            rd = operandAddress;
        }
        else if (function == "H ")
        {
            SI = 3;
            pcb2->TTC++;
            simulation(pcb2);
            MOS();
            break;
        }
        else if (function == "LR")
        {
            pcb2->TTC++;
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
            pcb2->TTC += 2;
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
            pcb2->TTC++;
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
            pcb2->TTC++;
            if (C)
            {
                pcb2->IC = operandAddress + (IR[3] - 48);
                C = false;
            }
            SI = 0;
        }
        else
        {
            SI = 0;
            PI = 1;
        }
        simulation(pcb2);
        MOS();
        if (!IOQ.empty())
        {

            if (pcb2->SIState == 1)
            {
                CH3Task = "IOGD";
            }
            else if (pcb2->SIState == 2)
            {
                CH3Task = "IOPD";
            }
            IOI += 4;
            channel3();
        }
    }
}

void OS::startExecution()
{
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
            string num = to_string(fream_no);
            int k = 3;
            for (int i = num.length() - 1; i >= 0; i--)
            {
                // M[PTR][k] = num.at(i);
                // k--;
            }
            for (int i = k; i >= 0; i--)
            {
                // M[PTR][i] = '0';
            }
        }
    } while (!readFile.eof());
}

void OS::loadPCTOMemory()
{

    cout << "load pc to main" << endl;
    pcb2 = LQ.front(); // Assign the front element to ptr

    int address = pcb2->programCardAddress;
    int addressCopy = address;
    while (pcb2->programCards != 0)
    {
        // cout << address << endl;
        if (pcb2->PTR == -1)
        {
            for (int i = 0; i < 300; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    M[i][j] = '-';
                }
            }
            pcb2->PTR = allocateFrame();
        }
        int fream_no = randomNum();
        int row = fream_no * 10;
        int col = 0;

        for (int i = 0; i < 40; i++)
        {
            if (col > 3)
            {
                col = 0;
                row++;
                address++;
            }
            M[row][col] = dm.drum[address][col];
            col++;
        }
        string num = to_string(fream_no);
        int k = 3;
        for (int i = num.length() - 1; i >= 0; i--)
        {
            M[pcb2->PTR][k] = num.at(i);
            k--;
        }
        for (int i = k; i >= 0; i--)
        {
            M[pcb2->PTR][i] = '0';
        }
        pcb2->programCards--;
    }
    dm.FreeTrack(addressCopy);
    if (pcb2->programCards == 0)
    {
        RQ.push(LQ.front());
        LQ.pop();
    }
}

void OS::channel1()
{
    IOI -= 1;
    CH1_flag = true;
    printf("In channel 1\n");
    if (readFile.eof())
    {
        printf("Input file is empty\n");
        return;
    }

    int bufferNo = sm.allotEmptyBuffer();
    if (bufferNo == -1)
    {
        printf("No empty buffer available\n");
        return;
    }

    readFile.getline(sm.buffer[bufferNo], 41);
    sm.addInputBuffer(bufferNo);
    CH1_flag = false;
}

void OS::channel2()
{
    IOI -= 2;
    CH2_flag = true;
    printf("In channel 2\n");

    if (sm.OutputFullBuffers.empty())
    {
        printf("output full buffer is empty\n");
        return;
    }
    int bufferNo = sm.OutputFullBuffers.front();

    for (int i = 0; i < 40; i++)
    {
        if (sm.buffer[bufferNo][i] != '-' && sm.buffer[bufferNo][i] != '\0')
        {
            writeFile << sm.buffer[bufferNo][i];
        }
    }
    clearbuffer(sm.buffer, bufferNo);
    sm.OutputFullBuffers.pop();
    sm.addEmptyBuffer(bufferNo);
    cout << "ifb size : " << sm.InputFullBuffers.size() << endl;
    cout << "ofb size : " << sm.OutputFullBuffers.size() << endl;
    cout << "eb size : " << sm.EmptyBuffers.size() << endl;
    CH2_flag = false;
}

void OS::channel3()
{

    printf("In channel 3\n");
    IOI -= 4;
    if (CH3Task == "IS")
    {

        CH3_flag = true;
        if (sm.InputFullBuffers.empty())
        {
            cout << "Empty inputfullbuffer\n";
            return;
        }
        int bufferNo = sm.returnIFB();
        char buffer[41];
        int i;
        for (i = 0; i < 40 && sm.buffer[bufferNo][i]; i++)
        {
            buffer[i] = sm.buffer[bufferNo][i];
        }
        buffer[i] = '\0';
        string card = string(buffer);
        clearbuffer(sm.buffer, bufferNo);
        sm.addEmptyBuffer(bufferNo);
        if (card.find("$AMJ") != -1)
        {
            pcb1 = (struct PCB *)malloc(sizeof(struct PCB));
            cout << "$AMJ" << endl;
            string pid = card.substr(4, 4);
            string ttl = card.substr(8, 4);
            string tll = card.substr(12, 4);
            pcb1->PID = stoi(pid);
            pcb1->TLL = stoi(tll);
            pcb1->TTL = stoi(ttl);
            pcb1->LLC = 0;
            pcb1->TTC = 0;
            pcb1->dataCards = 0;
            pcb1->programCards = 0;
            pcb1->PTR = -1;
            pcb1->codeFlag = true;
            pcb1->outputlineCardAddress = -1;
            pcb1->programCardAddress = -1;
            pcb1->dataCardAddress = -1;
        }
        else if (card.find("$DTA") != -1)
        {
            cout << "$DTA" << endl;
            pcb1->codeFlag = false;
        }
        else if (card.find("$END") != -1)
        {
            cout << "$END" << endl;
            LQ.push(pcb1);
        }
        else
        {
            int drumLocation = dm.allocateTrack();
            int row = drumLocation * 10;
            int col = 0;
            for (int j = 0; j < card.length(); j++)
            {
                if (col > 3)
                {
                    col = 0;
                    row++;
                }

                dm.drum[row][col] = card.at(j);
                // error
                if (card.at(j) == 'H' && pcb1->codeFlag)
                {
                    dm.drum[row][++col] = ' ';
                    dm.drum[row][++col] = ' ';
                    dm.drum[row][++col] = ' ';
                }
                col++;
            }
            if (pcb1->codeFlag)
            {
                cout << "program card\n";
                pcb1->programCardAddress = drumLocation * 10;
                pcb1->programCards++;
            }
            else
            {
                cout << "data card\n";
                pcb1->dataCardAddress = drumLocation * 10;
                pcb1->dataCards++;
            }
        }
    }
    else if (CH3Task == "LD")
    {
        clearMainMemory();
        loadPCTOMemory();
    }
    else if (CH3Task == "IOGD")
    {
        read();
    }
    else if (CH3Task == "IOPD")
    {
        write();
    }
    else if (CH3Task == "OS")
    {

        pcb2 = TQ.front();
        if (pcb2->outputlineCardAddress != -1)
        {
            int drumAddress = pcb2->outputlineCardAddress;
            int drumAddressCopy = drumAddress;
            while (pcb2->outputLines != 0)
            {
                int bufferNo = sm.allotEmptyBuffer();
                if (bufferNo == -1)
                {
                    printf("No empty buffer available\n");
                    return;
                }
                int k = 0;
                for (int i = drumAddress; i < drumAddress + 10; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        sm.buffer[bufferNo][k] = dm.drum[i][j];
                        k++;
                    }
                }
                sm.addOutputBuffer(bufferNo);
                dm.FreeTrack(drumAddressCopy);
                pcb2->outputLines--;
            }
        }
        int bufferNo = sm.allotEmptyBuffer();
        if (bufferNo == -1)
        {
            printf("No empty buffer available\n");
            return;
        }
        for (int i = 0; i < 40; i++)
        {
            if (i < pcb2->error.size())
            {
                sm.buffer[bufferNo][i] = pcb2->error.at(i);
            }
            else
            {
                sm.buffer[bufferNo][i] = '-';
            }
        }
        sm.addOutputBuffer(bufferNo);

        bufferNo = sm.allotEmptyBuffer();
        clearbuffer(sm.buffer, bufferNo);
        sm.buffer[bufferNo][0] = 'S';
        sm.buffer[bufferNo][1] = 'I';
        sm.buffer[bufferNo][2] = ':';
        sm.buffer[bufferNo][3] = '0' + SI;
        sm.buffer[bufferNo][4] = ' ';

        sm.buffer[bufferNo][5] = 'T';
        sm.buffer[bufferNo][6] = 'I';
        sm.buffer[bufferNo][7] = ':';
        sm.buffer[bufferNo][8] = '0' + TI;
        sm.buffer[bufferNo][9] = ' ';
        sm.buffer[bufferNo][10] = 'P';
        sm.buffer[bufferNo][11] = 'I';
        sm.buffer[bufferNo][12] = ':';
        sm.buffer[bufferNo][13] = '0' + PI;
        sm.buffer[bufferNo][14] = ' ';
        sm.buffer[bufferNo][15] = 'I';
        sm.buffer[bufferNo][16] = 'C';
        sm.buffer[bufferNo][17] = ':';
        sm.buffer[bufferNo][18] = '0' + pcb2->IC;
        sm.buffer[bufferNo][19] = '\n';
        sm.addOutputBuffer(bufferNo);
        TQ.pop();
    }

    CH3_flag = false;
}

void OS::startCHi()
{
    while (!readFile.eof() || !sm.InputFullBuffers.empty() || !sm.OutputFullBuffers.empty() || !LQ.empty() || !IOQ.empty() || !TQ.empty() || !RQ.empty())
    {
        if (!RQ.empty())
        {
            cout << "RQ has something in it Start execution \n";
            startExecution();
        }
        if (!sm.EmptyBuffers.empty() && !CH1_flag && !readFile.eof())
        {
            IOI += 1;
        }

        if (!CH3_flag)
        {
            if (!TQ.empty())
            {

                CH3Task = "OS";
                IOI += 4;
                SI = 0;
                PI = 0;
                TI = 0;
            }
            else if (!sm.InputFullBuffers.empty())
            {
                CH3Task = "IS";
                IOI += 4;
            }
            else if (!LQ.empty())
            {
                CH3Task = "LD";
                IOI += 4;
            }
        }

        if (!sm.OutputFullBuffers.empty() && !CH2_flag)
        {
            IOI += 2;
        }

        if (IOI != 0)
        {
            MOS();
        }
    }
}

int main()
{
    OS os;
    os.readFile.open("input.txt");
    os.writeFile.open("output.txt", ios::app);
    os.init();
    os.startCHi();
    os.readFile.close();
    os.writeFile.close();

    return 0;
}