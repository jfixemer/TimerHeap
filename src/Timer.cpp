#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
#include <thread>
#include "remove_heap.hpp"

#include <iostream>

class TimerHeap
{
    struct node;

public:
    using clock = std::chrono::steady_clock;
    using duration = clock::duration;
    using timepoint = clock::time_point;
    using timer_id = uint32_t;
    static const timer_id invalid = 0;

    // return a new duration to reschedule the same callback and tiemr_id for later.
    using callback_return = std::optional<duration>;
    using callback = std::function<callback_return(timer_id, timepoint)>;

    static bool timer_heap_compare(const node& lhs, const node& rhs)
    {
        return rhs.timeout < lhs.timeout;
    }

    TimerHeap()
        : id_counter(0)
    {
    }

    timer_id insert(duration from_now, callback action)
    {
        auto tmo = clock::now() + from_now;

        auto node_id = ++id_counter;
        if (node_id == invalid) // uint32 rollover.
            node_id = ++id_counter;

        _storage.emplace_back(node_id, action, tmo);
        std::push_heap(_storage.begin(), _storage.end(), timer_heap_compare);

        return node_id;
    }

    std::optional<duration> get_next_timeout()
    {
        if (_storage.empty())
            return {};
        auto now = clock::now();
        if (_storage.front().timeout < now)
            return std::chrono::milliseconds::zero();
        return _storage.front().timeout - now;
    }

    int call_timeouts()
    {
        int dispatch_count = 0;
        auto now = clock::now();
        while (!_storage.empty() && _storage.front().timeout <= now)
        {
            ++dispatch_count;
            std::pop_heap(_storage.begin(), _storage.end(), timer_heap_compare);

            // copy the node out of the vector so that scheduling new timers within the callback
            // works correctly.
            node dispatching = _storage.back();
            _storage.pop_back();

            auto result = dispatching.action(dispatching.id, dispatching.timeout);
            if (result.has_value())
            {
                // minimum recurrence delay of 5 milliseconds.
                if (result.value() < std::chrono::milliseconds(5))
                    dispatching.timeout += std::chrono::milliseconds(5);
                else
                    dispatching.timeout += result.value();

                // Move the node back into the heap. (same timer_id and callback, new timeout)
                _storage.push_back(std::move(dispatching));
                std::push_heap(_storage.begin(), _storage.end(), timer_heap_compare);
            }
        }
        return dispatch_count;
    }

    bool remove_timer(timer_id id)
    {
        auto pos = std::find_if(
            _storage.begin(), _storage.end(), [id](const node& elem) { return elem.id == id; });

        if (pos == _storage.end())
            return false;

        std::remove_heap(_storage.begin(), _storage.end(), pos, timer_heap_compare);
        _storage.pop_back();

        return true;
    }

private:
    struct node
    {
        timer_id id;
        callback action;
        timepoint timeout;
        node(timer_id i, callback a, timepoint t)
            : id(i)
            , action(a)
            , timeout(t)
        {
        }
    };
    std::vector<node> _storage;
    timer_id id_counter;
};

void rescheduler(TimerHeap& timer, TimerHeap::timepoint& startup)
{
    timer.insert(std::chrono::milliseconds(500),
        [&timer, &startup](TimerHeap::timer_id id, TimerHeap::timepoint tp) -> TimerHeap::callback_return {
            auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(tp - startup).count();
            std::cout << "The Rescheduled 500ms Timeout with ID: " << id << ", at (us): " << ticks << "\n";

            rescheduler(timer, startup);
            return {};
        });
}

int main(int argc, char** argv)
{
    TimerHeap timer;
    TimerHeap::timepoint startup = TimerHeap::clock::now();

    timer.insert(std::chrono::milliseconds(500),
        [startup](TimerHeap::timer_id id, TimerHeap::timepoint tp) -> TimerHeap::callback_return {
            auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(tp - startup).count();
            std::cout << "The 500ms Timeout with ID: " << id << " at (us): " << ticks << "\n";
            return std::chrono::milliseconds(500);
        });

    rescheduler(timer, startup);

    timer.insert(std::chrono::milliseconds(500),
        [](TimerHeap::timer_id id, TimerHeap::timepoint) -> TimerHeap::callback_return {
            std::cout << "The Oneshot 500ms Timeout with ID: " << id << "\n";
            return {};
        });

    timer.insert(std::chrono::milliseconds(500),
        [](TimerHeap::timer_id id, TimerHeap::timepoint) -> TimerHeap::callback_return {
            std::cout << "The 500ms Timeout with ID: " << id << "\n";
            return std::chrono::milliseconds(500);
        });
    timer.insert(std::chrono::milliseconds(500),
        [](TimerHeap::timer_id id, TimerHeap::timepoint) -> TimerHeap::callback_return {
            std::cout << "The 500ms Timeout with ID: " << id << "\n";
            return std::chrono::milliseconds(500);
        });
    timer.insert(std::chrono::milliseconds(500),
        [](TimerHeap::timer_id id, TimerHeap::timepoint) -> TimerHeap::callback_return {
            std::cout << "The 500ms Timeout with ID: " << id << "\n";
            return std::chrono::milliseconds(500);
        });

    timer.insert(std::chrono::milliseconds(2321),
        [](TimerHeap::timer_id id, TimerHeap::timepoint) -> TimerHeap::callback_return {
            std::cout << "The 2321ms Timeout with ID: " << id << "\n";
            return std::chrono::milliseconds(2321);
        });

    timer.remove_timer(5);

    auto timeout = timer.get_next_timeout();
    while (timeout.has_value())
    {
        auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(timeout.value()).count();
        std::cout << "Sleeping for " << ticks << "(us)\n";
        std::this_thread::sleep_for(timeout.value());
        auto start_proc = std::chrono::high_resolution_clock::now();
        timer.call_timeouts();
        timeout = timer.get_next_timeout();
        auto proc_time = std::chrono::high_resolution_clock::now() - start_proc;
        auto proc_time_ticks = std::chrono::duration_cast<std::chrono::microseconds>(proc_time).count();
        std::cout << "ran for :" << proc_time_ticks << '\n';
    }

    return 0;
}