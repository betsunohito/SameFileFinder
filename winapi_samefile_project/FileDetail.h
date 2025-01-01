#ifndef FILEDETAIL_H
#define FILEDETAIL_H

#include <windows.h>
#include <string>

class FileDetail {
private:
    DWORD fSize;            // File size (in bytes)
    std::wstring fName;     // File name (wide string)
    std::wstring fAddress;  // File address/directory path (wide string)
    FILETIME fCreatedAt;    // File creation time
    FILETIME fLastChangedAt; // Last modification time
    int fIconIndex;

public:
    // Constructor
    FileDetail(DWORD initialfSize, const std::wstring& initialfName,
        const std::wstring& initialfAddress, FILETIME initialfCreatedAt,
        FILETIME initialfLastChangedAt,int initialfIconIndex);

    ~FileDetail();

    // Getter and setter for fSize
    DWORD getFSize() const;
    void setFSize(DWORD newSize);

    // Getter and setter for fName
    std::wstring getFName() const;
    void setFName(const std::wstring& newName);

    // Getter and setter for fAddress
    std::wstring getFAddress() const;
    void setFAddress(const std::wstring& newAddress);

    // Getter and setter for fCreatedAt
    FILETIME getFCreatedAt() const;
    void setFCreatedAt(FILETIME newTime);

    // Getter and setter for fLastChangedAt
    FILETIME getFLastChangedAt() const;
    void setFLastChangedAt(FILETIME newTime);

    // Getter and setter for fIconIndex
    int getfIconIndex() const;
    void setfIconIndex(int newIntegar);

};

#endif // FILEDETAIL_H