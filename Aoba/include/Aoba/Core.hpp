﻿#pragma once

#include <Siv3D.hpp>
#include <atomic>

namespace s3d::aoba {
    class Page;
    class UIComponent;
    enum class ColorMode;

    class Core final {
    private:
        std::unique_ptr<class PageManager> m_pageManager;
        std::unique_ptr<class TaskRunner> m_taskRunner;

        std::atomic<bool> m_terminationPrevented = false;

        bool m_animateColor = false;

    public:
        Core(const Core&) = delete;

        Core(Core&&) = delete;

        Core& operator=(const Core&) = delete;

        Core& operator=(Core&&) = delete;

        static Core& Instance();

        static bool IsAsyncTaskAlive();

        static bool IsTerminationPrevented();

        static bool IsTimeoutAlive(size_t id);

        static bool Start();

        static void Terminate();

        static void SwitchPage(const String& identifier);

        static void AppendIsolatedComponent(const UIComponent& component);

        static void SetColorMode(ColorMode mode);

        static void ToggleColorMode();

        // If you call this, you should call ContinueTermination() to terminate app
        static void PreventTermination();

        static void ContinueTermination();

        /// <summary>
        /// Request to run a task asynchrony, and if need, a completion process will runs on main thread.
        /// </summary>
        /// <param name="task">The process that runs asynchrony. Do not set a process that changes user
        /// interfaces.</param> <param name="completion">The process that runs on main thread after func()
        /// ended.</param>
        static void PostAsyncTask(const std::function<void()>& task,
                                  const std::function<void()>& completion = std::function<void()>());

        /// <summary>
        /// Request to run a process on main thread. In many cases, func is the process that changes user interfaces.
        /// </summary>
        /// <param name="task">The process that runs on main thread.</param>
        static void PostSyncTask(const std::function<void()>& task);

        /// <summary>
        /// Set an event with timeout. Do not set a process that changes user interfaces.
        /// </summary>
        /// <param name="func">A function that runs when timed out.</param>
        /// <param name="ms">The time to time out.</param>
        /// <param name="threading">If true, the function runs asynchronously.</param>
        /// <returns>The ID of the Timeout. ID is 1, 2, 3, ...</returns>
        static size_t SetTimeout(const std::function<void()>& func, double ms, bool threading);

        static bool StopTimeout(size_t id);

        static bool RestartTimeout(size_t id);

        template <class T>
        static T& GetPage(const String& identifier) noexcept {
            return static_cast<T&>(Instance().getPage(identifier));
        }

        template <class T>
        static void AppendPage(const String& identifier) {
            Instance().appendPage(std::shared_ptr<T>(new T(identifier)));
        }

    private:
        Core();

        ~Core();

        static void AddLicense();

        Page& getPage(const String& identifier) const noexcept;

        bool animateColor();

        void appendPage(const std::shared_ptr<Page>& page);

        void appendIsolatedComponent(const std::shared_ptr<UIComponent>& component);

        void run();

        void update();
    };
}
