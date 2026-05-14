
// Public entry point for the JSON package.
//
// External callers reach the package via the sefun wrappers in
// /lib/core/efuns/json.c, which call:
//     "/packages/json/master"->encode(value)
//     "/packages/json/master"->decode(text)
//
// Implementation lives in src/encode.c and src/decode.c; we inherit
// them here so this object exposes the two public methods.

inherit "/packages/json/src/encode.c";
inherit "/packages/json/src/decode.c";
