# JSON parser
I've developed a JSON file parser in C++ (utilizing the C++17 standard). The parser is quite straightforward; initially, it performs a check to validate whether the content received from the file complies with the JSON format. Subsequently, the content is traversed once more to efficiently store the information in memory. While I cannot guarantee that the verification function is entirely flawless or highly efficient, the approach to storing information is simplistic and easily comprehensible