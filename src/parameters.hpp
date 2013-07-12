//  Copyright (c) 2012-2013 Thomas Heller
//  Copyright (c) 2012-2013 Andreas Schaefer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef VANDOUKEN_PARAMETERS_HPP
#define VANDOUKEN_PARAMETERS_HPP

#include "config.hpp"

#include <boost/program_options/options_description.hpp>

namespace vandouken {
    void setupParameters(boost::program_options::options_description& parameters, const std::string& type);
}

#endif
