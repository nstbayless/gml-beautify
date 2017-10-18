# include "parser.h"
# include "util.h"

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
string join_productions(const std::vector<P*> productions, string joinder, const BeautifulConfig& config, BeautifulContext context, Production* infix_source = nullptr) {
  bool first = true;
  string s = "";
  context.pad_infix_right = true;
  if (infix_source)
    s +=infix_source->renderWS(config, context);
  for (auto p: productions) {
    if (!first)
      s += joinder;
    if (infix_source)
      s += infix_source->renderWS(config, context);
    s += p->beautiful(config, context);
    first = false;
    if (infix_source)
      s += infix_source->renderWS(config, context);
  }
  return s;
}

template<class Base, class Any>
bool is_a(Any* ptr) {
  return !! dynamic_cast<Base*>(ptr);
}

bool hangable(Production* p) {
  return is_a<PrBody>(p) || is_a<PrEmptyStatement>(p);
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

BeautifulContext BeautifulContext::force_semicolon() const {
  auto b(*this);
  b.forced_semicolon = true;
  return b;
}

BeautifulContext BeautifulContext::style(InfixStyle s) const {
  auto b(*this);
  if (s == PAD_LEFT) {
    b.pad_infix_left = true;
  } else if (s == PAD_RIGHT) {
    b.pad_infix_right = true;
  } else if (s == PAD_BOTH) {
    b.pad_infix_left = b.pad_infix_right = true;
  } else if (s == PAD_NEITHER) {
    b.pad_infix_left = b.pad_infix_right = false;
  } else {
    b.infix_style = s;
  }
  return b;
}

string Production::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return "Unknown Production";
}

string Production::renderWS(const BeautifulConfig& config, BeautifulContext context) {
  if (infixes.empty())
    return "";
  PrInfixWS* ws = infixes.front();
  infixes.pop_front();
  if (!ws)
    return "";
  s = ws->beautiful(config, context);
  if (ws->val.type == COMMENT && context.infix_style <= TWO_LINES && !context.is_inline) {
    //single-line comment
    if (ws->val.value[0] == '/' && ws->val.value[1] == '/') {
      s += "\n" + indent(config, context.increment_depth().not_inline());
    }
  }
  delete(ws);
  if context.EOL_NO_CONVERT {
    s = trim(s);
  }
  return s;
}

string PrStatement::end_statement_beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s;
  if ((config.semicolons &&! context.non_statement) || context.forced_semicolon) {
    s += ";";
  }
  while (!infixes.empty()) {
    s += renderWS(config, context.style(AS_IS));
  }
  return s;
}

string PrDecor::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return indent(config, context) + rawToken.value;
}

string PrExprParen::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return indent(config, context) + "(" + content->beautiful(config, context.as_inline()) + ")";
}

string PrExpressionFn::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + identifier.value + renderWS(config, context) + "(";
  s += join_productions(args, ", ", config, context.as_inline(), this);
  s += ")";
  return s;
}

string PrExprArithmetic::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  bool l_space = true;
  bool r_space = true;
  
  // determine whether to put spacing on side of operator
  if (!config.opr_space && op.type == OPR)
    l_space = r_space = false;
  
  if (!config.not_space && op == Token(OP,"!"))
    l_space = r_space = false;
  
  if (op == Token(PUNC,"."))
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

string PrEmptyStatement::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  context.non_statement = true;
  if (context.attached)
    return end_statement_beautiful(config, context.force_semicolon());
  return end_statement_beautiful(config, context);
}

string PrFinal::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return final.value + renderWS(config, context);
}

string PrIdentifier::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return identifier.value + renderWS(config, context);
}

string PrAssignment::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + lhs->beautiful(config,context.as_inline());
  if (op.type != OPR || config.opr_space)
    s += " ";
  s += op.value;
  s += renderWS(config, context.style(PAD_LEFT));
  if (rhs)
    s += " " + rhs->beautiful(config,context.as_inline());
  s += end_statement_beautiful(config, context);
  return s;
}

