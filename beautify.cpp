# include "parser.h"
# include "beautify.h"
# include "util.h"

# include <iterator>

using namespace std;

template<class P>
LBString join_productions(const std::vector<P*> productions, LBString joinder, const BeautifulConfig& config, BeautifulContext context, Production* infix_source = nullptr) {
  bool first = true;
  LBString s;
  context.cost_mult *= 1.4f;
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

/// modifies surrounding parenthesis for production according to config.
LBString paren_wrap(Production* pr, const BeautifulConfig& config, BeautifulContext context) {
  if (!pr)
    return "";
  switch (config.cond_parens) {
    case 0: // leave as-is
      return pr->beautiful(config,context);
    case 1: // exactly one paren
      while (is_a<PrExprParen>(pr)) {
        pr = ((PrExprParen*)pr)->content;
      }
      return "(" + pr->beautiful(config,context) + ")";
    case 2: // exactly zero parens
      while (is_a<PrExprParen>(pr)) {
        pr = ((PrExprParen*)pr)->content;
      }
      return pr->beautiful(config,context);
  }
}

bool hangable(const BeautifulConfig& config, Production* p, bool egyptian = false) {
  return (is_a<PrBody>(p) && (config.egyptian || !egyptian)) || is_a<PrEmptyStatement>(p);
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

BeautifulContext BeautifulContext::trim_leading_blanks() const {
  auto b(*this);
  b.eol = 3;
  return b;
}

BeautifulContext BeautifulContext::not_eol() const {
  auto b(*this);
  b.eol = false;
  return b;
}

BeautifulContext BeautifulContext::force_semicolon() const {
  auto b(*this);
  b.forced_semicolon = true;
  return b;
}

LBString Production::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return "Unknown Production";
}

LBString render_internal_eol(const BeautifulConfig& config, BeautifulContext context, PrInfixWS* piws) {
  // this function uses dark magic to render nested
  // infixes in a pretty way that ensures properties
  // of consecutive, leading, and trailing blank lines
  PrEmptyStatement pes;
  pes.infixes.push_back(piws);
  pes.postfix_n = 1;
  pes.flattenPostfixes();
  auto& postfixes = pes.infixes;
  
  // trim newlines at start and end:
  while (!postfixes.empty()) {
    if (postfixes.front()) {
      if (postfixes.front()->val.value == "\n") {
        postfixes.pop_front();
      } else {
        if (context.eol == 3)
          break;
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
  LBString s;
  int blanks_seen = 0;
  for (int i=0;i<postfixes.size();i++) {
    if (postfixes[i]) {
      bool render = true;
      if (postfixes[i]->val.value == "\n") {
        render = (blanks_seen == 0);
        if (context.eol == 3)
          render = true;
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

LBString Production::renderWS(const BeautifulConfig& config, BeautifulContext context) {
  if (infixes.empty())
    return "";
  PrInfixWS* ws = infixes.front();
  infixes.pop_front();
  if (!ws)
    return "";
  // internal eol requires processing infix list:
  if (context.eol >= 2) {
    return render_internal_eol(config, context, ws);
  }
  
  if (ws->val.value == "\n" && context.no_single_newline) {
    // check no non-null infixes
    bool no_non_null = true;
    for (int i=0;i<ws->infixes.size();i++)
      if (ws->infixes[i])
        no_non_null = false;
    if (no_non_null) {
      // preserve line-endings?
      if (config.columns == -2) {
        return LBString(INFORCE);
      } else {
        return "";
      }
    }
  }
  
  LBString s(ws->beautiful(config, context));
  
  delete(ws);
  return s;
}

LBString Production::renderPostfixesTrimmed(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  
  context.no_single_newline = false;
  
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
      if (!is_a<PrEmptyStatement>(this)) { // empty statements are exempt from blank_before_comment
        int starting_blank_count = 0;
        for (int i=0;i<infixes.size();i++) {
          if (infixes[i]) {
            if (infixes[i]->val.type == COMMENT && starting_blank_count == 0) {
              continue;
            } else if (infixes[i]->val.value == "\n") {
              starting_blank_count++;
            } else {
              if (starting_blank_count == 1) {
                infixes.insert(infixes.begin() + i, new PrInfixWS(Token(ENX,"\n")));
                postfix_n++;
              }
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
    s += renderWS(config, context);
  }
  
  return s;
}

LBString PrStatement::end_statement_beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  
  // add semicolon
  if ((config.semicolons || context.forced_semicolon) &&! context.never_semicolon) {
    s += ";";
  }
  
  s += renderPostfixesTrimmed(config, context);
  
  return s;
}

LBString PrDecor::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return rawToken.value;
}

LBString PrExprParen::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  context.cost_mult *= 3;
  LBString s = "(" + LBString(NOPAD, 5 * context.cost_mult) + renderWS(config, context);
  s += content->beautiful(config, context) + ")";
  return s + renderWS(config, context);
}

LBString PrExpressionFn::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = identifier.value + renderWS(config, context) + "(";
  if (args.size() > 0) {
    s += LBString(NOPAD, 5 * context.cost_mult);
    s.extend(join_productions(args, "," + LBString(PAD), config, context, this));
  }
  s += ")";
  s += renderWS(config, context);
  return s;
}

LBString PrExprArithmetic::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  bool l_space = true;
  bool r_space = true;
  
  // adjust operator for some config options:
  if (op.value == "=" && config.force_double_equals_comparison)
    op.value = "==";
  if (op.value == "!" && config.compare_style == 2)
    op = Token(KW,"not");
  if (op.value == "||" && config.compare_style == 2)
    op = Token(KW,"or");
  if (op.value == "&&" && config.compare_style == 2)
    op = Token(KW,"and");
  if (op.value == "^^" && config.compare_style == 2)
    op = Token(KW,"xor");
  if (op.value == "not" && config.compare_style == 1)
    op = Token(OP,"!");
  if (op.value == "and" && config.compare_style == 1)
    op = Token(OP,"&&");
  if (op.value == "or" && config.compare_style == 1)
    op = Token(OP,"||");
  if (op.value == "xor" && config.compare_style == 1)
    op = Token(OP,"^^");
    
  float break_cost = get_op_priority(op);
  
  // determine whether to put spacing on side of operator
  if (!config.opr_space && op.type == OPR)
    l_space = r_space = false;
  
  if (!config.not_space && op == Token(OP,"!"))
    l_space = r_space = false;
  
  if (op == Token(PUNC,"."))
    l_space = r_space = false;
  
  if ((op == Token(OP,"-") || op == Token(OP,"+")) && !lhs)
    r_space = false;
  
  // (keywords absolutely need spaces to be parsed)
  if (op.type == KW) {
    l_space = true;
    r_space = true;
  }
  
  // breaking after or before parentheses looks prettier
  if (lhs && rhs)
    if (is_a<PrExpressionFn>(lhs) || is_a<PrExprParen>(lhs) || is_a<PrExprParen>(rhs))
      break_cost *= 0.5;
  
  // beautiful string:
  LBString s;   
  
  if (lhs) {
    s += lhs->beautiful(config,context);
    if (l_space) {
      if (rhs && config.op_end_line)
        s += LBString(PAD, break_cost*context.cost_mult);
      else
        s += " ";
    }
  }
  
  s += renderWS(config, context);
  
  s += op.value;
  
  if (rhs) {
    if (r_space) {
      if (lhs && !config.op_end_line)
        s += LBString(PAD, break_cost*context.cost_mult);
      else
        s += " ";
    }
    s += renderWS(config, context);
    s += rhs->beautiful(config,context);
  }
  
  s += renderWS(config, context);
  
  return s;
}

LBString PrEmptyStatement::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  context.never_semicolon = true;
  return end_statement_beautiful(config, context);
}

LBString PrFinal::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return final.value + renderWS(config, context);
}

LBString PrIdentifier::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  return identifier.value + renderWS(config, context);
}

LBString PrAssignment::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = lhs->beautiful(config,context);
  if ((rhs && lhs) || config.opr_space)
    s += " ";
  s += op.value;
  if (rhs) {
    s += " ";
    s += renderWS(config, context);
    s += rhs->beautiful(config,context);
  }
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrStatementFn::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s(fn->beautiful(config, context));
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrVarDeclaration::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  s += identifier.value;
  s += renderWS(config, context);
  if (definition) {
    s += LBString(PAD,15) + " = ";
    s += renderWS(config, context);
    s += definition->beautiful(config, context);
  }
  return s;
}

LBString PrStatementVar::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = type + " ";
  s += join_productions(declarations, "," + LBString(PAD), config, context, this);
  s += end_statement_beautiful(config, context.force_semicolon());
  return s;
}

LBString PrBody::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  
  s += "{";
  
  BeautifulContext subcontext;
  
  if (is_root) {
    s = LBString(LIST);
  }
  
  LBString s2;
  if (!is_root) {
    s2.indent();
  }
  // trim config
  bool l_trim = config.trim_block;
  bool r_trim = config.trim_block;
  
  bool empty = true;
  // add productions within block
  for (int i=0;i<productions.size();i++) {
    auto p = productions[i];
    
    // trim blank lines at start
    if (is_a<PrEmptyStatement>(p) && l_trim) {
      p->flattenPostfixes();
      bool seen_non_blank = false;
      for (int i=0;i<p->infixes.size();i++) {
        if (p->infixes[i]) {
          if (p->infixes[i]->val.value == "\n") {
            delete(p->infixes[i]);
            p->infixes[i] = nullptr;
          } else {
            seen_non_blank = true;
            break;
          }
        }
      }
      if (!seen_non_blank)
        continue;
    }
    l_trim = false;
    empty = false;
     
    // trim blank lines at end [final iteration]
    if (i == productions.size() - 1 && r_trim) {
      subcontext.no_trailing_blanks = true;
    }
      
    // append text from production
    s2 += LBString(FORCE);
    if (is_root && i==0)
      s2 = LBString(LIST);
    s2 += p->beautiful(config, subcontext);
  }
  
  s.append(s2);
  if (!empty)
    s += LBString(FORCE);
  else
    s += LBString(PAD,25);
  if (!is_root) {
    s += "}";
  }
  
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrControl::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = kw.value;
  if (val) {
    s += renderWS(config, context);
    s += LBString(PAD, 5 * context.cost_mult) + val->beautiful(config, context);
  }
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrStatementIf::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  
  s += "if" + LBString(PAD, 17 * context.cost_mult) + renderWS(config, context);
  s += paren_wrap(condition,config,context);
  s += renderWS(config, context.as_internal_eol());
  s += LBString(PAD, 13 * context.cost_mult);
  
  if (!hangable(config, result, true))
    s += LBString(FORCE);
  
  s.extend(result->beautiful(config, context).indent(!hangable(config,result)), !hangable(config, result));
  if (otherwise) {
    s += renderWS(config, context.trim_leading_blanks().as_internal_eol());
    if (!config.egyptian)
      s += LBString(FORCE);
    s += LBString(PAD) + "else ";
    s += renderWS(config, context.as_internal_eol().trim_leading_blanks());
    bool append = !hangable(config, otherwise) && !is_a<PrStatementIf>(otherwise);
    if (!hangable(config, otherwise,true) && !is_a<PrStatementIf>(otherwise)) {
      if (!config.egyptian && !is_a<PrStatementIf>(otherwise))
          s.new_line();
    }
    s.extend(otherwise->beautiful(config, context).indent(append), append);
  }
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrFor::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "for";
  s += renderWS(config, context.trim_leading_blanks());
  s += LBString(PAD, 17 * context.cost_mult) + "(";
  s += renderWS(config, context);
  context.forced_semicolon = false;
  context.never_semicolon = true;
  s += init->beautiful(config, context.trim_leading_blanks());
  s += renderWS(config, context.trim_leading_blanks());
  s += ";";
  s += LBString(NOPAD);
  if (condition)
    if (init)
      if (!is_a<PrEmptyStatement>(init))
        s += LBString(PAD);
  if (condition)
    s += condition->beautiful(config, context);
  s += ";" + LBString(NOPAD);
  s += renderWS(config, context.trim_leading_blanks());
  
  if (second) {
    if (!is_a<PrEmptyStatement>(second))
      s += LBString(PAD);
    s += second->beautiful(config, context);
  }
  s += renderWS(config, context.trim_leading_blanks());
  s += ")" + LBString(PAD, 13 * context.cost_mult);
  s += renderWS(config, context.trim_leading_blanks().as_internal_eol());
  if (!hangable(config, first, true))
    s += LBString(FORCE);
  
  context.forced_semicolon = false;
  context.never_semicolon = false;
  s.extend(first->beautiful(config, context).indent(!hangable(config,first)), !hangable(config, first));
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrWhile::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "while";
  s += LBString(PAD, 17 * context.cost_mult);
  s += renderWS(config, context);
  s += paren_wrap(condition,config,context);
  s += renderWS(config, context.as_internal_eol());
  s += LBString(PAD, 13 * context.cost_mult);
  if (!hangable(config, event, true))
    s += LBString(FORCE);
  s.extend(event->beautiful(config, context).indent(!hangable(config,event)), !hangable(config, event));
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrRepeat::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "repeat";
  s += LBString(PAD, 17 * context.cost_mult);
  s += renderWS(config, context);
  s += paren_wrap(count,config,context);
  s += renderWS(config, context.as_internal_eol());
  s += LBString(PAD, 13 * context.cost_mult);
  if (!hangable(config, event, true))
    s += LBString(FORCE);
  s.extend(event->beautiful(config, context).indent(!hangable(config,event)), !hangable(config, event));
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrDo::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "do";
  s += renderWS(config, context.as_internal_eol());
  s += LBString(PAD, 13 * context.cost_mult);
  if (!hangable(config, event, true))
    s += LBString(FORCE);
  s.extend(event->beautiful(config, context).indent(!hangable(config,event)), !hangable(config, event));
  s += renderWS(config, context);
  s += " until";
  s += LBString(PAD, 17 * context.cost_mult);
  s += renderWS(config, context);
  s += paren_wrap(condition,config,context);
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrWith::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "with";
  s += LBString(PAD, 17 * context.cost_mult);
  s += renderWS(config, context);
  s += paren_wrap(objid,config,context);
  s += renderWS(config, context.as_internal_eol());
  if (!hangable(config, event, true))
    s += LBString(FORCE);
  s.extend(event->beautiful(config, context).indent(!hangable(config,event)), !hangable(config, event));
  
  // end of statement
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrAccessorExpression::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = ds->beautiful(config, context);
  s += renderWS(config, context);
  s += "[" + renderWS(config, context);
  if (acc.length() > 0) {
    s += acc;
    if (config.accessor_space)
      s += " ";
  }
  s += join_productions(indices, "," + LBString(PAD, 3 * context.cost_mult), config, context, this);
  s += "]";
  return s;
}

