// FileDetail.cpp
#include "FileDetail.h"

// Constructor
FileDetail::FileDetail(DWORD initialfSize, const std::wstring& initialfName, const std::wstring& initialfAddress,
    FILETIME initialfCreatedAt, FILETIME initialfLastChangedAt,int initialfIconIndex)
    : fSize(initialfSize), fName(initialfName), fAddress(initialfAddress),
    fCreatedAt(initialfCreatedAt), fLastChangedAt(initialfLastChangedAt), fIconIndex(initialfIconIndex){
}

FileDetail::~FileDetail() {
    // Destructor is currently empty because no dynamic memory is being used.
    // If you add resources that need cleanup, like dynamic memory or file handles, add cleanup code here.
}

// Getter and setter for fSize
DWORD FileDetail::getFSize() const {
    return fSize;
}

void FileDetail::setFSize(DWORD newSize) {
    fSize = newSize;
}

// Getter and setter for fName
std::wstring FileDetail::getFName() const {
    return fName;
}

void FileDetail::setFName(const std::wstring& newName) {
    fName = newName;
}

// Getter and setter for fAddress
std::wstring FileDetail::getFAddress() const {
    return fAddress;
}

void FileDetail::setFAddress(const std::wstring& newAddress) {
    fAddress = newAddress;
}

// Getter and setter for fCreatedAt
FILETIME FileDetail::getFCreatedAt() const {
    return fCreatedAt;
}

void FileDetail::setFCreatedAt(FILETIME newTime) {
    fCreatedAt = newTime;
}

// Getter and setter for fLastChangedAt
FILETIME FileDetail::getFLastChangedAt() const {
    return fLastChangedAt;
}

void FileDetail::setFLastChangedAt(FILETIME newTime) {
    fLastChangedAt = newTime;
}

// Getter and setter for fIconIndex
int FileDetail::getfIconIndex() const {
    return fIconIndex;
}

void FileDetail::setfIconIndex(int newIntegar) {
    fIconIndex = newIntegar;
}