#pragma once

#include <util/Collection.hpp>
#include <util/Pair.hpp>

template <typename K, typename V>
class Map : public virtual Collection<Pair<K const, V>>
{
  public:
    using KeyType = K;
    using MappedType = V;
    using ValueType = Pair<K const, V>;

    /**
     * Adds the value for the given key.
     * @param key The key for which to insert.
     * @param value The value to insert.
     * @return `true` if the insertion took place, `false` otherwise.
     */
    virtual bool insert(const K &key, const V &value) = 0;

    /**
     * Removes the value for the given key, if it exists.
     * @param key The key for which to remove.
     * @return `true` if the object was removed, `false` otherwise.
     */
    virtual bool remove(const K &key) = 0;

    /**
     * Checks if there is a value for the given key.
     * @param key The key for which to check.
     * @return `true` if a value exists, `false` otherwise.
     */
    virtual bool contains(const K &key) const = 0;

    /**
     * Returns the value for the given key. If there is no such value, one is
     * created.
     * @param key The key for which to retrieve.
     * @return The value for the key, or a newly created value now at that key.
     */
    virtual V& operator[](K const &key) = 0;
};
