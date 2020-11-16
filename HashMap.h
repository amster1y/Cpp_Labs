#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "Matrix.h"

template <typename K, typename V>
class HashMap;

template <typename K, typename V>
class Pair
{
    K key;
    V value;
    bool free;
    bool alive;
    friend class HashMap<K, V>;
    Pair(): free(true), alive(false)
    {} 
    Pair(K key, V value): key(key), value(value), free(true), alive(false)
    {} 
};

template <typename K, typename V>
class HashMap
{
protected:
    size_t size;
    Pair<K, V>* items = nullptr;
    float fullness;
    int count;
    int all_count;

    class Iterator
    {
        Pair<K, V>* pointer; 
        friend class HashMap;
        Iterator(Pair<K, V>* pointer): pointer(pointer)
        {}
        Iterator& operator=(const Iterator& orig)
        {
            if (this != &orig)
            {
                pointer = orig.pointer;

            }
            return *this;
        }

        Pair<K, V>* operator->() 
        {
            return pointer;
        }

        bool operator==(const Iterator& orig) const
        {
            return pointer==orig.pointer;
        }

        void operator++(int n)
        {
            pointer++;
        }
    };

    Iterator begin()
    {
        size_t place = 0;
        while (items[place].free && !items[place].alive)
            place++;
        Iterator iter(&items[place]);
        return iter;
    }

    Iterator end()
    {
        size_t place = size - 1;
        while(items[place].free)
            place--;
        Iterator iter(&items[place]);
        return iter;
    }

    void extension()
    {
        HashMap other(size*2);
        Iterator iter = begin();
        while (true)
        {
            if (iter.pointer->alive)
                other.add(iter->key, iter->value);
            Iterator endl = end();
            if (iter == endl)
                break;
            iter++;
        }
        size*=2;
        delete [] items;
        items = new Pair<K, V> [size];
        for (size_t i = 0; i < size; i++)
        {
            items[i].key = other.items[i].key;
            items[i].value = other.items[i].value;
            items[i].free = other.items[i].free;
            items[i].alive = other.items[i].alive;
        }
    }

    size_t index(K given_key, size_t given_size)
    {
        std::hash<K> hasher;
        return hasher(given_key) % given_size;
    }
public:
    HashMap(): size(1), fullness(0.75), count(0), all_count(0)
    {
        items = new Pair<K, V> [size];
    }

    HashMap(size_t size): size(size), fullness(0.75), count(0), all_count(0)
    {
        items = new Pair<K, V> [size];
    }

    V search(K given_key) const
    {
        size_t place = index(given_key, size);
        while (items[place].key != given_key)
        {
            if (items[place].free)
                throw "В таблице нет элемента с необходимым ключом";
            place = (place == size - 1)? 0 : place + 1;
        }
        return items[place].value;
    }

    virtual void add(K given_key, V given_value)
    {
        size_t place = index(given_key, size);
        all_count++;
        count++;
        while(items[place].free != true)
        {
            if (items[place].key == given_key)
            {
                if (items[place].alive)
                {
                    count--;
                    all_count--;
                    items[place].value = given_value;
                    return;
                }
                else
                    break;
            }
            place = (place == size - 1)? 0 : place + 1;
        }
        items[place].free = false;
        items[place].alive = true;
        items[place].key = given_key;
        items[place].value = given_value;
        if ((float)all_count/size >= fullness)
            extension();
    }

    virtual void remove(K given_key)
    {
        size_t place = index(given_key, size);
        while (items[place].key != given_key)
        {
            if (items[place].free)
                return;
            place = (place == size - 1)? 0 : place + 1;
        }
        if (items[place].alive == false)
            return;
        items[place].alive = false;
        count--;
    }

    int get_count() const
    {
        return count;
    }

    int get_unique() const
    {
        std::set<V> unique_hash;
        Iterator iter = begin();
        while (true)
        {
            if (iter.pointer->alive)
                unique_hash.insert(iter->value);
            Iterator endl = end();
            if (iter == endl)
                break;
            iter++;
        }
        return unique_hash.size();
    }

    ~HashMap()
    {
        delete [] items;
    }
};

template <typename K, typename V>
void make_hash()
{
    K key;
    V value;
    char sym;
    int n;
    std::cin >> n;
    HashMap<K, V> hash;
    for (int i = 0; i < n; i++)
    {
        std::cin >> sym;
        if (sym == 'A')
        {
            std::cin >> key >> value;
            try
            {
                hash.add(key, value);
            }
            catch (const char* msg)
            {}
        }
        else
        {
            std::cin >> key;
            try
            {
                hash.remove(key);
            }
            catch (const char* msg)
            {}
        }
    }
    std::cout << hash.get_count() << ' ' << hash.get_unique();
}

template <typename K>
void prepare_hash()
{
    char v_type;
    std::cin >> v_type;
    if (v_type == 'I')
        make_hash<K, int>();
    if (v_type == 'D')
        make_hash<K, double>();
    if (v_type == 'S')
        make_hash<K, std::string>();
}

#endif