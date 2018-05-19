#pragma once

#include <Object.hpp>

#include <stddef.h>

/**
 * The root template interface in the collections class hierarchy. Collections,
 * simply put, are groups of objects. Collections may vary by their internal
 * structure and the rules surrounding how the objects in the Collection are
 * inserted, maintained, and removed. (e.g. Are duplicate objects allowed? Is it
 * ordered or unordered? Can you add/remove objects anywhere, or only in certain
 * positions?)
 * @tparam T The type of object the Collection contains.
 */
template<typename T>
class Collection : public virtual Object
{
public:
    using ValueType = T;

    /**
     * Returns whether or not the Collection is empty.
     * @return `true` if empty, `false` otherwise.
     */
    virtual bool isEmpty() const = 0;

    /**
     * The number of elements in the Collection.
     * @return A size_t equal to the number of elements.
     */
    virtual size_t size() const = 0;

    /** @name Optional Methods
     * These methods, though they must be implemented in all subclasses due to
     * language constraints, are not necessarily supported functionally. When
     * a Collection does not support the said functionality, the method (or
     * methods), should function as no-ops and, if possible, report that no
     * action has been taken.
     */

    /** @{ */

    /**
     * Adds the object to the Collection, growing its size.
     *
     * Some Collections may not allow duplicates to be added, and some may not
     * allow modification through this interface at all. In any case where the
     * object is *NOT* inserted, the method will return false, signifying that
     * no modification to the contents has occurred.
     *
     * @param object The object to add.
     * @return `true` if the Collection changed. `false` otherwise.
     */
    virtual bool insert(ValueType const &object) = 0;

    /**
     * Removes the object from the Collection, reducing its size.
     *
     * Some Collections may not allow removal. If no object is removed from the
     * Collection as a result of this operation (either because the object was
     * not in the Collection in the first place or because the container won't
     * allow it), this method will return false, signifying that no modification
     * to the contents has occurred.
     *
     * @param object The object to remove.
     * @return `true` if the Collection changed. `false` otherwise.
     */
    virtual bool remove(ValueType const &object) = 0;

    /**
     * Removes all objects from the Collection.
     *
     * Some Collections may not allow removal. If no object is removed from the
     * Collection as a result of this operation (because the container won't
     * allow it), this method will return false, signifying that no modification
     * to the contents has occurred.
     *
     * @return `true` if the Collection changed, `false` otherwise.
     */
    virtual bool clear() = 0;

    /** @} */
};
