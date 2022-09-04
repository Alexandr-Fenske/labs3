#include <iostream>
#include <windows.h>
using namespace std;

int main()
{
    SECURITY_ATTRIBUTES attr;
    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.bInheritHandle = TRUE;
    attr.lpSecurityDescriptor = NULL;

    HANDLE inputPipeForRead;
    HANDLE inputPipeForWrite;
    CreatePipe(&inputPipeForRead, &inputPipeForWrite, &attr, 0);

    HANDLE outputPipeForRead;
    HANDLE outputPipeForWrite;
    CreatePipe(&outputPipeForRead, &outputPipeForWrite, &attr, 0);

    SetHandleInformation(inputPipeForWrite, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(outputPipeForRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO params;
    ZeroMemory(&params, sizeof(params));
    params.cb = sizeof(params);
    params.hStdInput = inputPipeForRead;
    params.hStdOutput = outputPipeForWrite;
    params.hStdError = outputPipeForWrite;
    params.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION info;
    try
    {
        CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &params, &info);
    }
    catch (std::exception& e)
    {
        printf("An error occurred: %d\n", GetLastError());
        exit(1);
    }

    try
    {
        while (true)
        {
            char* readInput = NULL;
            char inputBuffer[256];
            while (!readInput)
            {
                printf("> ");
                fgets(inputBuffer, sizeof(inputBuffer), stdin);
                if (!strncmp(inputBuffer, "thanks", 6))
                {
                    break;
                }
                else if (strncmp(inputBuffer, "please", 6))
                {
                    fprintf(stderr, "You must add please before your command.\n");
                }
                else
                {
                    readInput = inputBuffer + sizeof("please");
                }
            }

            if (readInput == NULL)
            {
                break;
            }
            else
            {
                try
                {
                    WriteFile(inputPipeForWrite, readInput, strlen(readInput), NULL, NULL);
                }
                catch (std::exception& e)
                {
                     printf("An error occurred: %d\n", GetLastError());
                }


                char outputBuffer[64];
                DWORD outputBytes;
                do
                {
                    try
                    {
                        ReadFile(outputPipeForRead, outputBuffer, sizeof(outputBuffer), &outputBytes, NULL);
                    }
                    catch (std::exception& e)
                    {
                        printf("An error occurred: %d\n", GetLastError());
                    }
                    fwrite(outputBuffer, outputBytes, 1, stdout);
                }
                while ((outputBytes != 0) && (outputBuffer[outputBytes - 1] != '>'));
            }
        }
    }
    catch (std::exception& e)
    {
        printf("An error occurred: %d\n", GetLastError());
        exit(1);
    }

    return 0;
}
