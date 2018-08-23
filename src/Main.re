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

let newLexer = src => {src, pos: 0, line: 0};
let curSrcPos = l => {line: l.line, srcFile: l.src};

let nthChar = (l, n) => {
  let offset = l.pos + n;
  if (offset < String.length(l.src.text)) {
    l.src.text.[offset];
  } else {
    'x'; /* XXX */
  };
};
let curChar = l => nthChar(l, 0);
let advanceLexer = (l, n) => {...l, pos: l.pos + n};

let charInRange = (c, a, b) => c >= a && c <= b;
let isAlphaChar = c => charInRange(c, 'a', 'z') || charInRange(c, 'A', 'Z');
let isNumericChar = c => charInRange(c, '0', '9');
let isIdentStart = isAlphaChar;
let isIdentChar = c => isIdentStart(c) || isNumericChar(c) || c == '_';

let rec countCharMatches = (s, f) =>
  if (String.length(s) == 0 || !f(s.[0])) {
    0;
  } else {
    let s = String.sub(s, 1, String.length(s) - 1);
    countCharMatches(s, f) + 1;
  };

let restOfString = (s, offset) =>
  offset > String.length(s) ?
    "" : String.sub(s, offset, String.length(s) - 1);

Js.log(restOfString("abc ", 1));

let matchIdent = l => {
  if (isIdentStart(curChar(l))) {
    let n = countCharMatches(restOfString(l.src.text, 1), isIdentChar);
    (advanceLexer(l, 1 + n), None);
  } else {
    (l, None);
  };
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
