/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Transform.hpp
*/

#ifndef FOX_TRANSFORM_HPP_
#define FOX_TRANSFORM_HPP_

#include "Utils/Vec3.hpp"
#include "Utils/Quat.hpp"

namespace fox
{
    struct Transform
    {
        Vec3 position;
        Vec3 local_position;
        quat local_rotation;
        quat rotation;
        Vec3 scale;

        Transform* parent;

        /**
         * @brief Contructor
         */
        Transform() : scale(1, 1, 1), parent(nullptr) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         */
        Transform(Vec3 pos) : position(pos), scale(1, 1, 1), parent(nullptr) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         * @param sc the scale of the object
         */
        Transform(Vec3 pos, Vec3 sc) : position(pos), scale(sc), parent(nullptr) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         * @param rot the rotation of the object
         */
        Transform(Vec3 pos, quat rot) : position(pos), rotation(rot), scale(1, 1, 1), parent(nullptr) {}

        /**
         * @brief Contructor
         * @param pos the position of the object
         * @param rot the rotation of the object
         * @param sc the scale of the object
         */
        Transform(Vec3 pos, quat rot, Vec3 sc) : position(pos), rotation(rot), scale(sc), parent(nullptr) {}

        /**
         * @brief Get the position (local or global)
         * if the transform have parent the local position will be return otherwise
         * it the global position
         * @return the position (local or global)
         */
        Vec3 get_pos() const
        {
            return position;
        }

        /**
         * @brief Get the rotation (local or global)
         * * if the transform have parent the local rotation will be return otherwise
         * it the global rotation
         * @return the rotation (local or global)
         */
        quat get_rot() const
        {
            return rotation;
        }

        /**
         * @brief Get the scale
         * @return the scale
         */
        Vec3 get_scale() const
        {
            return scale;
        }

        /**
         * @brief Get the parent
         * @return the parent
         */
        Transform& get_parent() const
        {
            return *parent;
        }

        /**
         * @brief Set the position of the transform
         * @param v the new position
         */
        void set_pos(const Vec3& v)
        {
            if (parent)
                local_position = v;
            else
                position = v;
        }

        /**
         * @brief Set the position of the transform
         * @param x the new X value
         * @param y the new Y value
         * @param z the new Z value
         */
        void set_pos(float x, float y, float z = 0)
        {
            set_pos(Vec3(x, y, z));
        }

        /**
         * @brief Set the parent
         * @param p the parent
         */
        void set_parent(Transform& p)
        {
            parent = &p;
            position = get_global_pos();
            rotation = get_global_rot();
        }

        /**
         * @brief Use in the gameloop to update de hierarchy value
         */
        void update_position()
        {
            if (parent)
            {
                Vec3 new_pos = get_global_pos();
                if (new_pos != position)
                {
                    position = new_pos;
                }

                quat new_rot = get_global_rot();
                if (new_rot != rotation)
                {
                    rotation = new_rot;
                }
            }
        }

    private:
        Vec3 get_global_pos()
        {
            Vec3 res = position;
            if (parent) {
                res = local_position;
                res += parent->get_global_pos();
            }
            return res;
        }

        quat get_global_rot()
        {
            quat res = rotation;
            if (parent) {
                res = parent->get_global_rot() * local_rotation;
            }
            return res;
        }
    };
}

#endif