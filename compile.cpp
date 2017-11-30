#include "compile.h"
#include "production.h"
#include "util.h"
#include "error.h"

LBString compileModule(const PrBody& body) {
  LBString out;
  out += "#include \"root.h\"\n#include \"variable.h\"\n#include \"function.h\"\n\nusing namespace ogm;\n\nvoid launcher(C c)";
  out += LBString(FORCE);
  
  CompilerContext cc;
  out += body.compile(cc);
  
  out += LBString(FORCE) + LBString(FORCE);
  out += "int main (int argn, char** argv)\n{\n  Root root;\n  root.run(launcher);\n}\n";
  return out;
}

LBString PrExprParen::compile(const CompilerContext& cc) const {
  return "(" + content->compile(cc) + ")";
}

LBString PrExpressionFn::compile(const CompilerContext& cc) const {
  LBString s = "ogm::fn::" + identifier.value + "(" + cc.runtime_context;
  for (int i=0;i<args.size();i++)
  {
    s += ",";
    s += args[i]->compile(cc);
  }
  s += ")";
  return s;
}

std::string c_op(Token t) {
  if (t.value == "not")
    return "!";
  if (t.value == "and")
    return "&&";
  if (t.value == "or")
    return "||";
  if (t.value == "xor")
    return "^^";
  if (t.value == "mod")
    return "%";
  return t.value;
}

LBString PrExprArithmetic::compile(const CompilerContext& cc) const {
  LBString s;
  // div has no direct C analogue:
  if (op.value == "div") {
    PrExpressionFn pfn(Token(ID,"floor"));
    PrExprArithmetic sub(lhs, Token(OP, "/"), rhs);
    pfn.args.push_back(&sub);
    s = pfn.compile(cc);
    sub.lhs = nullptr;
    sub.rhs = nullptr;
    return s;
  }
  // all other operations:
  if (lhs)
    s += lhs->compile(cc);
  s += LBString(PAD) + c_op(op) + LBString(PAD);
  s += rhs->compile(cc);
}

LBString PrEmptyStatement::compile(const CompilerContext& cc) const {
  return ";";
}

std::string sanitize_string(std::string val)
{
  val = val.substr(1,val.size()-2);
  val = replace_all(val,"\\", "\\\\");
  val = replace_all(val,"\n", "\\n");
  val = replace_all(val,"\t", "\\t");
  val = replace_all(val,"\t", "\\t");
  val = replace_all(val,"\"", "\\\"");
  return "\"" + val + "\"";
}

LBString PrFinal::compile(const CompilerContext& cc) const {
  std::string val = this->final.value;
  
  // strings use double quotes
  if (val[0] == '\'' || val[0] == '"')
    return sanitize_string(val);
  
  // hex $ -> 0x
  if (val[0] == '$') {
    val[0] = 'x';
    val = "0" + val;
  }
    
  return val;
}

LBString PrIdentifier::compile(const CompilerContext& cc) const {
  return identifier.value;
}

LBString PrAssignment::compile(const CompilerContext& cc) const {
  LBString s;
  if (lhs)
    s += lhs->compile(cc);
  s += LBString(PAD) + op.value;
  if (rhs) {
    s += LBString(PAD) + rhs->compile(cc);
  }
  s += ";";
  return s;
}

LBString PrStatementFn::compile(const CompilerContext& cc) const {
  return fn->compile(cc) + ";";
}

LBString PrVarDeclaration::compile(const CompilerContext& cc) const {
  LBString s;
  s += identifier.value;
  if (definition)
    s += " = " + definition->compile(cc);
  return s;
}

LBString PrStatementVar::compile(const CompilerContext& cc) const {
  LBString s;
  if (type == "var") {
    s += "var ";
    for (int i=0;i<declarations.size();i++) {
      if (i>0)
        s += "," + LBString(PAD);
      s += declarations[i]->compile(cc);
    }
    s += ";";
  } else {
    throw LanguageFeatureNotImplementedError(type + " declarations");
  }
  return s;
}

