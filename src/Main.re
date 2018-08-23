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
  col: int,
};

type lexer = {
  srcPos,
  text: string,
};

type tkind =
  | Tauto
  | Tbreak
  | Tcase
  | Tchar
  | Tconst
  | Tcontinue
  | Tdefault
  | Tdo
  | Tdouble
  | Telse
  | Tenum
  | Textern
  | Tfloat
  | Tfor
  | Tgoto
  | Tif
  | Tint
  | Tlong
  | Tregister
  | Treturn
  | Tshort
  | Tsigned
  | Tsizeof
  | Tstatic
  | Tstruct
  | Tswitch
  | Ttypedef
  | Tunion
  | Tunsigned
  | Tvoid
  | Tvolatile
  | Twhile
  | Tplusassign
  | Tminusassign
  | Tmulassign
  | Tdivassign
  | Tmodassign
  | Teq
  | Tplus
  | Tminus
  | Tmul
  | Tdiv
  | Tmod
  | Tassign
  | Tident(string)
  | Tintv(int64)
  | Tws
  | Terr
  | Tcomment
  | Teof;

type token = {
  srcPos,
  s: string,
  k: tkind,
};

let charInRange = (c, a, b) => c >= a && c <= b;
let isAlphaChar = c => charInRange(c, 'a', 'z') || charInRange(c, 'A', 'Z');
let isNumericChar = c => charInRange(c, '0', '9');
let isIdentStart = isAlphaChar;
let isIdentChar = c => isIdentStart(c) || isNumericChar(c) || c == '_';
let isWsChar = c => c == ' ' || c == '\n' || c == '\t';

/* These string functions are in the stdlib I think,
     need to learn how to access them.
   */
let substrAt = (s, offset) =>
  offset >= String.length(s) ?
    "" : String.sub(s, offset, String.length(s) - offset);

let substrUpto = (s, offset) =>
  offset > String.length(s) ?
    String.sub(s, 0, String.length(s)) : String.sub(s, 0, offset);

let rec strfind = (~offset=0, s, fs) =>
  if (String.length(s) < String.length(fs)) {
    None;
  } else if (String.sub(s, 0, String.length(fs)) == fs) {
    Some(offset);
  } else {
    strfind(~offset=offset + 1, substrAt(s, 1), fs);
  };

let rec countCharMatches = (~n=0, s, f) =>
  String.length(s) == 0 ?
    n : countCharMatches(~n=n + (f(s.[0]) ? 1 : 0), substrAt(s, 1), f);

let countNewLines = s => countCharMatches(s, c => c == '\n');

let rec countCharPrefixMatches = (s, f) =>
  if (String.length(s) == 0 || !f(s.[0])) {
    0;
  } else {
    let s = String.sub(s, 1, String.length(s) - 1);
    countCharPrefixMatches(s, f) + 1;
  };

let newLexer = src => {
  srcPos: {
    srcFile: src,
    line: 0,
    col: 0,
  },
  text: src.text,
};

let rec nextColumn = (curCol, s) =>
  switch (String.length(s)) {
  | 0 => curCol
  | _ when s.[0] == '\n' => nextColumn(0, substrAt(s, 1))
  | _ when s.[0] == '\t' => nextColumn(curCol + 4, substrAt(s, 1))
  | _ => nextColumn(curCol + 1, substrAt(s, 1))
  };

let advanceLexer = (l, n) => {
  let toks = substrUpto(l.text, n);
  (
    {
      text: substrAt(l.text, n),
      srcPos: {
        srcFile: l.srcPos.srcFile,
        line: l.srcPos.line + countNewLines(toks),
        col: nextColumn(l.srcPos.col, toks),
      },
    },
    toks,
  );
};

let keywordMap = Hashtbl.create(32);
let () = {
  let keyword = (k, v) => Hashtbl.add(keywordMap, k, v);
  keyword("auto", Tauto);
  keyword("break", Tbreak);
  keyword("case", Tcase);
  keyword("char", Tchar);
  keyword("const", Tconst);
  keyword("continue", Tcontinue);
  keyword("default", Tdefault);
  keyword("do", Tdo);
  keyword("double", Tdouble);
  keyword("else", Telse);
  keyword("enum", Tenum);
  keyword("extern", Textern);
  keyword("float", Tfloat);
  keyword("for", Tfor);
  keyword("goto", Tgoto);
  keyword("if", Tif);
  keyword("int", Tint);
  keyword("long", Tlong);
  keyword("register", Tregister);
  keyword("return", Treturn);
  keyword("short", Tshort);
  keyword("signed", Tsigned);
  keyword("sizeof", Tsizeof);
  keyword("static", Tstatic);
  keyword("struct", Tstruct);
  keyword("switch", Tswitch);
  keyword("typedef", Ttypedef);
  keyword("union", Tunion);
  keyword("unsigned", Tunsigned);
  keyword("void", Tvoid);
  keyword("volatile", Tvolatile);
  keyword("while", Twhile);
};

