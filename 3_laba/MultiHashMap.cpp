#include <iostream>
#include <fstream>
#include "HashMap.h"
#include <list>

template <typename K, typename V>
class MultiHashMap: public HashMap <K, V>
{
public:
    MultiHashMap(): HashMap<K, V>()
    {}

    MultiHashMap(size_t size): HashMap<K, V>(size)
    {}

    void add(K given_key, V given_value)
    {
        size_t place = index(given_key, this->size);
        this->all_count++;
        this->count++;
        while(this->items[place].free != true)
        {
            place = (place == this->size - 1)? 0 : place + 1;
        }
        this->items[place].free = false;
        this->items[place].alive = true;
        this->items[place].key = given_key;
        this->items[place].value = given_value;
        if ((float)this->all_count/this->size >= this->fullness)
            this->extension();
    }

    void remove(K given_key)
    {
        size_t place = index(given_key, this->size);
        while (!this->items[place].free)
        {
            if (this->items[place].key == given_key && this->items[place].alive)
            {
                this->items[place].alive = false;
                this->count--;
            }
            place = (place == this->size - 1)? 0 : place + 1;
        }
    }

    ~MultiHashMap()
    {}
};

template <typename K, typename V>
void make_hash()
{
    K key;
    V value;
    char sym;
    int n;
    std::cin >> n;
    MultiHashMap<K, V> hash;
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
    if (v_type == 'M')
        make_hash<K, Matrix>();
}

int main()
{
    char k_type;
    std::cin >> k_type;
    if (k_type == 'I')
        prepare_hash<int>();
    if (k_type == 'D')
        prepare_hash<double>();
    if (k_type == 'S')
        prepare_hash<std::string>();
    if (k_type == 'M')
        prepare_hash<Matrix>(); 
    return 0;
}
