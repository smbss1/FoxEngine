//
// Created by samuel on 12/11/22.
//

#include <yaml-cpp/emitter.h>
#include "Reflect.hpp"
#include "Reflection/Core/Reflect.hpp"
#include "Components.hpp"
#include "Scripting/ScriptEngine.hpp"
#include "Serializer.hpp"

//#include "glm/vec3.hpp"

// register the class before main is called
//RTTR_REGISTRATION
//{
//    using namespace rttr;
//    using namespace fox;
//
////    registration::class_<math::vec3>("Vector3")
////        .property("x", &math::vec3::GetX, &math::vec3::SetX)
////        .property("y", &math::vec3::GetY, &math::vec3::SetY)
////        .property("z", &math::vec3::GetZ, &math::vec3::SetZ)
////    ;
//
//    registration::class_<TransformComponent>("Transform")
//        .constructor<>()
//        .property("Position", &TransformComponent::position)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Rotation", &TransformComponent::GetRotation, &TransformComponent::SetRotation)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Scale", &TransformComponent::scale)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//    ;
//
//    registration::class_<SpriteRenderer>("SpriteRenderer")
//        .constructor<>()
//        .property("Sprite", &SpriteRenderer::Sprite)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Color", &SpriteRenderer::Color)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Tiling Factor", &SpriteRenderer::TilingFactor)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        ;
//
//    // Circle Renderer
//    registration::class_<CircleRenderer>("CircleRenderer")
//        .constructor<>()
//        .property("Color", &CircleRenderer::Color)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Thickness", &CircleRenderer::Thickness)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Fade", &CircleRenderer::Fade)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        ;
//
//    // Box Collider2D
//    registration::class_<BoxCollider2D>("BoxCollider2D")
//        .constructor<>()
//        .property("Offset", &BoxCollider2D::Offset)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Size", &BoxCollider2D::Size)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Density", &BoxCollider2D::Density)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Friction", &BoxCollider2D::Friction)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Restitution", &BoxCollider2D::Restitution)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Restitution Threshold", &BoxCollider2D::RestitutionThreshold)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        ;
//
//    // Circle Collider 2D
//    registration::class_<CircleCollider2D>("CircleCollider2D")
//        .constructor<>()
//        .property("Offset", &CircleCollider2D::Offset)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Radius", &CircleCollider2D::Radius)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Density", &CircleCollider2D::Density)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Friction", &CircleCollider2D::Friction)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Restitution", &CircleCollider2D::Restitution)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Restitution Threshold", &CircleCollider2D::RestitutionThreshold)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        ;
//
//    // Rigidbody 2D
//    registration::class_<Rigidbody2D>("Rigidbody2D")
//        .constructor<>()
//        .enumeration<Rigidbody2D::BodyType>("E_TransformSpace")
//        (
//            value("Static", Rigidbody2D::BodyType::Static),
//            value("Dynamic", Rigidbody2D::BodyType::Dynamic),
//            value("Kinematic", Rigidbody2D::BodyType::Kinematic),
//            metadata("GUI_DESCR", "This enum describes the transformation.")
//        )
//        .property("Body Type", &Rigidbody2D::Type)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Fixed Rotation", &Rigidbody2D::FixedRotation)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        .property("Gravity Scale", &Rigidbody2D::GravityScale)
//            (
//                metadata("GUI_DESCR", "The position of the mesh.")
//            )
//        ;
//}

