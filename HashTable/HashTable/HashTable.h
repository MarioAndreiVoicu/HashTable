#pragma once
#include <iostream>
#include <list>
#include <vector>

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>>
class HashTable
{
private:
    using Bucket = std::list<std::pair<KeyType, ValueType>>;

    std::vector<Bucket> hashTable;
    Hash hashFunction;
    size_t size;
    int listCount;  //this variable is used for the hash function,and instead of calling hashTable.size() every time it is needed,having a variable to hold the count is more efficient
    float loadFactorThreshold; // used for rehashing

    void Rehash()  //if the loadFactor exceeds the specified threshold,the hashTable needs to be resized and rehashed
    {
        listCount *= 2;  //if there are too many elements for the ammount of lists there are,the list ammount doubles and the loadFactor is lowered
        std::vector<Bucket> newHashTable(listCount);

        for (const Bucket& list : hashTable)  //goes trough all the lists
            for (const std::pair<KeyType, ValueType>& pair : list)  //goes trough all the elements inside the lists
            {
                size_t listIndex = hashFunction(pair.first) % listCount;
                newHashTable[listIndex].push_front({ pair.first,pair.second });  //inserts the elements in the new hash table
            }

        hashTable = std::move(newHashTable);  //std::move is used so hashTable becomes the new rehashed map without needing to copy all the elements into it
        newHashTable.clear();  //newHashTable is no longer needed
    }

public:

    HashTable(float loadFactor = 0.8, const Hash& hashFunc = Hash(), size_t initialSize = 10)
        : hashTable(initialSize), hashFunction(hashFunc), size(0), listCount(initialSize), loadFactorThreshold(loadFactor) {};

    std::pair<KeyType, ValueType>* find(const KeyType& key)
    {
        size_t listIndex = hashFunction(key) % listCount;

        for (std::pair<KeyType, ValueType>& pair : hashTable[listIndex])  //search the list to find if there is an element with the specified key
            if (pair.first == key)
                return &pair;
        return nullptr;
    }

    void insert(const KeyType& key, const ValueType& value)
    {
        size_t listIndex = hashFunction(key) % listCount;

        for (std::pair<KeyType, ValueType>& pair : hashTable[listIndex])   //check if the key already exists so it keeps the keys unique
            if (pair.first == key) //if the key is found in the hashTable,it stores the new value over the old one
            {
                pair.second = value; //the size is not increased beacuse the value was changed and no new key was inserted
                return;
            }

        hashTable[listIndex].push_front({ key,value });
        size++;

        if ((float)size / listCount >= loadFactorThreshold)  //if the loadFactor is too high
            Rehash();
    }

    void erase(const KeyType& key)
    {
        size_t listIndex = hashFunction(key) % listCount;
        Bucket& list = hashTable[listIndex];

        for (auto it = list.begin(); it != list.end(); it++)  //iterates trough the list untill the element is found
            if (it->first == key)
            {
                it = list.erase(it); // erasing returns the next iterator,so i don't need to increment
                size--;
                return;
            }
    }

    ValueType& operator[](const KeyType& key)
    {
        size_t listIndex = hashFunction(key) % listCount;
        for (std::pair<KeyType, ValueType>& pair : hashTable[listIndex])   //check if the key already exists,if it does the function returns a reference to the value stored at the key
            if (pair.first == key)
                return pair.second;

        insert(key, ValueType());

        return find(key)->second;
    }

    bool empty()
    {
        return size == 0;
    }

    float getLoadFactor()
    {
        return (float)size / listCount;
    }

    bool contains(const KeyType& key)
    {
        if (find(key))
            return true;
        return false;
    }

    void clear()
    {
        for (Bucket& list : hashTable)
            list.clear();

        size = 0;
    }

    class Iterator
    {
        typename std::vector<Bucket>::iterator tableIterator;
        typename Bucket::iterator listIterator;

        HashTable& hashTable;
        typename std::vector<Bucket>::iterator lastList;
        typename std::vector<Bucket>::iterator firstList;