let lookupKeyword = s =>
  switch (Hashtbl.find(keywordMap, s)) {
  | v => Some(v)
  | exception Not_found => None
  };

let identOrKeyword = s =>
  switch (Hashtbl.find(keywordMap, s)) {
  | v => v
  | exception Not_found => Tident(s)
  };

  /* A basic lexer func follows the pattern of
      match n chars, callling a constructor on the matched string,
      and advancing the lexer.
   */
let mkBasicLexerFunc = (matcher, mkTok, l) => {
  let n = matcher(l.text);
  if (n > 0) {
    let (newlexer, match) = advanceLexer(l, n);
    Some((newlexer, {srcPos: l.srcPos, s: match, k: mkTok(match)}));
  } else {
    None;
  };
};

/* A two part lexer func follows the pattern of
      match n chars of prefix, match n chars of body,
      call a constructor on the matched string.
      advance lexer.
   */
let mkTwoPartLexerFunc = (matchStart, matchBody, mkTok, l) => {
  let nPrefix = matchStart(l.text);
  if (nPrefix > 0) {
    let nBody = matchBody(substrAt(l.text, nPrefix));
    let (newlexer, match) = advanceLexer(l, nPrefix + nBody);
    Some((newlexer, {srcPos: l.srcPos, s: match, k: mkTok(match)}));
  } else {
    None;
  };
};

let lexCComment =
mkBasicLexerFunc(
      s => {
        let start = substrUpto(s, 2);
        let rest = substrAt(s, 2);
        if (start != "/*") {
          0;
        } else {
          switch (strfind(rest, "*/")) {
          | Some(n) => 2 + n + 2
          | None => 0
          };
        };
      },
    _s => Tcomment,
  );

let lexCPPComment =
mkBasicLexerFunc(
      s => {
        let start = substrUpto(s, 2);
        let rest = substrAt(s, 2);
        if (start != "//") {
          0;
        } else {
          switch (strfind(rest, "\n")) {
          | Some(n) => 2 + n + 1
          | None => String.length(s)
          };
        };
      },
    _s => Tcomment,
  );

let lexWs =
  mkBasicLexerFunc(
    s => countCharPrefixMatches(s, isWsChar),
    _s => Tws,
  );

let lexIdentOrKeyword =
  mkTwoPartLexerFunc(
    s => countCharPrefixMatches(s, isIdentStart),
    s => countCharPrefixMatches(s, isIdentChar),
    identOrKeyword,
  );

let lexNumber =
  mkBasicLexerFunc(
    s => countCharPrefixMatches(s, isNumericChar),
    s => Tintv(Int64.of_string(s)),
  );

let lexOp = l => {
  let mkop = (n, k) => {
    let (nextlexer, s) = advanceLexer(l, n);
    Some((nextlexer, {srcPos: l.srcPos, s, k}));
  };
  let mkbinop = mkop(2);
  let mkunop = mkop(1);

  switch (substrUpto(l.text, 2)) {
  | "+=" => mkbinop(Tplusassign)
  | "-=" => mkbinop(Tminusassign)
  | "*=" => mkbinop(Tmulassign)
  | "/=" => mkbinop(Tdivassign)
  | "%=" => mkbinop(Tmodassign)
  | "==" => mkbinop(Teq)
  | _ =>
    switch (substrUpto(l.text, 1)) {
    | "+" => mkunop(Tplus)
    | "-" => mkunop(Tminus)
    | "*" => mkunop(Tmul)
    | "/" => mkunop(Tdiv)
    | "%" => mkunop(Tmod)
    | "=" => mkunop(Tassign)
    | _ => None
    }
  };
};

let lexEof = l =>
  String.length(l.text) == 0 ?
    Some((l, {srcPos: l.srcPos, s: "", k: Teof})) : None;

/* choose between two lexer functions a and b*/
let lexChoice = (a, b, l) => {
  let av = a(l);
  av == None ? b(l) : av;
};

let lex = l => {
  /* TODO -> operator */
  let lexNoErr =
    lexChoice(
      lexChoice(
        lexChoice(
          lexChoice(
            lexChoice(lexChoice(lexWs, lexCComment), lexCPPComment),
            lexIdentOrKeyword,
          ),
          lexNumber,
        ),
        lexOp,
      ),
      lexEof,
    );

  let maybeLexed = lexNoErr(l);
  switch (maybeLexed) {
  | Some((l, tok)) => (l, tok)
  | None => (l, {srcPos: l.srcPos, s: "", k: Terr})
  };
};

let testSrc = text => {path: "<test>", text};
let l = newLexer(testSrc("foo /* hu \n hu */ // bar\nbang"));
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
let (l, t) = lex(l);
Js.log(t.s);
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


 */
