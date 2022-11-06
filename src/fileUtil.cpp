#include "fileUtil.h"
#include <string>


bool FileUtil::createFile(wchar_t *filePath){

    DWORD fileAttr = GetFileAttributes(filePath);
    // Check whether the file exists.
	if ( INVALID_FILE_ATTRIBUTES==fileAttr || ( fileAttr & FILE_ATTRIBUTE_DIRECTORY))
	{   
        HANDLE handle = CreateFile(filePath,GENERIC_READ | GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
        if(handle==INVALID_HANDLE_VALUE){
            CloseHandle(handle);
            wstring msg =L"Failed to create file:"+wstring(filePath) + L". error code:" + std::to_wstring(GetLastError());
            MessageBox(NULL, msg.c_str(), L"WARN:", MB_OK);
            return 0;
        }
        CloseHandle(handle);
    }else{
        wcout << L"File is valid:" << filePath << endl;
    }
    return true;
}

bool FileUtil::createDir(wchar_t *dirPath){
    DWORD dirAttr = GetFileAttributes(dirPath);
    // Check whether the directory exists.
	if ( INVALID_FILE_ATTRIBUTES==dirAttr || !( dirAttr & FILE_ATTRIBUTE_DIRECTORY)){   
	    if (!CreateDirectory(dirPath, NULL)) {
            wstring msg = L"Failed to create directory:" + wstring(dirPath)+L". error code:"+ std::to_wstring(GetLastError());
            MessageBox(NULL, msg.c_str(), L"WARN:", MB_OK);
            return false;
        }
	}
    return true;

}

bool FileUtil::readFileLines(wchar_t *filePath, vector<wstring>  &lines){
    wifstream infile;
	infile.open(filePath, ios::in);
	if (!infile.is_open()){
        infile.close();
        wstring msg = L"Failed to read file:" + wstring(filePath) + L". error code:" + std::to_wstring(GetLastError());
        MessageBox(NULL, msg.c_str(), L"WARN:", MB_OK);
		return false;
	}
	wstring buf;
	while (std::getline(infile,buf)){
        lines.push_back(buf);
	}
    infile.close();
    return true;
}
bool FileUtil::writeFile(wchar_t* dirPath, vector<wstring>& lines){
    wofstream  outfile;
    outfile.open(dirPath, ios::out);
    if (!outfile.is_open()) {
        outfile.close();
        wstring msg = L"Failed to write file:" + wstring(dirPath) + L". error code:" + std::to_wstring(GetLastError());
        MessageBox(NULL, msg.c_str(), L"WARN:", MB_OK);
        return false;
    }
    for (auto it : lines) {
        outfile << it << endl;
    }
    outfile.close();
    return true;
}