#pragma once

#include <util/Comparator.hpp>
#include <util/DynamicArray.hpp>
#include <util/Hasher.hpp>
#include <util/LinkedList.hpp>
#include <util/Map.hpp>

namespace _ns_LIBSYS {

template <typename K, typename V, typename KeyHasher = Hasher<K>, typename KeyEqual = decltype(EqualTo<K>)>
class HashMap : public virtual Map<K, V>
{
  public:
    using KeyType = K const;
    using MappedType = V;
    using value_type = Pair<K const, V>;

    /**
     * Returns whether or not the HashMap is empty.
    * @return `true` if empty, `false` otherwise.
    */
    bool isEmpty() const { return _size == 0; }

    /**
     * The number of elements in the HashMap.
     * @return A size_t equal to the number of elements.
     */
    size_t size() const override { return _size; }

    /**
     * Adds the key/value pair to the HashMap.
     * @param object The key/value pair to add.
     * @return `true` if the insertion occurred. `false` otherwise.
     */
    bool insert(value_type const &object) override
    {
        if (shouldResize()) {
            resize();
        }

        auto newHash = hash(object.first);

        // check if this key already exists.
        for (auto &v : _values[newHash]) {
            if (_keyEqual(v.first, object.first)) {
                v.second = object.second;
                return true;
            }
        }

        return _values[newHash].enqueue(object);
    }

    /**
     * Adds the value for the given key.
     * @param key The key for which to insert.
     * @param value The value to insert.
     * @return `true` if the insertion took place, `false` otherwise.
     */
    bool insert(KeyType &key, MappedType const &value) override
    {
        return insert(value_type(key, value));
    }

    /**
     * Removes the key/value pair from the Collection, reducing its size. The
     * object is only removed if both the key and value match.
     * @param object The key/value pair to remove.
     * @return `true` if the Collection changed. `false` otherwise.
     */
    virtual bool remove(value_type const &object)
    {
        auto keyHash = hash(object.first);
        return _values[keyHash].remove(object);
    }


    /**
     * Removes the value for the given key, if it exists.
     * @param key The key for which to remove.
     * @return `true` if the object was removed, `false` otherwise.
     */
    bool remove(KeyType &key) override
    {
        if (contains(key)) {
            auto keyHash = hash(key);
            auto &list = _values[keyHash];
            for (auto &v : list) {
                if (_keyEqual(v.first, key)) {
                    return remove(v);
                }
            }
        }

        return false;
    }

    /**
     * Checks if there is a value for the given key.
     * @param key The key for which to check.
     * @return `true` if a value exists, `false` otherwise.
     */
    bool contains(KeyType &key) const override
    {
        auto keyHash = hash(key);
        auto &list = _values[keyHash];
        for (auto &v : list) {
            if (_keyEqual(v.first, key)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns the value for the given key. If there is no such value, one is
     * created.
     * @param key The key for which to retrieve.
     * @return The value for the key, or a newly created value now at that key.
     */
    V& operator[](KeyType &key) override
    {
        auto keyHash = hash(key);
        auto &list = _values[keyHash];
        for (auto &v : list)
        {
            if (_keyEqual(v.first, key)) {
                return v.second;
            }
        }

        // doesn't exist, create defaultp
        insert(value_type(key, MappedType{}));
        value_type *foundValue = nullptr;
        for (auto &v : list)
        {
            if (_keyEqual(v.first, key)) {
                foundValue = &v;
                break;
            }
        }

        return foundValue->second;
    }

  private:
    void resize() { _values.resize(_size * 2); }
    bool shouldResize() { return _size > (_values.capacity() * 2); }

    class ArrayResizer
    {
      public:
        static LinkedList<value_type> *
        resize(LinkedList<value_type> *oldData, size_t oldSize, size_t newSize, size_t)
        {
            LinkedList<value_type> *newData = new LinkedList<value_type>[newSize];
            KeyHasher hasher;

            for (size_t i = 0; i < oldSize; ++i) { // for each array index
                for (auto& value : oldData[i]) { // for each element in linkedlist
                    KeyType &key = value.first;
                    auto newHash = hasher(key) % newSize; // rehash
                    newData[newHash].enqueue(value); // insert
                }
            }

            return newData;
        }
    };

    size_t hash(KeyType const &k) const { return _hasher(k) % _values.capacity(); }

    DynamicArray<LinkedList<value_type>, ArrayResizer> _values;
    size_t _size = 0;
    KeyHasher _hasher;
    KeyEqual _keyEqual;
};

} // libsys namespace
