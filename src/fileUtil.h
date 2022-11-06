#pragma once
#include <Windows.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
using namespace std;
class FileUtil{
    public:
        // Create file.
        static bool createFile(wchar_t *filePath );

        // Create directory.
        static bool createDir(wchar_t *dirPath );

        /* Read file.
        *  @param filePath:  file path.
        *  @param lines: The result of reading.
        */ 
        static bool readFileLines(wchar_t *filePath, vector<wstring>  &lines);

        /* write file.
        *  @param filePath:  file path.
        *  @param lines: What's being write.
        */
        static bool writeFile(wchar_t* dirPath, vector<wstring>& lines);
};