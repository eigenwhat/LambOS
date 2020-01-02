#pragma once

#include <util/Collection.hpp>
#include <util/Pair.hpp>

namespace sys {

template <typename K, typename V>
class Map
{
  public:
    using KeyType = K;
    using MappedType = V;
    using value_type = Pair<K const, V>;

    /**
     * Returns whether or not the Map is empty.
     * @return `true` if empty, `false` otherwise.
     */
    virtual bool isEmpty() const = 0;

    /**
     * The number of elements in the Map.
     * @return A size_t equal to the number of elements.
     */
    virtual size_t size() const = 0;

    /**
     * Adds the object to the Map, growing its size.
     *
     * Some Maps may not allow duplicates to be added, and some may not
     * allow modification through this interface at all. In any case where the
     * object is *NOT* inserted, the method will return false, signifying that
     * no modification to the contents has occurred.
     *
     * @param object The object to add.
     * @return `true` if the Map changed. `false` otherwise.
     */
    virtual bool insert(value_type const &object) = 0;

    /**
     * Removes the object from the Map, reducing its size.
     *
     * Some Maps may not allow removal. If no object is removed from the
     * Map as a result of this operation (either because the object was
     * not in the Map in the first place or because the container won't
     * allow it), this method will return false, signifying that no modification
     * to the contents has occurred.
     *
     * @param object The object to remove.
     * @return `true` if the Map changed. `false` otherwise.
     */
    virtual bool remove(value_type const &object) = 0;

    /**
     * Removes all objects from the Map.
     *
     * Some Maps may not allow removal. If no object is removed from the
     * Map as a result of this operation (because the container won't
     * allow it), this method will return false, signifying that no modification
     * to the contents has occurred.
     *
     * @return `true` if the Map changed, `false` otherwise.
     */
    virtual bool clear() = 0;

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

} // libsys namespace
