#ifndef PARSER_UTILS_H_
#define PARSER_UTILS_H_

#include "parser.h"
#include "scanner.h"

namespace ParserUtils {
class PrintWalker;
void pprint(Parser::TreeNode *p);

#endif // PARSER_UTILS_H_
}
