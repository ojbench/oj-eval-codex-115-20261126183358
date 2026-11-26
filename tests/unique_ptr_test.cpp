#include "../src.hpp"

#include <cassert>
#include <type_traits>

struct Tracked {
  static int alive;
  int value;
  explicit Tracked(int n) : value(n) { ++alive; }
  ~Tracked() { --alive; }
};
int Tracked::alive = 0;

struct Forwarded {
  int &reference;
  sjtu::unique_ptr<int> owned;
  Forwarded(int &value, sjtu::unique_ptr<int> &&ptr)
      : reference(value), owned(std::move(ptr)) {}
};

int main() {
  static_assert(!std::is_copy_constructible<sjtu::unique_ptr<int>>::value, "");
  static_assert(!std::is_copy_assignable<sjtu::unique_ptr<int>>::value, "");
  {
    sjtu::unique_ptr<Tracked> empty;
    assert(empty.get() == nullptr);
    empty.reset();
    assert(empty.release() == nullptr);

    auto first = sjtu::make_unique<Tracked>(42);
    Tracked *address = first.get();
    assert(first->value == 42 && (*first).value == 42);
    sjtu::unique_ptr<Tracked> second(std::move(first));
    assert(first.get() == nullptr && second.get() == address);
    assert(Tracked::alive == 1);

    auto third = sjtu::make_unique<Tracked>(7);
    third = std::move(second);
    assert(second.get() == nullptr && third.get() == address);
    assert(Tracked::alive == 1);
    auto &alias = third;
    third = std::move(alias);
    assert(third.get() == address && Tracked::alive == 1);

    const auto &view = third;
    assert(view.get() == address);
    Tracked *released = third.release();
    assert(third.get() == nullptr && Tracked::alive == 1);
    delete released;
    assert(Tracked::alive == 0);

    third = sjtu::make_unique<Tracked>(9);
    third = std::move(empty);
    assert(third.get() == nullptr && Tracked::alive == 0);
    third = sjtu::make_unique<Tracked>(10);
    third.reset();
    third.reset();
    assert(Tracked::alive == 0);
    third = sjtu::make_unique<Tracked>(11);
  }
  assert(Tracked::alive == 0);
  auto zero = sjtu::make_unique<int>();
  assert(*zero == 0);
  const int value = 23;
  auto copy = sjtu::make_unique<int>(value);
  assert(*copy == value);
  int number = 8;
  auto forwarded = sjtu::make_unique<Forwarded>(number, std::move(copy));
  assert(&forwarded->reference == &number);
  assert(*forwarded->owned == value && copy.get() == nullptr);
}
