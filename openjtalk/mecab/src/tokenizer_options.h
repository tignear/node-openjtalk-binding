#pragma once
#include <memory>
#include <vector>
namespace MeCab
{
  struct TokenizerOpenFromMemoryOptionsData
  {
    std::shared_ptr<char> data;
    size_t size;
  };
  struct TokenizerOpenFromMemoryOptions
  {
    TokenizerOpenFromMemoryOptionsData unkdic;
    TokenizerOpenFromMemoryOptionsData sysdic;
    std::vector<TokenizerOpenFromMemoryOptionsData> userdic;
    TokenizerOpenFromMemoryOptionsData property;
  };
}