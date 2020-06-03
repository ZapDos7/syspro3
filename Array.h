#ifndef ARRAY_H
#define ARRAY_H

template <typename T>
class Array
{
public:
    int size;
    int capacity;
    T *items;

    Array(int capacity) : size(0), capacity(capacity)
    {
        items = new T[capacity];
    }

    virtual ~Array()
    {
        delete[] items;
    }

    void insert(T &item)
    {
        if (size < capacity)
        {
            items[size] = item;
            size++;
        }
        else
        {
            T *buf = new T[capacity * 2];
            for (int i = 0; i < capacity; i++)
            {
                buf[i] = items[i];
            }
            delete[] items;
            items = buf;

            capacity = 2 * capacity;

            items[size] = item;
            size++;
        }
    }

    virtual void print() = 0;
};

#endif /* PAIRARRAY_H */
