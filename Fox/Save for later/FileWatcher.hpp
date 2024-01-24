//
// Created by samuel on 08/07/2021.
//

#ifndef FOXENGINE_FILEWATCHER_HPP
#define FOXENGINE_FILEWATCHER_HPP


#include "Core/Base.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <atomic>

#include "common.hpp"

namespace fox
{
// Define available file changes
    enum class FileStatus
    {
        created, modified, erased
    };

    class FileWatcher
    {
        using PathMap = std::unordered_map<std::string, fs::file_time_type>;
        using WatchFunc = std::function<void(std::string, FileStatus)>;
    public:

        // Keep a record of files from the base directory and their last modification time
        FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay)
                : path_to_watch(path_to_watch), delay(delay)
        {
            for (auto &file : fs::recursive_directory_iterator(path_to_watch)) {
                m_vPaths[file.path().string()] = fs::last_write_time(file);
            }
        }

        ~FileWatcher()
        {
            m_bRunning = false;
            if (m_pThread->joinable())
                m_pThread->join();
            m_pThread.reset();
        }

        // Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
        void start(const WatchFunc &action)
        {
            m_oCallback = action;
            m_pThread = new_scope<std::thread>([this]() {
               while (m_bRunning)
               {
                   // Wait for "delay" milliseconds
                   std::this_thread::sleep_for(delay);

                   auto it = m_vPaths.begin();
                   while (it != m_vPaths.end()) {
                       if (!fs::exists(it->first))
                       {
                           m_oCallback(it->first, FileStatus::erased);
                           it = m_vPaths.erase(it);
                       } else {
                           it++;
                       }
                   }

                   // Check if a file was created or modified
                   for (auto &file : fs::recursive_directory_iterator(path_to_watch)) {
                       auto current_file_last_write_time = fs::last_write_time(file);

                       // File creation
                       if (!contains(file.path().string())) {
                           m_vPaths[file.path().string()] = current_file_last_write_time;
                           m_oCallback(file.path().string(), FileStatus::created);
                           // File modification
                       } else {
                           if (m_vPaths[file.path().string()] != current_file_last_write_time) {
                               m_vPaths[file.path().string()] = current_file_last_write_time;
                               m_oCallback(file.path().string(), FileStatus::modified);
                           }
                       }
                   }
               }
           });
        }

    private:
        std::atomic<bool> m_bRunning = true;
        std::string path_to_watch;
        // Time interval at which we check the base folder for changes
        std::chrono::duration<int, std::milli> delay;

        scope<std::thread> m_pThread;

        // Save the callback to use it in the thread
        WatchFunc m_oCallback;

        PathMap m_vPaths;

        // Check if "m_vPaths" contains a given key
        // If your compiler supports C++20 use paths_.contains(key) instead of this function
        bool contains(const std::string &key)
        {
            return m_vPaths.find(key) != m_vPaths.end();
        }
    };
}

#endif //FOXENGINE_FILEWATCHER_HPP
