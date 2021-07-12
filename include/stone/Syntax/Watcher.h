#ifndef STONE_SYNTAX_WATCHER_H
#define STONE_SYNTAX_WATCHER_H

namespace stone {
namespace syn {

class Watcher;

class Watchable {
public:
  virtual void Watch(Watcher &watcher) = 0;
};

class Watcher final {
public:
};
} // namespace syn
} // namespace stone

#endif
