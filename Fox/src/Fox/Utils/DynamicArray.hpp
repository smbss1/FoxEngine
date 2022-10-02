/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** DynamicArray.hpp
*/

#pragma once

#include <cstddef>
#include <vector>

template <typename T>
class DynamicArray
{
private:
    T* m_pData;
    size_t m_lSize;
    size_t m_lCapacity;
    float GrowPercent = 1.2f;

public:

    /**
     * @brief Constructor
     */
    DynamicArray()
    {
        m_lCapacity = 10;
	    m_pData = new T [m_lCapacity];
	    m_lSize = 0;
    }

    /**
     * @brief Destructor
     */
    ~DynamicArray()
    {
	    delete[] m_pData;
    }

    /**
     * @brief Reallocate memory with the new capacity
     * @param lNewCapacity the new size of the array
     */
    void resize(size_t lNewCapacity)
    {
	    T* pNewData = new T [lNewCapacity];

        if (m_pData)
        {
            for (int i = 0; i < m_lCapacity; ++i)
		        pNewData[i] = m_pData[i];
	        delete [] m_pData;
        }
	    m_pData = pNewData;
	    m_lCapacity = lNewCapacity;
    }

    /**
     * @brief Push a new element
     * @param oNewData the element
     * @return the index of the element
     */
    size_t push(T oNewData)
    {
        if (m_lSize == m_lCapacity)
            resize(m_lCapacity * GrowPercent);

	    m_pData[m_lSize++] = oNewData;
        return m_lSize;
    }

    /**
     * @brief Pop a element
     */
    void pop()
    {
        if (m_lSize > 0)
	        m_lSize--;
    }

    /**
     * @brief Add a element at the index givent in parameter
     * @param lIndex the index
     * @param oNewData the element
     */
    void insert(size_t lIndex, T oNewData)
    {
	    if (lIndex >= m_lCapacity)
            resize(lIndex * GrowPercent);

	    m_pData[lIndex++] = oNewData;
    }

    /**
     * @brief Remove an element
     * @param oData the element to remove to the array
     */
    void remove(T oData)
    {
        size_t lIndex = find(oData);
        if (lIndex != -1)
        {
            m_pData[lIndex] = -1;
        }
    }

    /**
     * @brief Research the element in the array
     * @param oData the element to search
     * @return the index of the element
     */
    size_t find(T oData)
    {
        for (size_t i = 0; i < m_lSize; ++i)
            if (m_pData[i] == oData)
                return i;
        return -1;
    }

    /**
     * @brief Get size of the array
     * @return the size of the array
     */
    size_t size() const
    {
        return m_lSize;
    }

    /**
     * @brief Check if the array is empty or not
     * @return true if the array is empty otherwise false
     */
    bool empty()
    {
        return m_lSize <= 0;
    }

    /**
     * @brief Get a element at index
     * @param lIndex the index position of the element in the array
     * @return the element at the position of the index
     */
    T& get(size_t lIndex)
    {
	    return m_pData[lIndex];
    }

    /**
     * @brief Clear the array
     */
    void clear()
    {
        m_lCapacity = 10;
        m_lSize = 0;
	    delete [] m_pData;
        m_pData = nullptr;
        resize(m_lCapacity);
    }

    /**
     * @brief Get the last added element
     * @return the last added element
     */
    T& back()
    {
	    return m_pData[m_lSize - 1];
    }

    /**
     * @brief Get the first added element
     * @return the first added element
     */
    T& front()
    {
        return m_pData[0];
    }

    T& operator [](size_t lIndex)
    {
        if (lIndex >= m_lCapacity)
        {
            m_lSize = lIndex;
            resize(lIndex * GrowPercent);
        }
        return m_pData[lIndex];
    }

    T& operator [](int iIndex)
    {
        if (iIndex >= m_lCapacity)
        {
            m_lSize = iIndex;
            resize(iIndex * GrowPercent);
        }
        return m_pData[iIndex];
    }
};