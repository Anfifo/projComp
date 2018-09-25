#define tINTEGER 257
#define tDOUBLE 258
#define tNULL 259
#define tIDENTIFIER 260
#define tSTRING 261
#define tTYPE 262
#define tPUBLIC 263
#define tUSE 264
#define tSMALL 265
#define tHUGE 266
#define tNEWS 267
#define tINPUT 268
#define tIF 269
#define tAND 270
#define tOR 271
#define tINITIALLY 272
#define tASSIGN 273
#define tTO 274
#define tSTOP 275
#define tAGAIN 276
#define tRETURN 277
#define tTWEET 278
#define tPOST 279
#define tCELL 280
#define tAT 281
#define tOBJECTS 282
#define tTHEN 283
#define tFROM 284
#define tBY 285
#define tSWEEP 286
#define tDO 287
#define tDEF 288
#define tFOR 289
#define tFUNCTION 290
#define tUSES 291
#define tON 292
#define tAS 293
#define tDEFINE 294
#define tPROCEDURE 295
#define tBLOCKEND 296
#define tBLOCKBEGIN 297
#define tIFX 298
#define tELIF 299
#define tELSE 300
#define tUNARY 301
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
  int                   i;	/* integer value */
  std::string          *s;	/* symbol name or string literal */
  double                 d;
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  basic_type           *t;
  gr8::block_node      *b;
  gr8::sweeping_node   *sw;
  gr8::null_node       *null;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