namespace fox::reflection
{
//    DEFINE_TYPE(double)
//    DEFINE_TYPENAME(int8_t, "int8")
//    DEFINE_TYPENAME(int16_t, "int16")
//    DEFINE_TYPENAME(int32_t, "int32")
//    DEFINE_TYPENAME(int64_t, "int64")
//    DEFINE_TYPENAME(uint8_t, "uint8")
//    DEFINE_TYPENAME(uint16_t, "uint16")
//    DEFINE_TYPENAME(uint32_t, "uint32")
//    DEFINE_TYPENAME(uint64_t, "uint64")
//    DEFINE_TYPE(bool)
//    DEFINE_TYPE(char)
//    DEFINE_TYPENAME(const char *, "cstring")
//
//
//    DEFINE_TYPENAME(std::string, "string")
//    DEFINE_TYPENAME(std::byte, "byte")
//
//    DEFINE_TYPENAME(glm::vec2, "Vector2")
//    DEFINE_TYPENAME(glm::vec3, "Vector3")
//    DEFINE_TYPENAME(glm::vec4, "Vector4")
//
//    BEGIN_TYPE(float)
//        ADD_CONVERSION(int)
//    END_TYPE
//
//// Reflection macro usage
//    BEGIN_ATTRIBUTES_FOR(TestStruct)
//        DEFINE_MEMBER(field1)
//        DEFINE_MEMBER(field2)
//        DEFINE_MEMBER(field3)
//        DEFINE_MEMBER(field4)
//        DEFINE_MEMBER(field5)
//        DEFINE_MEMBER(field6)
//        DEFINE_MEMBER(string)
//        DEFINE_MEMBER(convert)
//        DEFINE_MEMBER_FUNC(TestStruct::get, TestStruct::set, set)
//    END_ATTRIBUTES

//    template<typename T>
//    void SerializeObject(T& arg) {
//        rttr::array_range<rttr::property> properties = rttr::type::get(arg).GetProperties();
////        const ClassInfo* objectInfo = GetClass<T>();
////
////        auto& func = objectInfo->fields[7];
////        func->Get(arg);
////        func->Set(arg, 45);
//////        func.template CallSetter(arg, 455);
//////        rapidjson::Document document;
//////        rapidjson::Value key;
//////        rapidjson::Value value;
////
//////        document.SetObject();
////
//        for (auto& field : properties)
//        {
//            field.get_value(arg).
////            if (!field || field->type == nullptr) break;
////
//////            key.SetString(field.name.c_str(), field.name.size(), document.GetAllocator());
//////            uint8_t* source = field.GetPtr(&arg);
////
////            switch (field->type->id) {
////                case Hash::ConstFNVHash("int8"): {
////                    FOX_INFO("Parse int8 value: % %", (int16_t)field->Get(Any(arg)).AsRef<int8_t>(), KindToString(field->type->kind));
////                    break;
////                }
////                case Hash::ConstFNVHash("int16"): {
////                    FOX_INFO("Parse int16 value: % %", field->Get(Any(arg)).AsRef<int16_t>(), KindToString(field->type->kind));
////                    break;
////                }
////                case Hash::ConstFNVHash("int32"): {
////                    FOX_INFO("Parse int32 value: % %", *field->Get(Any(arg)).TryCast<int32_t>(), KindToString(field->type->kind));
////                    break;
////                }
////
////                case Hash::ConstFNVHash("uint8"): {
////                    FOX_INFO("Parse uint8 value: % %", *field->Get(Any(arg)).TryCast<uint8_t>(), KindToString(field->type->kind));
////                    break;
////                }
////                case Hash::ConstFNVHash("uint16"): {
////                    FOX_INFO("Parse uint16 value: % %", *field->Get(Any(arg)).As<uint16_t>(), KindToString(field->type->kind));
////                    break;
////                }
////                case Hash::ConstFNVHash("uint32"): {
////                    FOX_INFO("Parse uint32 value: % %", *field->Get(Any(arg)).TryCast<uint32_t>(), KindToString(field->type->kind));
////                    break;
////                }
////
////                case Hash::ConstFNVHash("string"): {
////                    FOX_INFO("Parse string value: % %", *field->Get(Any(arg)).TryCast<std::string>(), KindToString(field->type->kind));
////                    break;
////                }
////
////                case Hash::ConstFNVHash("float"): {
////
////                    Any value = field->Get(arg);
////
////                    FOX_INFO("float value %", *value.template TryCast<float>());
////                    FOX_INFO("int value %", *value.template TryConvert<int>().template TryCast<int>());
////                    break;
////                }
////
////                default:
//////                    assert(false);
////                    break;
////            }
//        }
//    }
}

