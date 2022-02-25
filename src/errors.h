// https://people.cs.georgetown.edu/~squier/Teaching/HardwareFundamentals/LC3-trunk/docs/README-LC3tools.html
//
// Command            ---- Description
// -------------      ---------------------------
// lc3as file.asm     ---- Creates file.obj and file.sym, the symbol table.
//
// Error Message                      ---- Explanation
// ------------------------           ---------------------------
// file contains only comments        ---- no assembly instructions in file.
// no .ORIG or .END directive found   ---- .ORIG/.END must be first/last line.
// multiple .ORIG directives found    ---- one .ORIG is allowed per file.
// instruction appears before .ORIG   ---- instructions must start after .ORIG.
// no .END directive found            ---- all programs must end with .END.
// label appears before .ORIG         ---- nothing should appear before .ORIG.
// label has already appeared         ---- label can only be defined once.
// unknown label x                    ---- label x is referenced, but not
// defined. illegal operands for x             ---- instruction has bad operand
// specification. unterminated string                ---- .STRINGZ has no
// close-quotes. contains unrecognizable characters ---- found chars that
// assembler cannot parse. WARNING: All text after .END is ignored.

#include <string>
#include <unordered_map>

// clang-format off
#define ERROR_DATA(d)\
d(noAssembly,           "No assembly instructions in file.")\
d(ORIG_END_location,    ".ORIG/END must be first/last line.")\
d(onlyOneORIG,          "one .ORIG is allowed per file.")\
d(assemblyBeforeORIG,   "instructions must start after .ORIG.")\
d(mustEndWithEND,       "all programs must end with .END")\
d(nothingBeforeORIG,    "nothing should appear before .ORIG")\
d(labelDefinedOnlyOnce, "label can only be defined once.")\
d(unknownLabel,         "label is referenced but NOT defined.")\
d(illigalOperand,       "instruction had bad operand specification.")\
d(untermincatedString,  ".STRINGZ has no close-quotes")\
d(unrecognizableChar,   "found chars that assembler cannot parse.")\
d(ignoreAfterEND,       "Warning: All text after .END is ignored.")
// clang-format on

#define e(a, b) a,
enum class Error { ERROR_DATA(e) };
#undef e

struct ErrorInfo {
  Error er;
  int location;
};

#define e(a, b) {Error::a, b},
const std::unordered_map<Error, const std::string> err2msg = {ERROR_DATA(e)};
#undef e
