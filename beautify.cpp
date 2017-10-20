# include "parser.h"
# include "util.h"

# include <iterator>

using namespace std;

string indent(const BeautifulConfig& config, BeautifulContext context) {
  if (context.is_inline)
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

BeautifulContext BeautifulContext::as_eol() const {
  auto b(*this);
  b.eol = true;
  return b;
}

BeautifulContext BeautifulContext::as_internal_eol() const {
  auto b(*this);
  b.eol = 2;
  return b;
}

BeautifulContext BeautifulContext::not_eol() const {
  auto b(*this);
  b.eol = false;
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

string render_internal_eol(const BeautifulConfig& config, BeautifulContext context, PrInfixWS* piws) {
  PrEmptyStatement pes;
  pes.infixes.push_back(piws);
  pes.postfix_n = 1;
  pes.flattenPostfixes();
  auto& postfixes = pes.infixes;
  
  // trim postfixes at start and end:
  while (!postfixes.empty()) {
    if (postfixes.front()) {
      if (postfixes.front()->val.value == "\n") {
        postfixes.pop_front();
      } else {
        if (postfixes.back()) {
          if (postfixes.back()->val.value == "\n")
            postfixes.pop_back();
          else
            break;
        } else {
          postfixes.pop_back();
        }
      }
    } else postfixes.pop_front();
  }
  
  // remove blank lines and collect string
  string s = "";
  int blanks_seen = 0;
  for (int i=0;i<postfixes.size();i++) {
    if (postfixes[i]) {
      bool render = true;
      if (postfixes[i]->val.value == "\n") {
        render = blanks_seen == 0;
        if (render)
          context.pad_infix_left = false;
        blanks_seen += 1;
      } else {
        blanks_seen = 0;
      }
      if (render)
        s += postfixes[i]->beautiful(config, context);
    }
  }
  
  return s;
}

string Production::renderWS(const BeautifulConfig& config, BeautifulContext context) {
  if (infixes.empty())
    return "";
  PrInfixWS* ws = infixes.front();
  infixes.pop_front();
  if (!ws)
    return "";
  // internal eol requires processing infix list:
  if (context.eol == 2) {
    return render_internal_eol(config, context, ws);
  }
  
  string s(ws->beautiful(config, context));
  
  delete(ws);
  return s;
}

string Production::renderPostfixesTrimmed(const BeautifulConfig& config, BeautifulContext context) {
  string s = "";
  
  // expand postfixes
  flattenPostfixes();
  
  // remove trailing blank infixes
  if (context.no_trailing_blanks) {
    while (!infixes.empty()) {
      auto pf = infixes.back();
      if (pf) {
        if (pf->val.type != ENX)
          break;
        
        // transfer nested infixes up:
        while (!pf->infixes.empty()) {
          infixes.push_back(pf->infixes.back());
          pf->infixes.pop_back();
        }
      }
      infixes.pop_back();
    }
  }
  
  // blank_before_comment configuration option
  if (config.blank_before_comment) {
    if (!infixes.empty()) {
      if (!is_a<PrEmptyStatement>(this)) {
        int starting_blank_count = 0;
        for (int i=0;i<infixes.size();i++) {
          if (infixes[i]) {
            if (infixes[i]->val.type == COMMENT && starting_blank_count == 0) {
              continue;
            } else if (infixes[i]->val.value == "\n") {
              starting_blank_count++;
            } else {
              if (starting_blank_count == 1)
                infixes.insert(infixes.begin() + i, new PrInfixWS(Token(ENX,"\n")));
              break;
            }
          }
        }
      }
    }
  }
  
  // internal: aggressively trim newlines internally and externally from postfixes
  if (context.eol == 2) {
    int max_blank_count = 0;
    int blank_count = max_blank_count + 1;;
    for (int i=0;i<infixes.size();i++) {
      auto& ws = infixes[i];
      if (ws) {
        if (ws->val.value == "\n") {
          if (blank_count > max_blank_count) {
            delete(ws);
            ws = nullptr;
          }
          blank_count ++;
        } else {
          blank_count = 0;
        }
      }
    }
  }
  
  // render postfixes
  while (!infixes.empty()) {
    bool next_pad = context.pad_infix_left;
    if (infixes[0]) next_pad = infixes[0]->val.value != "\n";
    s += renderWS(config, context);
    context.pad_infix_left = next_pad;
  }
  
  return s;
}

string PrStatement::end_statement_beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = "";
  
  // add semicolon
  if ((config.semicolons &&! context.never_semicolon) || context.forced_semicolon) {
    s += ";";
  }
  
  s += renderPostfixesTrimmed(config, context);
  
  return s;
}

string PrDecor::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return indent(config, context) + rawToken.value;
}

