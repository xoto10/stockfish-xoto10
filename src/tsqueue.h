/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2026 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <mutex>
#include <optional>
#include <queue>

namespace Stockfish {

template<typename T>
class ThreadsafeQueue {

   public:
    ThreadsafeQueue() = default;

    ThreadsafeQueue(const ThreadsafeQueue<T> &) = delete ;

    ThreadsafeQueue &operator=(const ThreadsafeQueue<T> &) = delete ;

    ThreadsafeQueue(ThreadsafeQueue<T> &&other) noexcept(false) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_ = std::move(other.queue_);
    }

    void push(T &&item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::forward<T>(item));
    }

    void push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
    }

    // Looks at the front of the queue without removing the element.
    // Returns nullopt if queue is empty.
    std::optional<T> peek() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return {};
        }
        return queue_.front();
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return {};
        }
        T tmp = std::move(queue_.front());
        queue_.pop();
        return tmp;
    }

    unsigned long size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!empty()) {
            queue_.pop();
        }
    }

   private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;

    bool empty() const {
        return queue_.empty();
    }
};

}
