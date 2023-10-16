#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

// load all strings from the given file into a vector.
std::vector<std::string> loadFileContents(const std::string& filename)
{
    std::ifstream fileStream(filename);
    std::string currentLine;
    std::vector<std::string> content;

    while (getline(fileStream, currentLine))
    {
        content.push_back(currentLine);
    }

    return content;
}

// check if the key exists within the specified segment of lines.
bool doesKeyExistInSegment(const std::vector<std::string>& lines, const std::string& key, size_t startIdx, size_t endIdx)
{
    for (size_t i = startIdx; i < endIdx; ++i)
    {
        if (lines[i] == key)
        {
            return true;
        }
    }

    return false;
}

void terminateAllChildren(const std::vector<pid_t>& children)
{
    for (const auto& childPid : children)
    {
        kill(childPid, SIGKILL);
    }
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <FILE NAME> <KEY> <NUMBER OF PROCESSES>" << std::endl;
        return 1;
    }

    const std::string filename = argv[1];
    const std::string searchKey = argv[2];
    int totalProcesses = std::stoi(argv[3]);

    std::vector<std::string> lines = loadFileContents(filename);
    size_t segmentLength = lines.size() / totalProcesses;

    std::vector<pid_t> childPids;

    for (int i = 0; i < totalProcesses; ++i)
    {
        pid_t childPid = fork();

        if (childPid == 0)  // Child process
        {
            size_t startIdx = i * segmentLength;
            size_t endIdx = (i == totalProcesses - 1) ? lines.size() : startIdx + segmentLength;

            bool found = doesKeyExistInSegment(lines, searchKey, startIdx, endIdx);
            exit(found ? 0 : 1);
        }
        else if (childPid < 0)  // Forking failed
        {
            perror("fork");
            exit(1);
        }
        else  // Parent process
        {
            childPids.push_back(childPid);
        }
    }

    int exitStatus;
    bool isKeyFound = false;

    for (int i = 0; i < totalProcesses && !isKeyFound; ++i)
    {
        wait(&exitStatus);

        if (WEXITSTATUS(exitStatus) == 0)
        {
            isKeyFound = true;
            terminateAllChildren(childPids);
        }
    }

    if (!isKeyFound)
    {
        std::cout << "No string found" << std::endl;
    }

    return 0;
}