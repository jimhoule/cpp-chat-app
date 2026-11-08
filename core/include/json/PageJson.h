#pragma once

#include "json/Json.h"
#include "views/PageView.h"

#include <utility>
#include <vector>

template<typename TItem, typename TItemToJsonConverter>
Json ConvertPageViewToJson(const PageView<TItem>& pageView, TItemToJsonConverter convertItemToJson)
{
    std::vector<Json> itemJsons = {};
    itemJsons.reserve(pageView.items.size());
    for (const TItem& item : pageView.items)
    {
        const Json itemJson = convertItemToJson(item);
        itemJsons.push_back(itemJson);
    }

    Json pageViewJson = {};
    pageViewJson.Set("items", itemJsons);
    pageViewJson.Set("nextCursor", pageView.nextCursor);

    return pageViewJson;
}

template<typename TItem, typename TJsonToItemConverter>
PageView<TItem> ConvertJsonToPageView(const Json& json, TJsonToItemConverter convertJsonToItem)
{
    // NOTE: Throws JsonException if items is not an array of objects
    const std::vector<Json> itemJsons = json.GetObjectArray("items");

    PageView<TItem> pageView = {};
    pageView.items.reserve(itemJsons.size());
    for (const Json& itemJson : itemJsons)
    {
        /**
         * NOTES:
         *  - item is a named variable so it is an lvalue and push_back() would copy it
         *  - std::move casts it back to an rvalue so push_back() moves it instead
         *  - item must not be const because the push_back() copy overload would be picked instead
         */
        TItem item = convertJsonToItem(itemJson);
        pageView.items.push_back(std::move(item));
    }

    pageView.nextCursor = json.GetString("nextCursor");

    return pageView;
}
