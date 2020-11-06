#include <iostream>
#include <fstream>
#include <string>
#include <set>

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

    Pair& operator=(const Pair& orig)
    {
        if (this != &orig)
        {
            key = orig.key;
            value = orig.value;
            free = orig.free;
            alive = orig.alive;
        }
        return *this;
    }

};

template <typename K, typename V>
class HashMap
{
    size_t size;
    Pair<K, V>* items = nullptr;
    std::set<V> s;
    float fullness;
    int count;
    int all_count;

    class Iterator
    {
        Pair<K, V>* pointer; 
        K first;
        V second;
        friend class HashMap;
        Iterator(Pair<K, V>* pointer): pointer(pointer)
        {
            first = pointer->key;
            second = pointer->value;
        }
        Iterator& operator=(const Iterator& orig)
        {
            if (this != &orig)
            {
                pointer = orig.pointer;
                first = pointer->key;
                second = pointer->value;
            }
            return *this;
        }

        bool operator==(const Iterator& orig) const
        {
            return (pointer==orig.pointer)? true : false;
        }

        void operator++(int n)
        {
            pointer++;
            first = pointer->key;
            second = pointer->value;
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
            other.add(iter.first, iter.second);
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
        delete & other;
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
        std::hash<K> hasher;
        size_t place = hasher(given_key) % size;
        while (items[place].key != given_key)
        {
            if (items[place].free)
                throw "В таблице нет элемента с необходимым ключом";
            place = (place == size - 1)? 0 : place + 1;
        }
        if (items[place].alive == false)
            throw "В таблице нет элемента с необходимым ключом";
        return items[place].value;
    }

    void add(K given_key, V given_value)
    {
        std::hash<K> hasher;
        size_t place = hasher(given_key) % size;
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
                    s.erase(items[place].value);
                    items[place].value = given_value;
                    s.insert(given_value);
                    return;
                }
                else
                    break;
            }
            place = (place == size - 1)? 0 : place + 1;
        }
        items[place].free = false;
        items[place].alive = true;
        s.insert(given_value);
        items[place].key = given_key;
        items[place].value = given_value;
        if ((float)all_count/size >= fullness)
            extension();
    }

    void remove(K given_key)
    {
        std::hash<K> hasher;
        size_t place = hasher(given_key) % size;
        while (items[place].key != given_key)
        {
            if (items[place].free)
                throw "В таблице нет элемента с необходимым ключом";
            place = (place == size - 1)? 0 : place + 1;
        }
        if (items[place].alive == false)
            throw "В таблице нет элемента с необходимым ключом";
        items[place].alive = false;
        count--;
        s.erase(items[place].value);
    }

    int get_count()
    {
        return count;
    }

    int get_unique_count()
    {
        return s.size();
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
    std::cout << hash.get_count() << ' ' << hash.get_unique_count();
    delete & hash;
}

int main()
{
    char k_type, v_type;
    std::cin >> k_type >> v_type;
    if (k_type == 'I' && v_type == 'I')
        make_hash<int, int>();
    if (k_type == 'I' && v_type == 'D')
        make_hash<int, double>();
    if (k_type == 'I' && v_type == 'S')
        make_hash<int, std::string>();
    if (k_type == 'D' && v_type == 'I')
        make_hash<double, int>();
    if (k_type == 'S' && v_type == 'I')
        make_hash<std::string, int>();
    if (k_type == 'D' && v_type == 'D')
        make_hash<double, double>();
    if (k_type == 'D' && v_type == 'S')
        make_hash<double, std::string>();
    if (k_type == 'S' && v_type == 'D')
        make_hash<std::string, double>();
    if (k_type == 'S' && v_type == 'S')
        make_hash<std::string, std::string>();
    return 0;
}
