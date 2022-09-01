//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_XX_STRING_H
#define EMP_XX_STRING_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XC_NULL                '\0'
#define EMP_XC_NEWLINE             '\n'
#define EMP_XC_TABULATION          '\t'
#define EMP_XC_SPACE               ' '
#define EMP_XC_NOT                 '!'
#define EMP_XC_DOUBLE_QUOTE        '"'
#define EMP_XC_SHARP               '#'
#define EMP_XC_DOLLAR              '$'
#define EMP_XC_MOD                 '%'
#define EMP_XC_AND                 '&'
#define EMP_XC_QUOTE               '\''
#define EMP_XC_LEFT_PARENTHESIS    '('
#define EMP_XC_RIGHT_PARENTHESIS   ')'
#define EMP_XC_MUL                 '*'
#define EMP_XC_ADD                 '+'
#define EMP_XC_COMMA               ','
#define EMP_XC_SUB                 '-'
#define EMP_XC_DOT                 '.'
#define EMP_XC_DIV                 '/'
#define EMP_XC_COLON               ':'
#define EMP_XC_SEMI_COLON          ';'
#define EMP_XC_LESS                '<'
#define EMP_XC_EQUAL               '='
#define EMP_XC_MORE                '>'
#define EMP_XC_QUESTION            '?'
#define EMP_XC_AT                  '@'
#define EMP_XC_LEFT_BRACKET        '['
#define EMP_XC_BACK_SLASH          '\\'
#define EMP_XC_RIGHT_BRACKET       ']'
#define EMP_XC_XOR                 '^'
#define EMP_XC_UNDERSCORE          '_'
#define EMP_XC_BACK_QUOTE          '`'
#define EMP_XC_LEFT_BRACE          '{'
#define EMP_XC_OR                  '|'
#define EMP_XC_RIGHT_BRACE         '}'
#define EMP_XC_TILDE               '~'

#define EMP_XC_lower_a 'a'
#define EMP_XC_lower_f 'f'
#define EMP_XC_lower_z 'z'
#define EMP_XC_UPPER_A 'A'
#define EMP_XC_UPPER_D 'D'
#define EMP_XC_UPPER_E 'E'
#define EMP_XC_UPPER_F 'F'
#define EMP_XC_UPPER_I 'I'
#define EMP_XC_UPPER_O 'O'
#define EMP_XC_UPPER_W 'W'
#define EMP_XC_UPPER_Z 'Z'

#define EMP_XC_0 '0'
#define EMP_XC_1 '1'
#define EMP_XC_2 '2'
#define EMP_XC_3 '3'
#define EMP_XC_4 '4'
#define EMP_XC_5 '5'
#define EMP_XC_6 '6'
#define EMP_XC_7 '7'
#define EMP_XC_8 '8'
#define EMP_XC_9 '9'

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_NULL                ""
#define EMP_XSZ_NEWLINE             "\n"
#define EMP_XSZ_TABULATION          "\t"
#define EMP_XSZ_SPACE               " "
#define EMP_XSZ_NOT                 "!"
#define EMP_XSZ_DOUBLE_QUOTE        "\""
#define EMP_XSZ_SHARP               "#"
#define EMP_XSZ_DOLLAR              "$"
#define EMP_XSZ_MOD                 "%"
#define EMP_XSZ_AND                 "&"
#define EMP_XSZ_QUOTE               "'"
#define EMP_XSZ_LEFT_PARENTHESIS    "("
#define EMP_XSZ_RIGHT_PARENTHESIS   ")"
#define EMP_XSZ_MUL                 "*"
#define EMP_XSZ_ADD                 "+"
#define EMP_XSZ_COMMA               ","
#define EMP_XSZ_SUB                 "-"
#define EMP_XSZ_DOT                 "."
#define EMP_XSZ_DIV                 "/"
#define EMP_XSZ_COLON               ":"
#define EMP_XSZ_SEMI_COLON          ";"
#define EMP_XSZ_LESS                "<"
#define EMP_XSZ_EQUAL               "="
#define EMP_XSZ_MORE                ">"
#define EMP_XSZ_QUESTION            "?"
#define EMP_XSZ_AT                  "@"
#define EMP_XSZ_LEFT_BRACKET        "["
#define EMP_XSZ_BACK_SLASH          "\\"
#define EMP_XSZ_RIGHT_BRACKET       "]"
#define EMP_XSZ_XOR                 "^"
#define EMP_XSZ_UNDERSCORE          "_"
#define EMP_XSZ_BACK_QUOTE          "`"
#define EMP_XSZ_LEFT_BRACE          "{"
#define EMP_XSZ_OR                  "|"
#define EMP_XSZ_RIGHT_BRACE         "}"
#define EMP_XSZ_TILDE               "~"

#define EMP_XSZ_lower_a "a"
#define EMP_XSZ_lower_z "z"
#define EMP_XSZ_UPPER_A "A"
#define EMP_XSZ_UPPER_Z "Z"
#define EMP_XSZ_UPPER_E "E"

