#ifndef STONE_SYNTAX_VISITOR_H
#define STONE_SYNTAX_VISITOR_H

namespace stone {
namespace syn {
class Visitor;

class Visitable {
public:
  virtual void Visit(Visitor &visitor) = 0;
};

class Visitor {
public:
};
} // namespace syn
} // namespace stone
#endif
