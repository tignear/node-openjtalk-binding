#pragma once
#include <memory>
#include <vector>
namespace MeCab
{
  struct ViterbiOptionsData
  {
    std::shared_ptr<char> data;
    size_t size;
  };
  struct ViterbiOptions
  {
    ViterbiOptionsData unkdic;
    ViterbiOptionsData sysdic;
    std::vector<ViterbiOptionsData> userdic;
    ViterbiOptionsData property;
    ViterbiOptionsData matrix;
  };
}