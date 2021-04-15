

#ifndef FOX_PIPELINE_HPP_
#define FOX_PIPELINE_HPP_

#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include "bitsets.hpp"
// #include "ASystem.hpp"

class ASystem;

class Pipeline
{
public:
    using SPtr = std::shared_ptr<Pipeline>;
    using UPtr = std::unique_ptr<Pipeline>;
    Pipeline();
    ~Pipeline();

    void add_phase(std::uint32_t id);
    void add_system(std::uint32_t id, ASystem* pSys);
    void remove(std::uint32_t id, ASystem* pSys);
    void run_phase(std::uint32_t id);
    bool has_phase(std::uint32_t id);

private:
    // std::vector<ASystem*> m_vSystems;
    std::vector<std::uint32_t> m_vPhasesOrder;
    std::unordered_map<std::uint32_t, std::vector<ASystem*>> m_vPhases;
    // std::vector<std::uint32_t> m_vPhasesId;
};

#endif