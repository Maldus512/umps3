/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef UMPS_ERROR_H
#define UMPS_ERROR_H

#include <stdexcept>
#include <string>

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& what) throw()
        : std::runtime_error(what) {}
    virtual ~Error() throw() {}
};

class FileError : public Error {
public:
    explicit FileError(const std::string& fileName) throw()
        : Error("Error accessing `" + fileName + "'"),
          fileName(fileName)
    {}
    virtual ~FileError() throw() {}

    const std::string fileName;
};

class InvalidFileFormatError : public Error {
public:
    InvalidFileFormatError(const std::string& fileName, const std::string& what) throw()
        : Error(what),
          fileName(fileName)
    {}
    virtual ~InvalidFileFormatError() throw() {}

    const std::string fileName;
};

class InvalidCoreFileError : public InvalidFileFormatError {
public:
    InvalidCoreFileError(const std::string& fileName, const std::string& what) throw()
        : InvalidFileFormatError(fileName, what)
    {}
    virtual ~InvalidCoreFileError() throw() {}
};

class CoreFileOverflow : public Error {
public:
    explicit CoreFileOverflow() throw()
        : Error("Core file too large") {}

    virtual ~CoreFileOverflow() throw() {}
};

class EthError : public Error {
public:
    EthError(unsigned int devNo) throw()
        : Error("Ethernet device error"), devNo(devNo) {}
    virtual ~EthError() throw() {}

    const unsigned int devNo;
};

// Error hook
void Panic(const char* message);

#endif // UMPS_ERROR_H
