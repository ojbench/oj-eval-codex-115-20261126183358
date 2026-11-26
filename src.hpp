#pragma once
#include <new>
#include <utility>

namespace sjtu {

template <typename T>
class unique_ptr {
 private:
  T *ptr_;

 public:
  unique_ptr() noexcept : ptr_(nullptr) {}
  explicit unique_ptr(T *ptr) noexcept : ptr_(ptr) {}
  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  unique_ptr(unique_ptr &&other) noexcept : ptr_(other.release()) {}

  unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this != &other) {
      reset();
      ptr_ = other.release();
    }
    return *this;
  }

  ~unique_ptr() { delete ptr_; }

  void reset() noexcept { delete release(); }

  T *release() noexcept {
    T *result = ptr_;
    ptr_ = nullptr;
    return result;
  }

  T *get() const noexcept { return ptr_; }
  T &operator*() noexcept { return *ptr_; }
  T *operator->() noexcept { return ptr_; }
};

static_assert(sizeof(unique_ptr<int>) <= sizeof(void *),
              "unique_ptr must store at most one pointer");

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&...args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace sjtu
