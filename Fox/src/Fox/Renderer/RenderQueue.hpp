#pragma once

#include <algorithm>
#include <cstdint>
#include <functional> 
#include <vector>
#include <iostream>

namespace fox
{
//    struct CommandPacket
//    {
//        typedef void(*function_t)(const void* command);
//
//        template <class CommandClass>
//        struct is_valid_type {
//            enum {
//                value = !std::is_same<CommandPacket, CommandClass>::value &&
//                !std::is_void<CommandClass>::value&&
//                std::is_pod<CommandClass>::value
//            };
//        };
//
//        template <class CommandClass>
//        static CommandPacket* create() {
//            static_assert(is_valid_type<CommandClass>::value, "CommandClass must be a valid POD type and not CommandPacket itself.");
//
//            uint8_t* data = new uint8_t[sizeof(CommandPacket) + sizeof(CommandClass)];
//
//            CommandPacket* packet = reinterpret_cast<CommandPacket*>(data);
//            packet->commandData = data + sizeof(CommandPacket);
//            //packet->auxilaryData = data + sizeof(CommandPacket) + sizeof(CommandClass);
//            packet->nextCommand = nullptr;
//
////#ifndef NDEBUG
////            if (auxilarySize == 0)
////                packet->auxilaryData = nullptr;
////#endif
//
//            return packet;
//        }
//
//        template <class CommandClass>
//        static CommandPacket* getCommandPacket(CommandClass* commandData) {
//            static_assert(is_valid_type<CommandClass>::value, "CommandClass must be a valid POD type and not CommandPacket itself.");
//            return reinterpret_cast<CommandPacket*>(reinterpret_cast<uint8_t*>(commandData) - sizeof(CommandPacket));
//        }
//
//        template <class CommandClass>
//        static const CommandPacket* getCommandPacket(const CommandClass* commandData) {
//            static_assert(is_valid_type<CommandClass>::value, "CommandClass must be a valid POD type and not CommandPacket itself.");
//            return reinterpret_cast<const CommandPacket*>(reinterpret_cast<const uint8_t*>(commandData) - sizeof(CommandPacket));
//        }
//
//        template <class CommandClass>
//        static CommandClass* getCommandData(CommandPacket* packet) {
//            static_assert(is_valid_type<CommandClass>::value, "CommandClass must be a valid POD type and not CommandPacket itself.");
//            return reinterpret_cast<CommandClass*>(packet->commandData);
//        }
//
//        static void dispatch(const CommandPacket* packet) {
//            do
//            {
//                (*packet->dispatchFunction)(packet->commandData);
//                packet = packet->nextCommand;
//            } while (packet != NULL);
//        }
//
//        function_t dispatchFunction;
//        void* commandData;
//        CommandPacket* nextCommand;
//    };

    struct CommandPacket
    {
        typedef void(*function_t)(const void* command);
        function_t dispatchFunction = nullptr;
        void* commandData = nullptr;
        size_t commandSize;
        CommandPacket* nextCommand = nullptr;

        static void dispatch(const CommandPacket* packet)
        {
            while (packet != nullptr)
            {
                (*packet->dispatchFunction)(packet->commandData);
                packet = packet->nextCommand;
            }
        }
    };

    struct CommandPair
    {
        CommandPacket* cmd = nullptr;

        friend bool operator<(const CommandPair& l, const CommandPair& r)
        {
            return false; // keep the same order
        }
    };

    class RenderCommandQueue
    {
        typedef std::function<void(CommandPair*, CommandPair*)> sort_func_t;

        template <class CommandClass>
        struct is_valid_type {
            enum {
                value = !std::is_same<CommandPacket, CommandClass>::value &&
                !std::is_void<CommandClass>::value&&
                std::is_pod<CommandClass>::value
            };
        };

    public:

        RenderCommandQueue();
        ~RenderCommandQueue();

        void Clear();
        void Destroy();

        template<typename T>
        T* Push()
        {
            static_assert(is_valid_type<T>::value, "CommandClass must be a valid POD type and not CommandPacket itself.");

            // Ensure alignment for the CommandPacket
            m_CommandBufferPtr = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(m_CommandBufferPtr) + alignof(CommandPacket) - 1) & ~(alignof(CommandPacket) - 1));