    public:
        Iterator(typename std::vector<Bucket>::iterator tableIt, typename Bucket::iterator listIt, HashTable& table)
            : tableIterator(tableIt), listIterator(listIt), hashTable(table)
        {
            lastList = hashTable.hashTable.end();  //the first hashTable is the member of the iterator,which is a reference to an instance of a HashTable, and the second hashTable is the member of the instance of class HashTable
            lastList = std::prev(lastList);
        }

        std::pair<KeyType, ValueType>& operator*()
        {
            return *listIterator;
        }

        std::pair<KeyType, ValueType>* operator->()
        {
            return &(*listIterator);
        }

        bool operator==(const Iterator& it)  //equality operator for custom iterator
        {
            return (tableIterator == it.tableIterator) && (listIterator == it.listIterator);
        }

        bool operator!=(const Iterator& it)
        {
            return !(*this == it);
        }

        void operator=(const Iterator& it)
        {
            listIterator = it.listIterator;
            tableIterator = it.tableIterator;
            hashTable = it.hashTable;
        }

        Iterator& operator++()  //pre increment
        {
            if (++listIterator != tableIterator->end())  //if there are more elements in the list,it goes to the next one
            {
                return *this;
            }

            if (tableIterator == lastList)  //if it is the last list and it has reached the last element of it
            {
                listIterator = tableIterator->end();
                return *this;
            }

            ++tableIterator;  //if it is not the last list,it goes to the next one

            while (tableIterator != lastList && tableIterator->empty())  //keeps searching for a list that is not empty
                ++tableIterator;

            if (tableIterator == lastList && tableIterator->empty())  //if it reached the last list and it is empty,then the end() has been reached
            {
                listIterator = tableIterator->end();
                return *this;
            }

            listIterator = tableIterator->begin();  //the iterator points to the beginning of the list
            return *this;
        }

        Iterator& operator++(int)  //post increment
        {
            Iterator it = *this;
            ++(*this);
            return it;
        }

        Iterator& operator--()  //pre decrement
        {
            if (listIterator != tableIterator->begin())  //if there ale elements before this one inside the list,it goes back
            {
                --listIterator;
                return *this;
            }

            if (tableIterator == hashTable.hashTable.begin())  //if the first table has been reached,there are no elements before it and begin is returned
            {
                listIterator = tableIterator->begin();
                return *this;
            }

            --tableIterator; //if it is not the first table,it goes to the previous one

            while (tableIterator != hashTable.hashTable.begin() && tableIterator->empty())  //keeps searching for a list that is not empty
                --tableIterator;

            if (tableIterator == hashTable.hashTable.begin() && tableIterator->empty())  //if it reached the first list and it is empty
            {
                *this = hashTable.begin();  //stores the beginning in the iterator //begin() returns an iterator pointing to the first element of a list that is not empty,not to the first list
                return *this;
            }

            listIterator = std::prev(tableIterator->end()); //points to the last element of the list
            return *this;
        }

        Iterator& operator--(int)  //post decrement
        {
            Iterator it = *this;
            --(*this);
            return it;
        }
    };

    Iterator begin()
    {
        typename std::vector<Bucket>::iterator tableIterator;
        typename Bucket::iterator listIterator;

        if (empty())
            return end();

        tableIterator = hashTable.begin();
        while (tableIterator->empty())  //it iterates untill it finds a list that is not empty.That first list will contain the first element
            ++tableIterator;

        listIterator = tableIterator->begin();

        return Iterator(tableIterator, listIterator, *this);
    }

    Iterator end()
    {
        typename std::vector<Bucket>::iterator tableIterator;
        typename Bucket::iterator listIterator;

        tableIterator = std::prev(hashTable.end());  //the end() is the element after the last pair in the last list
        listIterator = tableIterator->end();

        return Iterator(tableIterator, listIterator, *this);
    }

    void display()
    {
        Iterator it = begin();
        for (; it != end(); ++it)
            std::cout << "[" << it->first << "," << it->second << "] ";
    }
};
