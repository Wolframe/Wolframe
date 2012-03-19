
#include "config/description.hpp"
#include "configurationDescription_1.hpp"

int main( int, const char**)
{
        A cfg;
        std::string filename( "a.conf");
        std::string errmsg;
        boost::property_tree::ptree pt;
        try
        {
                boost::property_tree::read_info( configfile, pt);
                if (cfg.description()->parse( &cfg, pt, errmsg))
                {
                        cfg.description()->print( std::cout, &cfg);
                }
        }
        catch (std::exception& e)
        {
                std::cerr << e.what();
        }
}
