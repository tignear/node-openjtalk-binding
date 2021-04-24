#pragma once
#include "common.h"
namespace MeCab
{
  template <class T>
  struct Resource
  {
    Resource() : text(0), length(0), fileName("not_initialized")
    {
    };
    virtual ~Resource()
    {
      this->close();
    };
    T &operator[](size_t n) { return *(text + n); }
    const T &operator[](size_t n) const { return *(text + n); }
    T *begin() { return text; }
    const T *begin() const { return text; }
    T *end() { return text + size(); }
    const T *end() const { return text + size(); }
    size_t size() const { return length / sizeof(T); }
    const char *what() { return what_.str(); }
    const char *file_name() const { return fileName.c_str(); }
    size_t file_size() const { return length; }
    bool empty() const { return (length == 0); }
    virtual void close() {}

  protected:
    T *text;
    size_t length;
    std::string fileName;
    whatlog what_;
  };
}