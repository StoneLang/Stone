#ifndef STONE_SYNTAX_WALKER_H
#define STONE_SYNTAX_WALKER_H

namespace stone {
namespace syn {

class Walker;

class Walkable {
public:
  virtual bool Walk(Walker &waker) = 0;
};
class Walker {
public:
};
} // namespace syn
} // namespace stone
#endif
