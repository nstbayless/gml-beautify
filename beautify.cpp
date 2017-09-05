# include "parser.h"

# include <iterator>

using namespace std;

string indent(const BeautifulConfig& config, BeautifulContext context) {
  if (context.is_inline || context.condense)
    return "";
  if (config.indent_spaces)
    return std::string(config.indent_spaces_per_tab * context.depth, ' ');
  else
    return std::string(context.depth, '\t');
}

template<class P>
string join_productions(const std::vector<P*> productions, string joinder, const BeautifulConfig& config, BeautifulContext context) {
  bool first = true;
  string s = "";
  for (auto p: productions) {
    if (!first)
      s += joinder;
    s += p->beautiful(config, context);
    first = false;
  }
  return s;
}

BeautifulContext BeautifulContext::increment_depth() const {
  auto b(*this);
  b.depth++;
  return b;
}

BeautifulContext BeautifulContext::decrement_depth() const {
  auto b(*this);
  b.depth--;
  return b;
}

BeautifulContext BeautifulContext::as_condensed() const {
  auto b(*this);
  b.depth++;
  b.condense = true;
  return b;
}

BeautifulContext BeautifulContext::as_inline() const {
  auto b(*this);
  b.is_inline = true;
  return b;
}

BeautifulContext BeautifulContext::not_inline() const {
  auto b(*this);
  b.is_inline = false;
  return b;
}

BeautifulContext BeautifulContext::attach() const {
  auto b(*this);
  b.attached = true;
  return b;
}

BeautifulContext BeautifulContext::detach() const {
  auto b(*this);
  b.attached = false;
  return b;
}

string Production::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return "Unknown Production";
}

string PrDecor::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return indent(config, context) + rawToken.value;
}

string PrExprParen::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return indent(config, context) + "(" + content->beautiful(config, context.as_inline()) + ")";
}

string PrExpressionFn::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + identifier.value + "(";
  s += join_productions(args, ", ", config, context.as_inline());
  s += ")";
  return s;
}

string PrExprArithmetic::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  bool l_space = true;
  bool r_space = true;
  
  // determine whether to put spacing on side of operator
  if (!config.opr_space && op.type == OPR)
    l_space = r_space = false;
  
  if (!config.not_space && op == Token(OP,"!"))
    l_space = r_space = false;
  
  // (keywords absolutely need spaces to be parsed)
  if (op.type == KW) {
    l_space = true;
    r_space = true;
  }
  
  // beautiful string:
  string s = "";
  
  if (lhs) {
    s += lhs->beautiful(config,context.as_inline());
    if (l_space)
      s += " ";
  }
  
  s += op.value;
  
  if (rhs) {
    if (r_space)
      s += " ";
    s += rhs->beautiful(config,context.as_inline());
  }
  
  return s;
}

string PrEmptyStatement::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return "";
}

string PrFinal::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return final.value;
}

string PrIdentifier::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  return identifier.value;
}

string PrAssignment::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + lhs->beautiful(config,context.as_inline()) + " " + op.value;
  if (rhs)
    s += " " + rhs->beautiful(config,context.as_inline());
  if (!context.is_inline && config.semicolons)
    s += ";";
  return s;
}

string PrStatementFn::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s(fn->beautiful(config, context));
  if (!context.is_inline && config.semicolons)
    s += ";";
  return s;
}

string PrVarDeclaration::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = identifier.value;
  if (definition)
    s += " = " + definition->beautiful(config, context);
  return s;
}

string PrStatementVar::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + "var " + join_productions(declarations, ", ", config, context.as_inline());
  if (!context.is_inline) // var statement requires semicolon
    s += ";";
  return s;
}

string PrBody::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  if (context.attached)
    context = context.decrement_depth();
  string s = "";
  // determine indent style:
  if (config.egyptian) {
    if (!context.attached)
      s = indent(config, context);
  } else {
    if (context.attached)
      s = "\n";
    s += indent(config, context);
  }
  
  s += "{";
  
  BeautifulContext subcontext = context.increment_depth().detach();
  
  // add productions within block
  for (auto p: productions) {
    s += "\n";
    s += p->beautiful(config, subcontext);
  }
 
  s += "\n" + indent(config, context) + "}";
  return s;
}

string PrStatementIf::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = "if " + condition->beautiful(config,context.as_inline());
  if (typeid(result) == typeid(PrBody))
    context = context.attach();
  else
    s += "\n";
  s += result->beautiful(config, context.not_inline().increment_depth());
  context = context.detach();
  if (otherwise) {
    if (typeid(result) == typeid(PrBody))
      s += " ";
    else
      s += "\n";
    s += "else ";
    if (typeid(otherwise) == typeid(PrBody))
      context = context.attach();
    else
      s += "\n";
    s += otherwise->beautiful(config, context.not_inline().increment_depth());
  }
  return s;
}

string PrFor::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + "for (" + init->beautiful(config, context.as_inline());
  s += ";";
  if (condition)
    s += " " + condition->beautiful(config, context.as_inline());
  s += ";";
  if (typeid(second) != typeid(PrEmptyStatement))
    s += " ";
  s += second->beautiful(config, context.as_inline());
  s += ")";
  if (typeid(first) == typeid(PrBody))
    context = context.attach();
  else
    s += "\n";
  s += first->beautiful(config, context.increment_depth());
  return s;
}

string PrControl::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + kw.value;
  if (!context.is_inline && config.semicolons)
    s += ";";
  return s;
}

string PrWhile::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + "while (" + condition->beautiful(config, context.as_inline()) + ")";
  if (typeid(event) == typeid(PrBody))
    context = context.attach();
  s += event->beautiful(config, context.increment_depth());
  return s;
}

string PrWith::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) +"with (" + objid->beautiful(config, context.as_inline());
  s += ")";
  if (typeid(event) == typeid(PrBody))
    context = context.attach();
  else
    s += "\n";
  s += event->beautiful(config, context.increment_depth());
  return s;
}

string PrAccessorExpression::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + ds->beautiful(config, context.as_inline());
  s += "[";
  if (acc.length() > 0) {
    s += acc;
    if (config.accessor_space)
      s += " ";
  }
  s += join_productions(indices, ", ", config, context.as_inline());
  s += "]";
  return s;
}

string PrSwitch::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context) + "switch ";
  s += condition->beautiful(config, context.as_inline());
  context = context.not_inline();
  
  if (config.egyptian)
    s += " {";
  else
    s += "\n" + indent(config, context) + "}";
  
  for (auto c: cases)
    s += c->beautiful(config, context);
  
  s += indent(config, context) + "}";
  return s;
}

string PrCase::beautiful(const BeautifulConfig& config, BeautifulContext context) const {
  string s = indent(config, context);
  if (value)
    s += "case " + value->beautiful(config, context.as_inline());
  else
    s += "default";
  s += ":\n";
  for (auto p: productions) {
    s += p->beautiful(config, context.increment_depth()) + "\n";
  }
  return s;
}
