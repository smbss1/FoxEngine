//
// Created by samuel on 26/06/2021.
//

#ifndef FOXENGINE_VERTEXBUFFERLAYOUT_HPP
#define FOXENGINE_VERTEXBUFFERLAYOUT_HPP

#include <vector>
#include <glad/glad.h>
#include <cassert>

namespace fox
{
    template<typename T>
    struct identity { typedef T type; };

    struct VertexBufferElement
    {
        unsigned int m_iType;
        unsigned int m_iCount;
        unsigned char m_bNormalized;

        static unsigned int GetSizeOfType(unsigned int type)
        {
            switch (type) {
                case GL_FLOAT: return sizeof(GLfloat);
                case GL_UNSIGNED_INT: return sizeof(GLuint);
                case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
            }
            assert(false);
            return 0;
        }
    };

    class VertexBufferLayout
    {
    public:
        VertexBufferLayout();

        template<typename T>
        void Push(unsigned int count)
        {
            Push(count, identity<T>());
        }

        template<typename T>
        void Push(unsigned int count, identity<T>)
        {
            assert(false);
        }

        void Push(unsigned int count, identity<float>)
        {
            m_vElements.push_back({ GL_FLOAT, static_cast<unsigned int>(count), GL_FALSE });
            m_iStride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
        }

        void Push(unsigned int count, identity<unsigned int>)
        {
            m_vElements.push_back({ GL_UNSIGNED_INT, static_cast<unsigned int>(count), GL_FALSE });
            m_iStride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);

        }

        void Push(unsigned int count, identity<unsigned char>)
        {
            m_vElements.push_back({ GL_UNSIGNED_BYTE, static_cast<unsigned int>(count), GL_TRUE });
            m_iStride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
        }

        [[nodiscard]] const std::vector<VertexBufferElement>& GetElements() const&;
        [[nodiscard]] unsigned int GetStride() const;
    private:
        std::vector<VertexBufferElement> m_vElements;
        unsigned int m_iStride;
    };
}


#endif //FOXENGINE_VERTEXBUFFERLAYOUT_HPP
