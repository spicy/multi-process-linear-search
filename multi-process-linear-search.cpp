#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

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

int findKeyInSegment(const std::vector<std::string>& lines, const std::string& key, size_t startIdx, size_t endIdx)
{
    for (size_t i = startIdx; i < endIdx; ++i)
    {
        if (lines[i] == key)
        {
            return i;  // Return the exact line number
        }
    }

    return -1;
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

    if (lines.empty())
    {
        std::cerr << "The file is empty or there was an error reading it." << std::endl;
        return 1;
    }

    if (totalProcesses <= 0)
    {
        std::cerr << "The number of processes must be greater than zero." << std::endl;
        return 1;
    }

    if (lines.size() < totalProcesses)
    {
        std::cerr << "Warning: Number of lines in the file is less than the number of processes. Adjusting processes count." << std::endl;
        totalProcesses = lines.size();
    }

    size_t segmentLength = lines.size() / totalProcesses;

    if (segmentLength == 0)
    {
        std::cerr << "Segment length is zero, which is not expected. Exiting." << std::endl;
        return 1;
    }

    std::cout << "Total lines in file: " << lines.size() << ", Segment length: " << segmentLength << std::endl;

    std::vector<pid_t> childPids;

    for (int i = 0; i < totalProcesses; ++i)
    {
        pid_t childPid = fork();

        if (childPid == 0)  // Child process
        {
            size_t startIdx = i * segmentLength;
            size_t endIdx = (i == totalProcesses - 1) ? lines.size() : startIdx + segmentLength;

            int localFoundLine = findKeyInSegment(lines, searchKey, startIdx, endIdx);

            if (localFoundLine != -1)
            {
                exit(0);  // found
            }
            else
            {
                exit(1);  // not found
            }
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
    bool found = false;

    for (int i = 0; i < totalProcesses && !found; ++i)
    {
        pid_t childPid = wait(&exitStatus);

        if (WEXITSTATUS(exitStatus) == 0)  // found
        {
            found = true;
            terminateAllChildren(childPids);
        }
    }

    if (found)
    {
        std::cerr << "Key was found! Woohoo!! :)" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "No string found" << std::endl;
        return -1;
    }
}