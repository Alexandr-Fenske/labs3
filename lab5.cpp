#include <iostream>
#include <windows.h>
#include <string>
#include <iomanip>

#define PAGE(address, size) ((uintptr_t)address & ~((size_t)size - 1))

using namespace std;

int main()
{
    SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	auto minAllowedAddress = reinterpret_cast<uintptr_t>(sysInfo.lpMinimumApplicationAddress);
	auto maxAllowedAddress = reinterpret_cast<uintptr_t>(sysInfo.lpMaximumApplicationAddress);
	cout << "Allowed address range: from " << hex << showbase << minAllowedAddress << " to " << maxAllowedAddress << "\n";

	cout << "Enter min: ";
	uintptr_t enteredMin;
	uintptr_t address;
	while (true)
	{
		cin >> hex >> address;
		if (address < minAllowedAddress || address > maxAllowedAddress)
        {
            cout << "Invalid address entered. Try again.\n";
        }
		else
        {
           enteredMin = address;
           break;
        }
	}

	cout << "Enter max: ";
	uintptr_t enteredMax;
	while (true)
	{
		cin >> hex >> address;
		if (address < enteredMin || address > maxAllowedAddress)
        {
            cout << "Invalid address entered. Try again.\n";
        }
		else
        {
           enteredMax = address;
           break;
        }
	}

    uintptr_t enteredMinPage = PAGE(enteredMin, sysInfo.dwPageSize);
	uintptr_t enteredMaxPage = PAGE(enteredMax, sysInfo.dwPageSize);
    MEMORY_BASIC_INFORMATION info{};
	cout << "  Address \tSize \tUsed \tType \tAccess \tState\n";
	for (uintptr_t i = enteredMinPage; i < enteredMaxPage; i += info.RegionSize)
	{
		if (VirtualQuery((void*)i, &info, sizeof info) == 0)
		{
            cerr << "VirtualQuery ERROR for page";
            continue;
		}

        cout << setw(10) << hex << showbase << i;
		cout << dec << setw(12) << info.RegionSize;

		if (info.State == MEM_FREE) {
            cout <<  "   no ";
		} else {
            cout << "   yes";
		}

        string type;
        switch (info.Type)
        {
            case MEM_IMAGE:
                type = "image   ";
                break;
            case MEM_MAPPED:
                type = "mapped  ";
                break;
            case MEM_PRIVATE:
                type = "private ";
                break;
            default:
                type = "not used";
                break;
        }
		cout << "   " << type << " ";

		string status;
        switch (info.AllocationProtect & 0x00FF)
        {
            case PAGE_WRITECOPY:
                status = "RC ";
                break;
            case PAGE_GUARD:
                status = "G  ";
                break;
            case PAGE_NOCACHE:
                status = "N  ";
                break;
            case PAGE_EXECUTE:
                status = "X  ";
                break;
            case PAGE_EXECUTE_READ:
                status = "RX ";
                break;
            case PAGE_EXECUTE_READWRITE:
                status = "RWX";
                break;
            case PAGE_READWRITE:
                status = "RW ";
                break;
            case PAGE_EXECUTE_WRITECOPY:
                status = "RXC";
                break;
            case PAGE_READONLY:
                status = "R  ";
                break;
            case PAGE_WRITECOMBINE:
                status = "Q  ";
                break;
            default:
                status = "---";
                break;
        }
        cout << "     " << status  << " ";

		char state;
		if (info.State == MEM_COMMIT)
			state = 'C';    // Committed
		if (info.State == MEM_RESERVE)
			state = 'R';    // Reserved
		if (info.State == MEM_FREE)
			state = 'F';    // Free
        cout << "     " << state << '\n';
	}

    system("pause");
    return 0;
}
