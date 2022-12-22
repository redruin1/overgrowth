//-----------------------------------------------------------------------------
//           Name: nano_yaml.h
//      Developer: Wolfire Games LLC + redruin1 (Thomas Schaefer)
//    Description: Simple YAML-compliant file loader and emitter
//        License: Read below
//-----------------------------------------------------------------------------
//
//   Copyright 2022 Wolfire Games LLC
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//-----------------------------------------------------------------------------

/**
 * @brief Provides functionality of a very small subset of the YAML Markup 
 * language specification, intended for the use in 1-dimensional configuration
 * files.
 * 
 * Creates a base implementation that is used by `config.cpp` and `locale.cpp`.
 * 
 * In order to re-use code, I've modified the format of config files by changing
 * the comment delimeter from `//` to `#`; but this is the only change needed to
 * make the current config files YAML-compliant.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

namespace YAML {

class Scalar {
public:
    std::string key; // redundant, but necessary
    std::string data;
    std::string comment;
    bool literal;
    int order;

    Scalar() : data(), comment() { }

    bool operator<(const Scalar& other) {
        return order < other.order;
    }
};

/**
 * @brief Base class that provides loading and saving itself in YAML-compliant
 * format.
 */
class Interpreter {
public:
    typedef std::map<std::string, Scalar> Map;
    Map _map;
    std::vector<Scalar> _literals;  // Comments and whitespace used to aide in readability
    
    bool LoadYAML(std::ifstream& stream, Map& map, bool overwrite=true);
    bool SaveYAML(std::ofstream& filename, const Map& map);
};

}  // namespace YAML