#define EMP_XSZ_0 "0"
#define EMP_XSZ_1 "1"
#define EMP_XSZ_2 "2"
#define EMP_XSZ_3 "3"
#define EMP_XSZ_4 "4"
#define EMP_XSZ_5 "5"
#define EMP_XSZ_6 "6"
#define EMP_XSZ_7 "7"
#define EMP_XSZ_8 "8"
#define EMP_XSZ_9 "9"

#define EMP_XSZ_FMT_X0 "#0"
#define EMP_XSZ_FMT_X1 "#1"
#define EMP_XSZ_FMT_X2 "#2"
#define EMP_XSZ_FMT_X3 "#3"
#define EMP_XSZ_FMT_X4 "#4"

#define EMP_XSZ_BASE_2  "01"
#define EMP_XSZ_BASE_8  "01234567"
#define EMP_XSZ_BASE_10 "0123456789"
#define EMP_XSZ_BASE_16 "0123456789abcdef"

#define EMP_XSZ_NULLPTR     "nullptr"
#define EMP_XSZ_TRUE        "true"
#define EMP_XSZ_FALSE       "false"
#define EMP_XSZ_ASSERT      "Assert"
#define EMP_XSZ_DEBUG       "Debug"
#define EMP_XSZ_INFORMATION "Information"
#define EMP_XSZ_WARNING     "Warning"
#define EMP_XSZ_ERROR       "Error"
#define EMP_XSZ_OUTPUT      "Output"
#define EMP_XSZ_STATEMENT   "Statement"
#define EMP_XSZ_CONDITION   "Condition"
#define EMP_XSZ_MESSAGE     "Message"
#define EMP_XSZ_FILE        "File"
#define EMP_XSZ_LINE        "Line"
#define EMP_XSZ_FUNCTION    "Function"
#define EMP_XSZ_SIGNATURE   "Signature"
#define EMP_XSZ_NAME        "Name"
#define EMP_XSZ_ALIAS       "Alias"
#define EMP_XSZ_DESCRIPTION "Description"
#define EMP_XSZ_INVALID     "Invalid"
#define EMP_XSZ_DUPLICATE   "Duplicate"
#define EMP_XSZ_OPTION      "Option"
#define EMP_XSZ_FORMAT      "Format"
#define EMP_XSZ_CHAR        "char"

#define EMP_XSZ_yuyu "yuyu"

#define EMP_XSZ_ERR_INVALID_PARAMETER        "Invalid parameter"
#define EMP_XSZ_ERR_INVALID_OPTION_X0        "Invalid option : \"#0\""
#define EMP_XSZ_ERR_INVALID_INPUT            "Invalid input"
#define EMP_XSZ_ERR_INVALID_USER_INPUT       "Invalid user input"
#define EMP_XSZ_ERR_INVALID_CHAR_X0          "Invalid char : '#0'"

#define EMP_XSZ_ERR_BUFFER_TOO_SMALL         "Buffer to small"

#define EMP_XSZ_ERR_DUPLICATE_ADDRESS        "Duplicate address"
#define EMP_XSZ_ERR_DUPLICATE_NAME_X0        "Duplicate name : \"#0\""
#define EMP_XSZ_ERR_DUPLICATE_ALIAS_X0       "Duplicate alias : '#0'"
#define EMP_XSZ_ERR_NOT_FOUND                "Not found"
#define EMP_XSZ_ERR_PARSING_FAILED           "Parsing failed"

#define EMP_XSZ_ERR_NOT_FOUND                "Not found"
#define EMP_XSZ_ERR_VALUE_NOT_FOUND_X0       "Value not found : (#0)"
#define EMP_XSZ_ERR_ALIAS_NOT_FOUND_X0       "Alias not found : '#0'"
#define EMP_XSZ_ERR_NAME_NOT_FOUND_X0        "Name not found : \"#0\""
#define EMP_XSZ_ERR_DESCRIPTION_NOT_FOUND_X0 "Description not found : \"#0\""

#define EMP_XSZ_S1 "//"
#define EMP_XSZ_S0 \
"//---------------------------------------------------------------------------\
--"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_SPACE_COMMA EMP_XSZ_SPACE EMP_XSZ_COMMA
#define EMP_XSZ_SPACE_COMMA_SPACE EMP_XSZ_SPACE_COMMA EMP_XSZ_SPACE

#define EMP_XSZ_S1_SPACE EMP_XSZ_S1 EMP_XSZ_SPACE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_NEWLINE_S0 EMP_XSZ_NEWLINE EMP_XSZ_S0
#define EMP_XSZ_NEWLINE_S1_SPACE EMP_XSZ_NEWLINE EMP_XSZ_S1_SPACE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_HEADER_X0\
    EMP_XSZ_S0\
    EMP_XSZ_NEWLINE_S1_SPACE "#0"\
    EMP_XSZ_NEWLINE_S0

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_HEADER_STATEMENT\
    EMP_XSZ_S0\
    EMP_XSZ_NEWLINE_S1_SPACE EMP_XSZ_STATEMENT\
    EMP_XSZ_NEWLINE_S0

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_XSZ_HEADER_ASSERT\
    EMP_XSZ_S0\
    EMP_XSZ_NEWLINE_S1_SPACE EMP_XSZ_ASSERT\
    EMP_XSZ_NEWLINE_S0

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

