# HashTable

HashTable is a C++ class implementation of a hash table data structure, offering efficient key-value mapping using hashing.

## Features

- **Iterator Support**: HashTable includes iterators, allowing for traversal of key-value pairs in the hash table.
- **Functions**: HashTable includes: insert, erase, find, operator[], rehashing, iterators, clear, contains, empty, getLoadFactor and display.
- **Flexible Parameters**: Customizable parameters such as load factor threshold, hash function and initial size for optimizing the hash table to specific needs.
- **Separate Chaining**: Collisions are handled using separate lists, ensuring efficient performance even in the presence of hash collisions.

## Usage

To use HashTable in your C++ projects, simply include the `HashTable.h` header file in your source files.

```cpp
#include "HashTable.h"

int main()
{
    HashTable<int, std::string> unorderedMap;  //default load factor is 0.8 //HashTable<int, std::string> unorderedMap(customLoadFactor,hashFunctor,initialListCount)

    int key;
    std::string value;

    unorderedMap.insert(1, "Apple");
    unorderedMap.insert(2, "Orange");
    unorderedMap.insert(3, "Banana");

    HashTable<int, std::string>::Iterator it = unorderedMap.begin();
    for (; it != unorderedMap.end(); ++it)
        std::cout << "[" << it->first << "," << it->second << "] ";

    unorderedMap.erase(1);

    unorderedMap[1];  //because the key 1 no longer exists,this will insert the element [1,""]
    unorderedMap[1] = "Mango";
    std::cout << "\n" << unorderedMap[3] << "\n";  //this will display Banana

    unorderedMap.display();  //this will display all the key-value pairs from the unordered map

    unorderedMap.clear();

    return 0;
}
