#pragma once
#include <list>
#include <vector>
#include <functional>

template<typename Item>
using List = std::list<Item>;

template<typename Item>
using Vector = std::vector<Item>;

// Iterate list : for each element in range [_startIndex;_endIndex] call _callback()

template<typename ListType>
void Iterate(ListType &_list, std::function<void(typename ListType::value_type &)> _callback, int _startIndex = 0, int _endIndex = -1) {
    int index = 0;
    for (typename ListType::value_type &item : _list) {
        if (index >= _startIndex) {
            if (_endIndex != -1 && index > _endIndex)
                break;
            _callback(item);
        }
        index++;
    }
}

template<typename ListType>
void IterateIndex(ListType &_list, std::function<void(typename ListType::value_type &, int)> _callback, int _startIndex = 0, int _endIndex = -1) {
    int index = 0;
    for (typename ListType::value_type &item : _list) {
        if (index >= _startIndex) {
            if (_endIndex != -1 && index > _endIndex)
                break;
            _callback(item, index - _startIndex);
        }
        index++;
    }
}

template<typename ListType>
void IterateFirstLast(ListType &_list, std::function<void(typename ListType::value_type &, bool, bool)> _callback, int _startIndex = 0, int _endIndex = -1) {
    int index = 0;
    size_t length = _list.size();
    for (typename ListType::value_type &item : _list) {
        if (index >= _startIndex) {
            if (_endIndex != -1 && index > _endIndex)
                break;
            _callback(item, index == _startIndex, index == (length - 1));
        }
        index++;
    }
}
