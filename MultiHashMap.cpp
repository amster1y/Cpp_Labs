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

int main()
{
    return 0;
}