LBString PrBody::compile(const CompilerContext& cc) const {
  LBString s;
  if (productions.empty()) return "{ }";
  s += "{" + LBString(FORCE);
  LBString s2;
  for (int i=0;i<productions.size();i++) {
    s2 += productions[i]->compile(cc);
    s2 += LBString(FORCE);
  }
  s.append(s2.indent(true).indent(true));
  s += "}";
  return s;
}

LBString PrStatementIf::compile(const CompilerContext& cc) const {
  LBString s;
  s += "if (";
  s += condition->compile(cc);
  s += ")" + LBString(FORCE);
  s.append(result->compile(cc));
  if (otherwise) {
    s += LBString(FORCE);
    s += "else";
    s += LBString(FORCE);
    s.append(otherwise->compile(cc));
  }
  return s;
}

LBString PrWhile::compile(const CompilerContext& cc) const {
  LBString s;
  s += "while (";
  s += condition->compile(cc);
  s += ")" + LBString(FORCE);
  s.append(event->compile(cc));
  return s;
}

LBString PrFor::compile(const CompilerContext& cc) const {
  LBString s;
  s += "for (";
  if (init)
    s += init->compile(cc);
  else
    s += ";";
  if (condition)
    s += condition->compile(cc);
  s += ";";
  if (second)
    s += second->compile(cc);
  s += ")" + LBString(FORCE);
  s += first->compile(cc);
  return s;
}

const char* varlist = "ijkqp";

LBString PrRepeat::compile(const CompilerContext& cc) const {
  std::string varname = "_";
  CompilerContext cc_internal = cc;
  int varl = ++(cc_internal.varl);
  while (varl > 5) {
    varname.push_back(varlist[varl % 5]);
    varl /= 5;
  }
  LBString s;
  s += "{" + LBString(FORCE);
  LBString s2;
  s2 += "int _max" + varname + " = Variable(" + count->compile(cc_internal) + ").get_real();" + LBString(FORCE);
  s2 += "for (int " + varname + " = 0; " + varname + " < _max" + varname + "; " + varname + "++)";
  s2 += LBString(FORCE);
  s2 += event->compile(cc_internal);
  s.append(s2.indent(true));
  s += LBString(FORCE);
  s += "}";
  return s;
}

LBString PrDo::compile(const CompilerContext& cc) const {
  LBString s;
  s += "do";
  s += LBString(FORCE);
  s.append(event->compile(cc));
  s += LBString(FORCE);
  s += "while (!";
  s += condition->compile(cc);
  s += ")";
  return s;
}

LBString PrWith::compile(const CompilerContext& cc) const {
  throw LanguageFeatureNotImplementedError("with statements");
}

LBString PrAccessorExpression::compile(const CompilerContext& cc) const {
  if (acc != "")
    throw LanguageFeatureNotImplementedError("accessor \"" + acc + "\"");
  else {
    LBString s = ds->compile(cc);
    for (int i=0;i<indices.size();i++) {
      s += "[" + indices[i]->compile(cc) + "]";
    }
  }
}

LBString PrSwitch::compile(const CompilerContext& cc) const {
  LBString s;
  s += "switch (";
  s += condition->compile(cc);
  s += ")";
  s += LBString(FORCE);
  s += "{";
  s += LBString(FORCE);
  for (int i=0;i<cases.size();i++)
  {
    s += cases[i]->compile(cc);
  }
  s += "}";
  return s;
}

LBString PrCase::compile(const CompilerContext& cc) const {
  LBString s;
  s += "case " + value->compile(cc) + ":";
  s += "{";
  LBString s2;
  for (int i=0;i<productions.size();i++) {
    s2 += productions[i]->compile(cc);
    s2 += LBString(FORCE);
  }
  s.append(s2.indent(true));
  s += "}";
  return s;
}

LBString PrControl::compile(const CompilerContext& cc) const {
}