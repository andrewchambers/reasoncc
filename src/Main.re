[@bs.module "fs"]
external readFileSync: (string, string) => string = "readFileSync";
[@bs.module "process"] external argv: array(string) = "argv";

type srcFile = {
  path: string,
  text: string,
};

type srcPos = {
  srcFile,
  line: int,
};

type lexer = {
  src: srcFile,
  line: int,
  pos: int,
};

type token = {
  srcPos,
  s: string,
};

let charInRange = (c, a, b) => c >= a && c <= b;
let isAlphaChar = c => charInRange(c, 'a', 'z') || charInRange(c, 'A', 'Z');
let isNumericChar = c => charInRange(c, '0', '9');
let isIdentStart = isAlphaChar;
let isIdentChar = c => isIdentStart(c) || isNumericChar(c) || c == '_';

let restOfString = (s, offset) =>
  offset > String.length(s) ?
    "" : String.sub(s, offset, String.length(s) - 1);

let rec countCharMatches = (~n=0, s, f) =>
  if (String.length(s) == 0) {
    n;
  } else {
    countCharMatches(~n=n + (f(s.[0]) ? 1 : 0), restOfString(s, 1), f);
  };

let rec countCharPrefixMatches = (s, f) =>
  if (String.length(s) == 0 || !f(s.[0])) {
    0;
  } else {
    let s = String.sub(s, 1, String.length(s) - 1);
    countCharPrefixMatches(s, f) + 1;
  };

let newLexer = src => {src, pos: 0, line: 0};
let curSrcPos = l => {line: l.line, srcFile: l.src};

let nthChar = (l, n) => {
  let s = restOfString(l.src.text, n);
  if (String.length(s) == 0) {
    '\000'; /* For now null byte is eof, this simplifies predicate functions */
  } else {
    s.[0];
  };
};

let curChar = l => nthChar(l, 0);

let advanceLexer = (l, n) => {
  let toks = restOfString(l.src.text, l.pos + n);
  (
    {
      ...l,
      pos: l.pos + n,
      line: l.line + countCharMatches(toks, c => c == '\n'),
    },
    toks,
  );
};

let matchIdent = l =>
  if (isIdentStart(curChar(l))) {
    let n = countCharPrefixMatches(restOfString(l.src.text, 1), isIdentChar);
    let (newl, identstr) = advanceLexer(l, 1 + n);
    (newl, None);
  } else {
    (l, None);
  };

/*

  let readSrc = path => {
    let text = readFileSync(path, "utf-8");
    {path, text};
  };

  let runcc = args => {
    let src = readSrc(args[2]);
    ();
  };

  runcc(argv);


 let testSrc = text =>  {path: "<test>", text}
 let testLexer = newLexer (testSrc("abc"))
 let (l, t) =  matchIdent(testLexer)
 */