string PrStatementFn::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s(fn->beautiful(config, context));
  s += end_statement_beautiful(config, context);
  return s;
}

string PrVarDeclaration::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = identifier.value;
  if (definition)
    s += " = " + definition->beautiful(config, context);
  return s;
}

string PrStatementVar::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "var " + join_productions(declarations, ", ", config, context.as_inline());
  s += end_statement_beautiful(config, context.force_semicolon());
  return s;
}

string PrBody::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  if (context.attached)
    context = context.decrement_depth();
  string s = "";
  // determine indent style:
  if (config.egyptian) {
    if (!context.attached)
      s = indent(config, context);
    else
      s = " ";
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
  if (productions.size() > 0)
    s += "\n" + indent(config, context);
  else
    s += " ";
  s += "}";
  return s;
}

string PrStatementIf::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = "";
  if (!context.attached)
    s += indent(config, context);
  else
    context = context.decrement_depth();
  s += "if " + renderWS(config, context) + condition->beautiful(config,context.as_inline().style(SL_NO_CONVERT));
  context = context.attach();
  s += result->beautiful(config, context.not_inline().increment_depth());
  context = context.detach();
  if (otherwise) {
    if (hangable(result) && config.egyptian)
      s += " ";
    else
      s += "\n" + indent(config, context);
    s += "else";
    if (hangable(otherwise) || is_a<PrStatementIf>(otherwise)) {
      context = context.attach();
      s += " ";
    } else {
      s += "\n";
    }
    s += otherwise->beautiful(config, context.not_inline().increment_depth());
  }
  return s;
}

string PrFor::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "for (" + init->beautiful(config, context.as_inline());
  s += ";";
  if (condition)
    if (init)
      if (!is_a<PrEmptyStatement>(init))
        s += " ";
  if (condition)
    s += condition->beautiful(config, context.as_inline());
  s += ";";
  if (second)
    if (!is_a<PrEmptyStatement>(second))
      s += " ";
  s += second->beautiful(config, context.as_inline());
  s += ")";
  if (hangable(first))
    context = context.attach();
  else
    s += "\n";
  s += first->beautiful(config, context.increment_depth());
  return s;
}

string PrControl::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + kw.value;
  if (val) s += " " + val->beautiful(config, context.as_inline());
  s += end_statement_beautiful(config, context);
  return s;
}

string PrWhile::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "while (" + condition->beautiful(config, context.as_inline()) + ")";
  if (hangable(event))
    context = context.attach();
  else
    s += "\n";
  s += event->beautiful(config, context.increment_depth());
  return s;
}

string PrWith::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) +"with (" + objid->beautiful(config, context.as_inline());
  s += ")";
  if (hangable(event))
    context = context.attach();
  else
    s += "\n";
  s += event->beautiful(config, context.increment_depth());
  return s;
}

string PrAccessorExpression::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + ds->beautiful(config, context.as_inline());
  s += "[" + renderWS(config, context.style(PAD_RIGHT));
  if (acc.length() > 0) {
    s += acc;
    if (config.accessor_space)
      s += " ";
  }
  s += join_productions(indices, ", ", config, context.as_inline(), this);
  s += "]";
  return s;
}

string PrSwitch::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "switch ";
  s += condition->beautiful(config, context.as_inline());
  
  if (cases.size() == 0 && !config.egyptian) {
    return s + "\n{ }\n";
  }
  
  context = context.not_inline();
  
  if (config.egyptian)
    s += " {\n";
  else
    s += "\n" + indent(config, context) + "{\n";
  
  for (auto c: cases)
    s += c->beautiful(config, context);
  
  s += indent(config, context) + "}";
  return s;
}

string PrCase::beautiful(const BeautifulConfig& config, BeautifulContext context) {
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

string PrInfixWS::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = "";
  
  // pad left
  if (context.pad_infix_left && val.type == COMMENT)
    s += " ";
  
  s += val.value + renderWS(config, context);
  
  if (context.pad_infix_right && val.type == COMMENT && val.value[1] == "*")
    s += " ";
  return s;
}
