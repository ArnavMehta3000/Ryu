#pragma once
#include "Core/Common/Concepts.h"
#include <memory>
#include <mutex>

namespace Ryu::Utils
{
    struct ConstructableAccess
    {
        template<typename T, typename... Args>
        static constexpr bool HasOnConstructImpl()
        {
            return requires(T * obj, Args&&... args)
            {
                obj->OnConstruct(std::forward<Args>(args)...);
            };
        }

        template<typename T>
        static constexpr bool HasParameterlessOnConstructImpl()
        {
            return requires(T * obj)
            {
                obj->OnConstruct();
            };
        }

        template<typename T, typename... Args>
        static constexpr bool HasOnDestructImpl()
        {
            return requires(T * obj, Args&&... args)
            {
                obj->OnDestruct(std::forward<Args>(args)...);
            };
        }

        template<typename T>
        static constexpr bool HasParameterlessOnDestructImpl()
        {
            return requires(T * obj)
            {
                obj->OnDestruct();
            };
        }

        template<typename T, typename... Args>
        static void InvokeOnConstructImpl(T* obj, Args&&... args)
        {
            if (obj) obj->OnConstruct(std::forward<Args>(args)...);
        }

        template<typename T>
        static void InvokeOnConstructImpl(T* obj)
        {
            if (obj) obj->OnConstruct();
        }

        template<typename T, typename... Args>
        static void InvokeOnDestructImpl(T* obj, Args&&... args)
        {
            if (obj) obj->OnDestruct(std::forward<Args>(args)...);
        }

        template<typename T>
        static void InvokeOnDestructImpl(T* obj)
        {
            if (obj) obj->OnDestruct();
        }
    };

    template <typename T, typename... Args>
    concept HasOnConstruct = ConstructableAccess::HasOnConstructImpl<T, Args...>();

    template<typename T>
    concept HasParameterlessOnConstruct = ConstructableAccess::HasParameterlessOnConstructImpl<T>();

    template <typename T, typename... Args>
    concept HasOnDestruct = ConstructableAccess::HasOnDestructImpl<T, Args...>();

    template<typename T>
    concept HasParameterlessOnDestruct = ConstructableAccess::HasParameterlessOnDestructImpl<T>();

    template<typename T>
        requires HasParameterlessOnConstruct<T>
    void InvokeOnConstruct(T* obj)
    {
        ConstructableAccess::InvokeOnConstructImpl(obj);
    }

    template<typename T, typename... OnConstructArgs>
        requires HasOnConstruct<T, OnConstructArgs...>
    void InvokeOnConstruct(T* obj, OnConstructArgs&&... args)
    {
        ConstructableAccess::InvokeOnConstructImpl(obj, std::forward<OnConstructArgs>(args)...);
    }

    template<typename T>
        requires HasParameterlessOnDestruct<T>
    void InvokeOnDestruct(T* obj)
    {
        ConstructableAccess::InvokeOnDestructImpl(obj);
    }

    template<typename T, typename... OnDestructArgs>
        requires HasOnDestruct<T, OnDestructArgs...>
    void InvokeOnDestruct(T* obj, OnDestructArgs&&... args)
    {
        ConstructableAccess::InvokeOnDestructImpl(obj, std::forward<OnDestructArgs>(args)...);
    }

    // Base Constructable class
    template <typename Derived, typename... CompulsoryInitArgs>
    class Constructable
    {
    public:
        Constructable() = default;

        // Constructor that accepts compulsory args - marks as initialized
        explicit Constructable(CompulsoryInitArgs... compulsoryArgs)
            : m_compulsoryArgs(std::forward<CompulsoryInitArgs>(compulsoryArgs)...)
        {
            std::call_once(m_initFlag, []() {});
        }

        virtual ~Constructable() = default;

