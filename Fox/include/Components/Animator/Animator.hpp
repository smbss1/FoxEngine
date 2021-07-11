/*
** EPITECH PROJECT, 2020
** B-YEP-400-TLS-4-1-indiestudio-thomas.marches
** File description:
** Animator.hpp
*/

#ifndef FOX_ANIMATOR_HPP_
#define FOX_ANIMATOR_HPP_

#include <unordered_map>
#include <Utils/FileSystem.hpp>
#include <Core/Logger/Logger.hpp>
#include <Utils/Graph.hpp>
#include <common.hpp>

#include "Timeline.hpp"
#include "Serialization.hpp"

struct AnimVariable;

struct Condition
{
    virtual bool IsValid() { return true; };
};

struct Transition
{
    int id;
    int from, to;
    fox::ref<Condition> condition;

    Transition() = default;
    Transition(const int id, const int f, const int t) : id(id), from(f), to(t) {}

    inline int  opposite(const int n) const { return n == from ? to : from; }
    inline bool contains(const int n) const { return n == from || n == to; }
};

enum class AnimVarType
{
    INT, FLOAT, BOOL, TRIGGER
};

struct AnimVariable
{
    std::string m_strName;
    AnimVarType type;

    union
    {
        bool trigger;
        bool boolean;
        int int_number;
        float float_number;
    } u;

    void SetBool(bool value)
    {
        switch (type) {
            case AnimVarType::BOOL:
                u.boolean = value;
                break;
            case AnimVarType::TRIGGER:
                u.trigger = value;
                break;
            default:
                fox::warn("Invalid variable type");
                break;
        }
    }

    void SetFloat(float value)
    {
        switch (type) {
            case AnimVarType::FLOAT:
                u.float_number = value;
                break;
            default:
                fox::warn("Invalid variable type");
                break;
        }
    }

    void SetInt(int value)
    {
        switch (type)
        {
            case AnimVarType::INT:
                u.int_number = value;
                break;
            default:
                fox::warn("Invalid variable type");
                break;
        }
    }

    // Getter
    [[nodiscard]] bool GetBool() const
    {
        switch (type)
        {
            case AnimVarType::BOOL:
                return u.boolean;
            case AnimVarType::TRIGGER:
                return u.trigger;
            default:
                fox::warn("Invalid variable type");
                break;
        }
        return false;
    }

    [[nodiscard]] float GetFloat() const
    {
        switch (type) {
            case AnimVarType::FLOAT:
                return u.float_number;
            default:
                fox::warn("Invalid variable type");
                break;
        }
        return 0.0f;
    }

    [[nodiscard]] int GetInt() const
    {
        switch (type)
        {
            case AnimVarType::INT:
                return u.int_number;
            default:
                fox::warn("Invalid variable type");
                break;
        }
        return 0;
    }

    void Reset()
    {
        switch (type)
        {
            case AnimVarType::INT:
                SetInt(0);
                break;
            case AnimVarType::FLOAT:
                SetFloat(0);
                break;
            case AnimVarType::BOOL:
            case AnimVarType::TRIGGER:
                SetBool(false);
                break;
        }
    }
};

struct TriggerCondition : public Condition
{
    TriggerCondition(AnimVariable& variable) : trigger(variable) {}
    bool IsValid() override { return trigger.type == AnimVarType::TRIGGER && trigger.GetBool(); };

private:
    AnimVariable& trigger;
};

class Animator
{
public:
    explicit Animator() : current(nullptr), previous(nullptr), m_oGraph() /*m_vAnimations()*/
    {
        m_bCheckTransition.add_update_event([this](basic_property<bool>* prop)
        {
            if (*prop)
                CheckTransitions();
        });

        current.add_update_event(FOX_BIND_EVENT_FN(OnAnimationChange));
    }
    ~Animator() = default;

    Animator(Animator&& other)
    : m_oGraph(std::move(other.m_oGraph)), current(other.current), previous(other.previous)
    {
    }

    Animator& operator=(Animator&& rhs)
    {
        m_oGraph = std::move(rhs.m_oGraph);
        current = rhs.current;
        previous = rhs.previous;
        return *this;
    }

    Timeline& add_anim(const std::string& name)
    {
        fox::ref<Timeline> anim = fox::new_ref<Timeline>(name);
        Timeline* ptr = anim.get();
        int anim_id = m_oGraph.insert_node(std::move(anim));
        ptr->ID = anim_id;
        m_vStringToNodeId.insert({name, anim_id});
        if (!current)
            current = ptr;
        return *ptr;
    }

    void RemoveAnim(const std::string& name)
    {
        auto it = Find(name);
        if (it)
        {
            m_oGraph.erase_node(m_vStringToNodeId[name]);
        }
    }
//
//    void switch_to(const std::string& name)
//    {
//        auto it = Find(name);
//        if (it)
//        {
//            if (current)
//                current->reset();
//            current = it;
//        }
//    }