string PrExprParen::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "(" + content->beautiful(config, context.as_inline()) + ")";
  return s + renderWS(config, context);
}

string PrExpressionFn::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + identifier.value + renderWS(config, context) + "(";
  s += join_productions(args, ", ", config, context.as_inline(), this);
  s += ")";
  s += renderWS(config, context);
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
  
  s += renderWS(config, context);
  
  return s;
}

string PrEmptyStatement::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  context.never_semicolon = true;
  if (context.attached)
    return end_statement_beautiful(config, context.force_semicolon());
  context.pad_infix_left = false;
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
  
  BeautifulContext subcontext;
  
  if (is_root) {
    s = "";
  } else {
    subcontext = context.increment_depth().detach();
  }
    
  // trim config
  bool l_trim = config.trim_block;
  bool r_trim = config.trim_block;
  
  // add productions within block
  for (int i=0;i<productions.size();i++) {
    auto p = productions[i];
    
    // trim blank lines at start
    if (is_a<PrEmptyStatement>(p) && l_trim)
      continue;
    else
      l_trim = false;
     
    // trim blank lines at end [final iteration]
    if (i == productions.size() - 1 && r_trim) {
      subcontext.no_trailing_blanks = true;
    }
      
    // append text from production
    s += "\n";
    if (is_root && i==0)
      s = "";
    s += p->beautiful(config, subcontext);
  }
  if (productions.size() > 0)
    s += "\n" + indent(config, context);
  else
    s += " ";
  if (!is_root) {
    s += "}";
  }
  return s;
}

string PrStatementIf::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = "";
  if (!context.attached)
    s += indent(config, context);
  else
    context = context.decrement_depth();
  
  s += "if " + renderWS(config, context.style(PAD_NEITHER).style(PAD_RIGHT));
  s += condition->beautiful(config,context.as_inline());
  s += renderWS(config, context.as_internal_eol());
  if (!hangable(result))
    s += "\n";
  
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
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
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
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

string PrControl::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + kw.value;
  if (val) s += " " + val->beautiful(config, context.as_inline());
  s += end_statement_beautiful(config, context);
  return s;
}

string PrWhile::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) + "while ";
  s += renderWS(config, context.style(PAD_NEITHER).style(PAD_RIGHT));
  s += condition->beautiful(config, context.as_inline());
  s += renderWS(config, context.as_internal_eol());
  if (hangable(event))
    context = context.attach();
  else
    s += "\n";
  s += event->beautiful(config, context.increment_depth());
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

string PrWith::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  string s = indent(config, context) +"with ";
  s += renderWS(config, context.style(PAD_NEITHER).style(PAD_RIGHT));
  s += objid->beautiful(config, context.as_inline());
  s += renderWS(config, context.as_internal_eol());
  if (hangable(event))
    context = context.attach();
  else
    s += "\n";
  s += event->beautiful(config, context.increment_depth());
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
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
  s += renderWS(config, context.style(PAD_NEITHER).style(PAD_RIGHT));
  s += condition->beautiful(config, context.as_inline());
  s += renderWS(config, context.as_internal_eol());
  
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
  context.pad_infix_left = false;
  
  s += val.value;
  // newline must be followed by correct indent
  if (val.value == "\n")
    s += indent(config,context);
  
  // render nested infixes:
  for (int i=0;i<infixes.size();i++)
    if (infixes[i])
      s += infixes[i]->beautiful(config, context);
  
  if (context.pad_infix_right && val.type == COMMENT && val.value[1] == '*')
    s += " ";
  return s;
}
