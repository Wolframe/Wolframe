
#include "config/description.hpp"
#include "configurationDescription_1.hpp"

const config::DescriptionBase* A::description()
{
        struct ThisDescription :public config::Description<A>
        {
                ThisDescription()
                {
                        (*this)
                        ( "x", &A::x)
                        ( "y", &A::y);
                }
        };
        static const ThisDescription rt;
        return &rt;
}