    Timeline& add_anim_from_file(const std::string& strFilepath)
    {
        std::string out;
        if (fox::file::ReadFile(strFilepath, out))
        {
            fox::info("Parse animation file: %", strFilepath);

            fox::json::Value anim_json = fox::json::parse(out);
            std::string strName = anim_json["name"].get<std::string>();
            auto anim = anim_json.get<fox::ref<Timeline>>();
            anim->Name = strName;

            fox::info("Animation Name: %", anim_json["name"].get<std::string>());
            fox::info("Animation Tracks count: %", anim->Tracks.get().size());
            Timeline* ptr = anim.get();
            m_vStringToNodeId.insert({strName, m_oGraph.insert_node(std::move(anim))});
            // m_vAnimations.push_back(std::move(anim));
            if (!current)
                current = ptr;
            return *ptr;
        }
        else
            throw std::runtime_error("Cannot find the animation file: " + strFilepath);
    }

    void run()
    {
        current->run();
    }

    Timeline* Get(const std::string& strName)
    {
        return Find(strName);
    }

    fox::Graph<fox::ref<Timeline>, Transition>& GetGraph()
    {
        return m_oGraph;
    }

    int GetNodeIdFromName(const std::string& strName)
    {
        return m_vStringToNodeId[strName];
    }

    const std::unordered_map<std::string, int>& GetStringToNodeId() const
    {
        return m_vStringToNodeId;
    }

    void AddTransition(const fox::ref<Condition>& condition, int from_state, int to_state)
    {
        auto trans_id = m_oGraph.insert_edge(from_state, to_state);
        auto it = m_oGraph.edges().find(trans_id);
        if (it != m_oGraph.edges().end())
        {
            it->condition = condition;
            fox::info("Add Transition % -> %", from_state, to_state);
        }
    }

    AnimVariable* GetParam(const std::string& strName)
    {
        return FindVariable(strName);
    }

    void SetTrigger(const std::string& strName)
    {
        auto variable = FindVariable(strName);
        if (variable && variable->type == AnimVarType::TRIGGER)
        {
            if (!variable->GetBool()) {
                variable->SetBool(true);
                // TODO: Implement the HasExit variable to switch animation without waiting the end of the animation
                m_bCheckTransition = true;
            }
        }
        else
            fox::error("The variable '%' don't exist or is not a trigger type.", strName);
    }

    void OnAnimationChange(property<Timeline*>* pAnim)
    {
        if (previous)
            previous->IsFinish.remove_update_event(m_lOnAnimationChangeId);
        m_lOnAnimationChangeId = (*pAnim)->IsFinish.add_update_event(FOX_BIND_EVENT_FN(OnAnimationFinish));
        previous = nullptr;
        for (auto& variable : m_vVariables)
        {
            variable.Reset();
        }
    }

    void OnAnimationFinish(property<bool>* bIsFinish)
    {
        if (bIsFinish && bIsFinish->get())
            m_bCheckTransition = true;
    }

    void CheckTransitions()
    {
        for (auto& transition : m_oGraph.edges())
        {
            if (transition.from == current->ID)
            {
                if (transition.condition->IsValid() && current->ID.get() != transition.to)
                {
                    previous = current;
                    current = m_oGraph.node(transition.to).get();
                    break;
                }
            }
        }
        m_bCheckTransition = false;
    }

    void AddParam(const std::string& string, AnimVarType type)
    {
        AnimVariable variable;
        variable.type = type;
        variable.m_strName = string;
        variable.Reset();
        m_vVariables.push_back(variable);
    }

private:

    Timeline* Find(const std::string& strName)
    {
        for (auto& node : m_vStringToNodeId)
        {
            auto& anim = m_oGraph.node(node.second);
            if (anim->Name.get() == strName)
                return anim.get();
        }
        return nullptr;
    }

    AnimVariable* FindVariable(const std::string_view& strName)
    {
        for (auto& variable : m_vVariables)
        {
            if (variable.m_strName == strName)
                return &variable;
        }
        return nullptr;
    }

public:
    property<Timeline*> current;
    property<Timeline*> previous;

private:
//    AnimationVec m_vAnimations;
    std::vector<AnimVariable> m_vVariables;
    std::unordered_map<int, std::vector<Transition>> m_vTransitions;
    std::unordered_map<std::string, int> m_vStringToNodeId;
    fox::Graph<fox::ref<Timeline>, Transition> m_oGraph;

    property<bool> m_bCheckTransition{};

    // Slots id property registration event
    size_t m_lOnAnimationChangeId = -1;
};

#endif