
#include "serialize/struct/filtermapBase.hpp"
#include <string>

namespace _Wolframe {
namespace example {

struct Customer
{
    int ID;                         // Internal customer id
    std::string name;               // Name of the customer
    std::string canonical_Name;     // Customer name in canonical form
    std::string country;            // Country
    std::string locality;           // Locality

    static const serialize::StructDescriptionBase* getStructDescription();
};

}}//namespace

