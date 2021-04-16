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
    std::vector<size_t> m_vAvailable;

public:
    DynamicArray()
    {
        m_lCapacity = 10;
	    m_pData = new T [m_lCapacity];
	    m_lSize = 0;
    }

    ~DynamicArray()
    {
	    delete[] m_pData;
    }
    
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

    size_t push(T oNewData)
    {
        size_t lIndex = 0;
        if (!m_vAvailable.empty())
        {
            lIndex = m_vAvailable.back();
            m_pData[lIndex] = oNewData;
            m_vAvailable.pop_back();
            return lIndex;
        }
        if (m_lSize == m_lCapacity)
            resize(m_lCapacity * GrowPercent);

        lIndex = m_lSize;
	    m_pData[m_lSize++] = oNewData;
        return lIndex;
    }

    void pop()
    {
        if (m_lSize > 0)
	        m_lSize--;
    }

    void insert(size_t lIndex, T oNewData)
    {
	    if (lIndex >= m_lCapacity)
            resize(lIndex * GrowPercent);

	    m_pData[lIndex++] = oNewData;
    }

    void remove(T oData)
    {
        size_t lIndex = find(oData);
        if (lIndex != -1)
        {
            m_pData[lIndex] = 0;
            m_vAvailable.push_back(lIndex);
        }
    }

    size_t find(T oData)
    {
        for (size_t i = 0; i < m_lSize; ++i)
            if (m_pData[i] == oData)
                return i;
        return -1;
    }

    size_t size()
    {
        return m_lSize;
    }

    bool empty()
    {
        return m_lSize <= 0;
    }

    T& get(size_t lIndex)
    {
	    return m_pData[lIndex];
    }

    void clear()
    {
        m_lCapacity = 10;
        m_lSize = 0;
	    delete [] m_pData;
        m_pData = nullptr;
    }

    T& back()
    {
	    return m_pData[m_lSize - 1];
    }

    T& operator [](size_t lIndex)
    {
        if (lIndex >= m_lCapacity)
        {
            m_lSize = lIndex;
            resize(lIndex * GrowPercent);
        }

        // m_pData[m_lSize] = oNewData;
        // m_lSize++;
        return m_pData[lIndex];
    }

    T& operator [](int iIndex)
    {
        if (iIndex >= m_lCapacity)
        {
            m_lSize = iIndex;
            resize(iIndex * GrowPercent);
        }

        // m_pData[m_lSize] = oNewData;
        // m_lSize++;
        return m_pData[iIndex];
    }
};