            // Allocate space for CommandPacket and move the pointer
            CommandPacket* packet = reinterpret_cast<CommandPacket*>(m_CommandBufferPtr);
            m_CommandBufferPtr += sizeof(CommandPacket);

            // Ensure alignment for the command data
            m_CommandBufferPtr = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(m_CommandBufferPtr) + alignof(T) - 1) & ~(alignof(T) - 1));

            // Allocate space for the command data and move the pointer
            void* memory = m_CommandBufferPtr;
            m_CommandBufferPtr += sizeof(T);

            // Initialize the command data using placement new
            T* commandData = new (memory) T(/* constructor arguments for T */);

            // Setup the command packet
            packet->dispatchFunction = T::Execute;
            packet->commandData = commandData;
            packet->commandSize = sizeof(T);
            packet->nextCommand = nullptr; // Assuming you will link it somehow later

            /*CommandPair& pair = m_Commands[m_CommandCount];
            pair.cmd = packet;*/

            // Increase command count, manage the linked list, etc.
            m_CommandCount++;

            return commandData;
        }

        void sort(sort_func_t sortFunc = std::sort<CommandPair*>)
        {
            sortFunc(m_Commands.data(), m_Commands.data() + (int)m_CommandCount);
        }

        //template<typename T, typename... Args>
        //void Push(Args&&... args)
        //{
        //    auto renderCmd = [](void* ptr) {
        //        auto pFunc = (T*)ptr;
        //        T::Execute(ptr);

        //        // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
        //        // however some items like uniforms which contain std::strings still exist for now
        //        // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
        //        //pFunc->~T();
        //    };
        //    auto storageBuffer = Push(renderCmd, sizeof(T));
        //    new (storageBuffer) T(std::forward<Args>(args)...);
        //}

        //template<typename T>
        //void Push(T&& cmd)
        //{
        //    auto renderCmd = [](void* ptr) {
        //        /*auto pFunc = (T*)ptr;
        //        pFunc->Execute();*/

        //        auto pFunc = (T*)ptr;
        //        T::Execute();

        //        // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
        //        // however some items like uniforms which contain std::strings still exist for now
        //        // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
        //        //pFunc->~T();
        //    };
        //    auto storageBuffer = Push(renderCmd, sizeof(T));
        //    new (storageBuffer) T(std::move(cmd));
        //}

        void Execute();
    private:
        uint8_t* m_CommandBuffer = nullptr;
        uint8_t* m_CommandBufferPtr = nullptr;
        uint32_t m_CommandCount = 0;

        std::vector<CommandPair> m_Commands;
    };

	//class RenderQueue
	//{
 //       typedef CommandPacket command_t;
 //       typedef std::function<void(command_t*, command_t*)> sort_func_t;
 //       //typedef MaterialBinderClass binder_t;
 //       //typedef KeyDecoderClass decoder_t;
 //       //typedef int(*log_function_t)(const char* fmt, ...);

 //   public:
 //       explicit RenderQueue(uint32_t commandCount = 5000, uint32_t commandKBytes = 512);
 //       //explicit RenderQueue(const MaterialBinderClass& materialBinder);

 //       template <class CommandClass>
 //       CommandClass* addCommand()
 //       {

 //           return 
 //       }

 //       /////@tparam AuxilaryData must be POD
 //       /////@note Also copies data from the given auxiliary data.
 //       //template <class CommandClass, typename AuxilaryData>
 //       //CommandClass* addCommandData(const key_t& key, const AuxilaryData& data);
 //       //template <class CommandClass, typename AuxilaryData>
 //       //CommandClass* addCommandData(const key_t& key, const std::vector<AuxilaryData>& data);
 //       //template <class CommandClass, typename AuxilaryData>
 //       //CommandClass* addCommandData(const key_t& key, const AuxilaryData* data, uint32_t count);

 //   private:
 //       //MaterialBinderClass             m_materialBinder;
 //       std::vector<command_t>          m_commands;
 //       uint32_t           m_currentIndex;
 //       //std::atomic<uint32_t>           m_currentIndex;
	//};

 //   #define COMMAND_TEMPLATE template <typename KeyType, class KeyDecoderClass, class MaterialBinderClass>
 //   #define COMMAND_QUAL CommandBuffer<KeyType, KeyDecoderClass, MaterialBinderClass>
}