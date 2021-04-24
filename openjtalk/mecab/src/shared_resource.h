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
    void open(std::shared_ptr<T> ptr, size_t length)
    {
      this->ptr = ptr;
      this->text = ptr.get();
      this->length = length;
    }
    virtual ~SharedResource()
    {
    }

  private:
    std::shared_ptr<T> ptr;
  };
}