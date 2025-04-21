#ifndef STR_CONST_HPP
#define STR_CONST_HPP

#include <string>
// List of strings to be replaced with Helm dependency injection
namespace params{
    const std::string DEFAULT_LOG_DIR("");
    const std::string ground_log_path(DEFAULT_LOG_DIR + "/ground/_");
    const std::string ground_pub_address("tcp://localhost:5555");
    const std::string ground_sub_address("tcp://localhost:5556");
    const std::string sat_pub_address("tcp://localhost:5555");
    const std::string sat_sub_address("tcp://localhost:5556");
    const std::string proxy_xpub_address("tcp://localhost:5556");
    const std::string proxy_xsub_address("tcp://localhost:5555");
}

#endif