namespace fox
{
    void Reflection::Init()
    {
        Reflect::Reflect<int>("Int");
        Reflect::Reflect<long>("Long");
        Reflect::Reflect<bool>("Bool");
        Reflect::Reflect<char>("Char");
        Reflect::Reflect<std::byte>("Byte");
        Reflect::Reflect<int16_t>("Short");
        Reflect::Reflect<int32_t>("Int");
        Reflect::Reflect<int64_t>("Long");
        Reflect::Reflect<uint16_t>("UShort");
        Reflect::Reflect<uint32_t>("UInt");
        Reflect::Reflect<uint64_t>("ULong");
        Reflect::Reflect<uint8_t>("UByte");

        Reflect::Reflect<glm::vec2>("Vector2");
        Reflect::Reflect<glm::vec3>("Vector3");
        Reflect::Reflect<glm::vec4>("Vector4");

        Reflect::Reflect<float>("Float")
            .AddConversion<int>();

        Reflect::Reflect<double>("Double")
            .AddConversion<int>();

        Reflect::Reflect<const char *>("cstring")
            .AddConversion<std::string>();

        // Transform
        Reflect::Reflect<TransformComponent>("Transform")
            .AddConstructor<>()
            .AddDataMember(&TransformComponent::position, "Position")
            .AddDataMember<&TransformComponent::SetRotation, &TransformComponent::GetRotation>("Rotation")
            .AddDataMember(&TransformComponent::scale, "Scale");

        Reflect::Reflect<AssetLink>("AssetLink")
            .BeginEnum<AssetType>("AssetType")
                .AddValue("Texture", AssetType::Texture)
            .EndEnum()
            .AddDataMember(&AssetLink::Handle, "Handle")
            .AddDataMember(&AssetLink::Type, "Type");

        // Sprite Renderer
        Reflect::Reflect<SpriteRenderer>("SpriteRenderer")
            .AddConstructor<>()
            .AddDataMember<&SpriteRenderer::SetSprite, &SpriteRenderer::GetSprite>("Sprite")
            .AddDataMember(&SpriteRenderer::Color, "Color")
            .AddDataMember(&SpriteRenderer::TilingFactor, "Tiling Factor");

        // Circle Renderer
        Reflect::Reflect<CircleRenderer>("CircleRenderer")
            .AddConstructor<>()
            .AddDataMember(&CircleRenderer::Color, "Color")
            .AddDataMember(&CircleRenderer::Thickness, "Thickness")
            .AddDataMember(&CircleRenderer::Fade, "Fade");

        // Box Collider2D
        Reflect::Reflect<BoxCollider2D>("BoxCollider2D")
            .AddConstructor<>()
            .AddDataMember(&BoxCollider2D::Offset, "Offset")
            .AddDataMember(&BoxCollider2D::Size, "Size")
            .AddDataMember(&BoxCollider2D::Density, "Density")
            .AddDataMember(&BoxCollider2D::Friction, "Friction")
            .AddDataMember(&BoxCollider2D::Restitution, "Restitution")
            .AddDataMember(&BoxCollider2D::RestitutionThreshold, "Restitution Threshold");

        // Circle Collider 2D
        Reflect::Reflect<CircleCollider2D>("CircleCollider2D")
            .AddConstructor<>()
            .AddDataMember(&CircleCollider2D::Offset, "Offset")
            .AddDataMember(&CircleCollider2D::Radius, "Radius")
            .AddDataMember(&CircleCollider2D::Density, "Density")
            .AddDataMember(&CircleCollider2D::Friction, "Friction")
            .AddDataMember(&CircleCollider2D::Restitution, "Restitution")
            .AddDataMember(&CircleCollider2D::RestitutionThreshold, "Restitution Threshold");

        // Rigidbody 2D
        Reflect::Reflect<Rigidbody2D>("Rigidbody2D")
            .AddConstructor<>()
            .BeginEnum<Rigidbody2D::BodyType>("BodyType")
                .AddValue("Static", Rigidbody2D::BodyType::Static)
                .AddValue("Dynamic", Rigidbody2D::BodyType::Dynamic)
                .AddValue("Kinematic", Rigidbody2D::BodyType::Kinematic)
            .EndEnum()
            .AddDataMember(&Rigidbody2D::Type, "Body Type")
            .AddDataMember(&Rigidbody2D::FixedRotation, "Fixed Rotation")
            .AddDataMember(&Rigidbody2D::GravityScale, "GravityScale");

        // Camera
        Reflect::Reflect<CameraComponent>("Camera Component")
            .AddConstructor<>()
            .AddDataMember(&CameraComponent::Primary, "Primary")
            .AddDataMember(&CameraComponent::FixedAspectRatio, "Fixed Rotation");

//        Reflect::Reflect<SceneCamera>("Camera")
//            .AddConstructor<>()
//            .AddDataMember<&SceneCamera::, &TransformComponent::GetRotation>("Primary")
//            .AddDataMember(&CameraComponent::FixedAspectRatio, "Fixed Rotation");

//        Reflect::Reflect<ScriptFieldInstance>("ScriptFieldInstance")
//            .AddConstructor<>()
//            .AddDataMember<&ScriptFieldInstance::SetValue, &ScriptFieldInstance::>("Value");

//        TransformComponent transform;
//        transform.position = glm::vec3(4, 5, 8);
//        YAML::Emitter out;
//        SerializeYAML(transform, out);
//        std::cout << out.c_str() << std::endl;

//        YAML::Node data = YAML::Load(out.c_str());
//        std::cout << Dump(data) << std::endl;
    }

//    Type Member::GetType() const
//    {
//        return m_Property.get_type();
//    }
}