        // Initialize with OnConstruct arguments (compulsory args + additional args)
        template<typename... OnConstructArgs>
            requires HasOnConstruct<Derived, CompulsoryInitArgs..., OnConstructArgs...>
        Derived& Initialize(CompulsoryInitArgs... compulsoryArgs, OnConstructArgs&&... args)
        {
            std::call_once(m_initFlag, [&]()
            {
                m_compulsoryArgs = std::make_tuple(compulsoryArgs...);
                InvokeOnConstruct(static_cast<Derived*>(this), compulsoryArgs..., std::forward<OnConstructArgs>(args)...);
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize with OnConstruct that takes additional args but not compulsory args
        template<typename... OnConstructArgs>
            requires HasOnConstruct<Derived, OnConstructArgs...>
            && (!HasOnConstruct<Derived, CompulsoryInitArgs..., OnConstructArgs...>)
            && (sizeof...(OnConstructArgs) > 0)
            Derived& Initialize(CompulsoryInitArgs... compulsoryArgs, OnConstructArgs&&... args)
        {
            std::call_once(m_initFlag, [&]()
            {
                m_compulsoryArgs = std::make_tuple(compulsoryArgs...);
                InvokeOnConstruct(static_cast<Derived*>(this), std::forward<OnConstructArgs>(args)...);
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize with parameterless OnConstruct (only compulsory args)
        Derived& Initialize(CompulsoryInitArgs... compulsoryArgs)
            requires HasOnConstruct<Derived, CompulsoryInitArgs...>
            && (sizeof...(CompulsoryInitArgs) > 0)
        {
            std::call_once(m_initFlag, [&]()
            {
                m_compulsoryArgs = std::make_tuple(compulsoryArgs...);
                InvokeOnConstruct(static_cast<Derived*>(this), compulsoryArgs...);
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize with parameterless OnConstruct (ignoring compulsory args)
        Derived& Initialize(CompulsoryInitArgs... compulsoryArgs)
            requires HasParameterlessOnConstruct<Derived>
            && (!HasOnConstruct<Derived, CompulsoryInitArgs...>)
            && (sizeof...(CompulsoryInitArgs) > 0)
        {
            std::call_once(m_initFlag, [&]()
            {
                m_compulsoryArgs = std::make_tuple(compulsoryArgs...);
                InvokeOnConstruct(static_cast<Derived*>(this));
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize without OnConstruct (only compulsory args)
        Derived& Initialize(CompulsoryInitArgs... compulsoryArgs)
            requires (!HasOnConstruct<Derived, CompulsoryInitArgs...>) && (sizeof...(CompulsoryInitArgs) > 0)
        {
            std::call_once(m_initFlag, [&]()
            {
                m_compulsoryArgs = std::make_tuple(compulsoryArgs...);
            });
            return static_cast<Derived&>(*this);
        }

        // Destroy with OnDestruct arguments
        template<typename... OnDestructArgs>
            requires HasOnDestruct<Derived, OnDestructArgs...>
        void Destroy(OnDestructArgs&&... args)
        {
            std::call_once(m_destroyFlag, [&]()
            {
                InvokeOnDestruct(static_cast<Derived*>(this), std::forward<OnDestructArgs>(args)...);
            });
        }

        // Destroy with parameterless OnDestruct
        void Destroy()
            requires HasParameterlessOnDestruct<Derived>
        {
            std::call_once(m_destroyFlag, [&]()
            {
                InvokeOnDestruct(static_cast<Derived*>(this));
            });
        }

        // Destroy without OnDestruct
        void Destroy()
            requires (!HasParameterlessOnDestruct<Derived>)
        {
            std::call_once(m_destroyFlag, [&]() {});
        }

        // Get the stored compulsory arguments
        [[nodiscard]] inline const std::tuple<CompulsoryInitArgs...>& GetCompulsoryArgs() const noexcept
        {
            return m_compulsoryArgs;
        }

        // Check if the object has been initialized
        bool IsInitialized() const noexcept
        {
            return m_initFlag._Opaque;
        }

    private:
        std::once_flag m_destroyFlag;
        std::once_flag m_initFlag;
        std::tuple<CompulsoryInitArgs...> m_compulsoryArgs;
    };

    // Specialization for no compulsory arguments
    template <typename Derived>
    class Constructable<Derived>
    {
    public:
        Constructable() = default;

        // Initialize with OnConstruct arguments
        template<typename... OnConstructArgs>
            requires HasOnConstruct<Derived, OnConstructArgs...>
        Derived& Initialize(OnConstructArgs&&... args)
        {
            std::call_once(m_initFlag, [&]()
            {
                InvokeOnConstruct(static_cast<Derived*>(this), std::forward<OnConstructArgs>(args)...);
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize with parameterless OnConstruct
        Derived& Initialize()
            requires HasParameterlessOnConstruct<Derived>
        {
            std::call_once(m_initFlag, [&]()
            {
                InvokeOnConstruct(static_cast<Derived*>(this));
            });

            return static_cast<Derived&>(*this);
        }

        // Initialize without OnConstruct
        Derived& Initialize()
            requires (!HasParameterlessOnConstruct<Derived>)
        {
            std::call_once(m_initFlag, [&]() {});
            return static_cast<Derived&>(*this);
        }

        // Destroy with OnDestruct arguments
        template<typename... OnDestructArgs>
            requires HasOnDestruct<Derived, OnDestructArgs...>
        void Destroy(OnDestructArgs&&... args)
        {
            std::call_once(m_destroyFlag, [&]()
            {
                InvokeOnDestruct(static_cast<Derived*>(this), std::forward<OnDestructArgs>(args)...);
            });
        }

        // Destroy with parameterless OnDestruct
        void Destroy()
            requires HasParameterlessOnDestruct<Derived>
        {
            std::call_once(m_destroyFlag, [&]()
            {
                InvokeOnDestruct(static_cast<Derived*>(this));
            });
        }

        // Destroy without OnDestruct
        void Destroy()
            requires (!HasParameterlessOnDestruct<Derived>)
        {
            std::call_once(m_destroyFlag, [&]() {});
        }

        // Check if the object has been initialized
        bool IsInitialized() const noexcept
        {
            return m_initFlag._Test();
        }

    private:
        std::once_flag m_destroyFlag;
        std::once_flag m_initFlag;
    };
}

#define RYU_CONSTRUCTABLE_FRIEND friend struct ::Ryu::Utils::ConstructableAccess
