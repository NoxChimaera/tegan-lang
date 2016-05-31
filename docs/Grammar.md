# Грамматика языка
## Лексемы
### Символы
- '(' - PL (Parenthesis Left),
- ')' - PR (Parenthesis Right),
- '{' - BL (Brace Left),
- '}' - BR (Brace Right),
- ',' - COMMA,
- ';' - SEMICOLON,
- ':' - COLON,
- '+' - ADD('+'),
- '-' - ADD('-'),
- '*' - MUL('*'),
- '/' - MUL('/'),
- '\==' - CMP_EQ('=='),
- '!=' - CMP_EQ('!='),
- '>' - CMP('>'),
- '>=' - CMP('>='),
- '<' - CMP('<'),
- '<=' - CMP('<='),
- '&&' - LAND,
- '||' - LOR,
- '!' - NOT,
- '=' - ASSIGN

### Идентификаторы, ключевые слова и константы
- (:alnum:)+ - SYMBOL (идентификатор),
- 'func' - FUNC,
- 'if' - IF,
- 'else' - ELSE,
- 'do' - DO,
- 'while' - WHILE,
- 'return' - RETURN,
- 'int' - TYPE('int'),
- 'float' - TYPE('float'),
- 'bool' - TYPE('bool'),
- 'False' - BOOL('False'),
- 'True' - BOOL('True'),
- 'print' - IO_PRINT,
- (:digit:)+ - INTEGER,
- (:digit:)+\.(:digit:)+ - FLOAT

## Синтаксис
- `start := <program-stmt>*`,
- `program-stmt := <function-definition>`,
- `function-definition := FUNC SYMBOL ( '(' <function-args>? ')' )? ':' TYPE <stmt>`,
- `function-args := ( TYPE SYMBOL ',' )* TYPE SYMBOL`,
- `stmt := <block-stmt> | <if-stmt> | <while-stmt> | <do-while-stmt> | <declaration> ';'? | <assign> ';'? | <io-print> ';'? | <return> ';'?`
- `block-stmt := '{' <stmt>* '}'`,
- `if-stmt := IF <expr> <stmt> ( ELSE <stmt> )?`,
- `while-stmt := WHILE <expr> <stmt>`,
- `do-while-stmt := DO <stmt> WHILE <expr>`,
- `declaration := TYPE SYMBOL '=' <expr>`,
- `assign := SYMBOL '=' <expr>`,
- `io-print := PRINT <expr>`,
- `return := RETURN <expr>`,
- `expr := <lor>`,
- `lor := <land> LOR <lor> | <land>`,
- `land := <cmpeq> LAND <land> | <cmpeq>`,
- `cmpeq := <cmp> CMP_EQ <cmpeq> | <cmp?`,
- `cmp := <add> CMP <cmp> | <add>`,
- `add := <mul> ADD <add> | <mul>`,
- `mul := <unary> MUL <mul> | <unary>`,
- `unary := ADD <unary> | NOT <unary> | <factor>`,
- `factor := '('  <lor> ')' | <const> | <funcall> | SYMBOL`,
- `const := BOOL | INTEGER | FLOAT`,
- `funcall := SYMBOL '(' <funcall-args>? ')'`,
- `funcall-args := ( <expr> ',' )* <expr>`