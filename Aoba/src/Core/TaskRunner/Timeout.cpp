﻿#include "Timeout.hpp"

#include <atomic>
#include <thread>

namespace s3d::aoba {
    std::atomic<size_t> Timeout::m_Id = 1;

    Timeout::Timeout(const std::function<void()>& func, double ms, bool threading) :
        m_id(m_Id), m_ms(ms), m_threading(threading), m_func(func), m_startedTime(std::chrono::system_clock::now()) {
        m_Id++;
    }

    void Timeout::update() {
        if (!m_started && !m_stopped) {
            const auto currentTime   = std::chrono::system_clock::now();
            const auto elapsedTime = static_cast<double>(
                std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startedTime).count());
            if (elapsedTime > m_ms) {
                m_started = true;
                if (m_threading) {
                    std::thread thread([this] {
                        m_func();
                        m_finished = true;
                    });

                    thread.detach();
                } else {
                    m_func();
                    m_finished = true;
                }
            }
        }
    }

    bool Timeout::restart() {
        if (isAlive() && !isRunning()) {
            m_startedTime = std::chrono::system_clock::now();
            return true;
        }
        return false;
    }

    bool Timeout::stop() {
        if (isAlive() && !isRunning()) {
            m_stopped = true;
            return true;
        }
        return false;
    }
}