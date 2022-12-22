//-----------------------------------------------------------------------------
//           Name: nano_yaml.cpp
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

#include "nano_yaml.h"

#include <Compat/fileio.h>
#include <Logging/logdata.h>

#include <algorithm>

// Utility function to trim whitespace off the ends of a string
inline std::string trim(std::string source) {
    std::string result = source.erase(source.find_last_not_of(" \t\r") + 1);
    return result.erase(0, result.find_first_not_of(" \t\r"));
}

namespace YAML {

bool Interpreter::LoadYAML(std::ifstream& stream, Map& map, bool overwrite) {
    std::string line;
    std::string comment_delimeter = "#";
    std::string key_delimeter = ":";

    int count = 0;
    while (stream.good()) {
        Scalar scalar;
        scalar.order = count;  // record the current line
        count++;

        getline(stream, line);

        // Check for any comments
        size_t commIdx = line.find(comment_delimeter);
        if (commIdx != std::string::npos) {
            // Grab the comment (excluding the "#")
            scalar.comment = line.substr(commIdx + 1);
            // Strip the comment from the rest of the line
            line = line.substr(0, commIdx);
        }
        
        // If the remaining line is empty, then we assume that this is used for formatting
        if (trim(line).empty()) {
            // Mark the value as a literal string and add it as an entry
            scalar.literal = true;
            _literals.push_back(scalar);
            continue;
        }

        // Otherwise, the line has some intended content
        // Check to see if it can be interpreted as a "key: value" pair
        size_t delimIdx = line.find(key_delimeter);
        if (delimIdx == std::string::npos) {
            // Send an error describing the offending entry
            LOGE << "Couldn't parse line '" << line << "'" << std::endl;
            continue;
        }

        std::string key = trim(line.substr(0, delimIdx));
        std::string value = trim(line.substr(delimIdx + 1));

        // TODO: handle double and single quoted strings

        scalar.key = key;
        scalar.data = value;
        scalar.literal = false;

        if (!key.empty()) {
            if (overwrite || map.find(key) == map.end()) {
                map[key] = scalar;
            }
        }
    }
    return true;
}

bool Interpreter::SaveYAML(std::ofstream& file, const Map& map) {
    // Aggregate all scalars, both the formatted strings and the key: value pairs
    std::vector<std::pair<std::string, Scalar> > all_scalars(map.begin(), map.end());
    all_scalars.insert(all_scalars.end(), _literals.begin(), _literals.end());

    // Sort them to produce a list that should be identical in order to the originally read file
    std::sort(all_scalars.begin(), all_scalars.end());

    for (auto& s : _literals) {
        if (!s.literal) {
            file << s.key << ": " << map[s.key].data;
        }
        if (!s.comment.empty()) {
            file << (s.literal ? "# " : " # ") << s.comment;
        }
        file << "\n";
    }

    return true;
}

}  // namespace YAML