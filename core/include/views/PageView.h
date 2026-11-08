#pragma once

#include <string>
#include <vector>

template<typename TItem>
struct PageView
{
    std::vector<TItem> items;
    std::string nextCursor;
};
