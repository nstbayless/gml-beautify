#include "root.h"
#include "variable.h"
#include "function.h"

namespace ogm { namespace varn {
constexpr VariableID z = 0;
}}

using namespace ogm;

void launcher(C c)
{
    c.instance->local[ogm::varn::z] = "the value of z";
    ogm::fn::show_debug_message(c,c.instance->local[ogm::varn::z]);
    var x(0);
    var y( - 4);
    if (y < x)
    x += 1;
    if (y < x)
    x += 1;
    {
        int _max_i = Variable((17)).get_real();
        for (int _i = 0; _i < _max_i; _i++)
        y += 1;
    }
    {
        int _max_i = Variable((y)).get_real();
        for (int _i = 0; _i < _max_i; _i++)
        x += y;
    }
    y -= x + 90.434253;
    var q(1);
    {
        int _max_i = Variable((10)).get_real();
        for (int _i = 0; _i < _max_i; _i++)
        q *= 2;
    }
    var rz(256);
    do
    rz /= 2;
    while (!(rz <= 40));
    var abh(ogm::fn::angle_difference(c, - 94,32));
    ogm::fn::show_debug_message(c,abh);
    ogm::fn::show_debug_message(c,q);
    ogm::fn::show_debug_message(c,rz);
    ogm::fn::show_debug_message(c,y);
    ogm::fn::show_debug_message(c,"test");
    var r("blah");
    ogm::fn::show_debug_message(c,r);
}

int main (int argn, char** argv)
{
  Root root;
  root.run(launcher);
}
