// MeCab -- Yet Another Part-of-Speech and Morphological Analyzer
//
//
//  Copyright(C) 2001-2006 Taku Kudo <taku@chasen.org>
//  Copyright(C) 2004-2006 Nippon Telegraph and Telephone Corporation
#ifndef MECAB_MMAP_H
#define MECAB_MMAP_H

#include <errno.h>
#include <string>

extern "C"
{

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#else

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
}

#include "common.h"
#include "utils.h"
#include "resource.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace MeCab
{
  template <class T>
  class Mmap : public Resource<T>
  {

#if defined(_WIN32) && !defined(__CYGWIN__)
    HANDLE hFile;
    HANDLE hMap;
#else
    int fd;
    int flag;
#endif

  public:
    // This code is imported from sufary, develoved by
    //  TATUO Yamashita <yto@nais.to> Thanks!
#if defined(_WIN32) && !defined(__CYGWIN__)
    bool open(const char *filename, const char *mode = "r")
    {
      this->close();
      unsigned long mode1, mode2, mode3;
      fileName = std::string(filename);

      if (std::strcmp(mode, "r") == 0)
      {
        mode1 = GENERIC_READ;
        mode2 = PAGE_READONLY;
        mode3 = FILE_MAP_READ;
      }
      else if (std::strcmp(mode, "r+") == 0)
      {
        mode1 = GENERIC_READ | GENERIC_WRITE;
        mode2 = PAGE_READWRITE;
        mode3 = FILE_MAP_ALL_ACCESS;
      }
      else
      {
        CHECK_FALSE(false) << "unknown open mode:" << filename;
      }

#if 1 /* for Open JTalk */
      hFile = ::CreateFileA(filename, mode1, FILE_SHARE_READ, 0,
#else
      hFile = ::CreateFileW(WPATH(filename), mode1, FILE_SHARE_READ, 0,
#endif
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
      CHECK_FALSE(hFile != INVALID_HANDLE_VALUE)
          << "CreateFile() failed: " << filename;

      length = ::GetFileSize(hFile, 0);

      hMap = ::CreateFileMapping(hFile, 0, mode2, 0, 0, 0);
      CHECK_FALSE(hMap) << "CreateFileMapping() failed: " << filename;

      text = reinterpret_cast<T *>(::MapViewOfFile(hMap, mode3, 0, 0, 0));
      CHECK_FALSE(text) << "MapViewOfFile() failed: " << filename;

      return true;
    }

    void close()
    {
      if (text)
      {
        ::UnmapViewOfFile(text);
      }
      if (hFile != INVALID_HANDLE_VALUE)
      {
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
      }
      if (hMap)
      {
        ::CloseHandle(hMap);
        hMap = 0;
      }
      text = 0;
    }

    Mmap() : hFile(INVALID_HANDLE_VALUE), hMap(0) {}

#else

    bool open(const char *filename, const char *mode = "r")
    {
      this->close();
      struct stat st;
      this->fileName = std::string(filename);

      if (std::strcmp(mode, "r") == 0)
        flag = O_RDONLY;
      else if (std::strcmp(mode, "r+") == 0)
        flag = O_RDWR;
      else
        CHECK_FALSE(false) << "unknown open mode: " << filename;

      CHECK_FALSE((fd = ::open(filename, flag | O_BINARY)) >= 0)
          << "open failed: " << filename;

      CHECK_FALSE(::fstat(fd, &st) >= 0)
          << "failed to get file size: " << filename;

      this->length = st.st_size;

#ifdef HAVE_MMAP
      int prot = PROT_READ;
      if (flag == O_RDWR)
        prot |= PROT_WRITE;
      char *p;
      CHECK_FALSE((p = reinterpret_cast<char *>(::mmap(0, this->length, prot, MAP_SHARED, fd, 0))) != MAP_FAILED)
          << "mmap() failed: " << filename;

      this->text = reinterpret_cast<T *>(p);
#else
      this->text = new T[this->length];
      CHECK_FALSE(::read(fd, this->text, this->length) >= 0)
          << "read() failed: " << filename;
#endif
      ::close(fd);
      fd = -1;

      return true;
    }

    void close()
    {
      if (fd >= 0)
      {
        ::close(fd);
        fd = -1;
      }

      if (this->text)
      {
#ifdef HAVE_MMAP
        ::munmap(reinterpret_cast<char *>(this->text), this->length);
        this->text = 0;
#else
        if (flag == O_RDWR)
        {
          int fd2;
          if ((fd2 = ::open(this->fileName.c_str(), O_RDWR)) >= 0)
          {
            ::write(fd2, this->text, this->length);
            ::close(fd2);
          }
        }
        delete[] text;
#endif
      }

      this->text = 0;
    }

    Mmap() : fd(-1) {}
#endif

    virtual ~Mmap()
    {
      this->close();
    }

  };
}
#endif
