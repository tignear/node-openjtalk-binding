#pragma once
#include <memory>
#include "resource.h"

namespace MeCab
{
  template <class T>
  struct SharedResource : public Resource<T>
  {
    SharedResource() : Resource<T>(), ptr()
    {
    }
    void open(std::shared_ptr<char> ptr, size_t length)
    {
      this->ptr = ptr;
      this->text = reinterpret_cast<T *>(ptr.get());
      this->length = length;
    }
    void close()
    {
      this->text = nullptr;
      this->length = 0;
      this->ptr.reset();
    }

  private:
    std::shared_ptr<char> ptr;
  };
}