LBString PrSwitch::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s = "switch";
  s += LBString(PAD, 17 * context.cost_mult);
  s += renderWS(config, context);
  s += paren_wrap(condition,config,context);
  s += renderWS(config, context.as_internal_eol());
  
  if (cases.size() == 0 && !config.egyptian) {
    context.never_semicolon = true;
    return s + LBString(FORCE) + "{ }" + end_statement_beautiful(config, context);
  }
  
  context = context;
  
  if (config.egyptian)
    s += " {";
  else
    s += LBString(FORCE) + "{";
    
  LBString s2 = LBString(FORCE);
  s2 += renderWS(config, context.trim_leading_blanks());
  
  for (auto c: cases)
    s2 += c->beautiful(config, context);
  
  s.append(s2.indent(true));
  s += "}";
  context.never_semicolon = true;
  s += end_statement_beautiful(config, context);
  return s;
}

LBString PrCase::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  if (value) {
    s += "case ";
    s += renderWS(config, context);
    s += value->beautiful(config, context);
    s += renderWS(config, context);
  } else {
    s += "default";
    s += renderWS(config, context);
  }
  s += ":" + renderWS(config, context.as_internal_eol());
  LBString s2;
  for (auto p: productions) {
    s2 += LBString(FORCE) +  p->beautiful(config, context);
  }
  s.append(s2.indent(true));
  s += renderWS(config, context);
  s += LBString(FORCE);
  context.never_semicolon = true;
  return s;
}

LBString beautify_comment(std::string in, const BeautifulConfig& config, BeautifulContext context) {
  if (!config.comment_space)
    return in;

  rtrim(in);
  
  std::string out = in.substr(0,2);
  
  // find first meaningful character and prepend a space
  for (int i=2;i<in.size();i++) {
    char c = in[i];
    if (iswspace(c))
      return in;
    if (isalnum(c) || c == '$' || c == '.' || c == '?' || c == '<' || c == '>')
      return out + " " + in.substr(i,in.length()-i);
    out += c;
  }
  
  return out;
}

LBString PrInfixWS::beautiful(const BeautifulConfig& config, BeautifulContext context) {
  LBString s;
  
  // pad left (except for postfixes)
  s += " ";
   
  // value
  if (val.value == "\n")
    s += LBString(FORCE);
  else {
    if (val.type == COMMENT)
      s += beautify_comment(val.value, config, context);
    else
      s += val.value;
  }
  
  // render nested infixes:
  context.no_single_newline=false;
  for (int i=0;i<infixes.size();i++)
    if (infixes[i]) {
      s += infixes[i]->beautiful(config, context);
    }
  
  // pad right
  s += LBString(PAD, 30 * context.cost_mult);
  
  return s;
}
