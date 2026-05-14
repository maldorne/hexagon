
#include "/packages/test/src/test.h"

// Battery for json_encode / json_decode (defined in
// /lib/core/efuns/json/{encode,decode}.c).

void do_tests()
{
  string s;
  mixed value;
  mapping m;
  mixed * arr;

  // ============================================================
  // json_encode
  // ============================================================

  TEST("json_encode nil → null");
    ASSERT(json_encode(nil) == "null");
  END_TEST();

  TEST("json_encode integer");
    ASSERT(json_encode(0) == "0");
    ASSERT(json_encode(1) == "1");
    ASSERT(json_encode(-42) == "-42");
    ASSERT(json_encode(1234567) == "1234567");
  END_TEST();

  TEST("json_encode float");
    // %g formatting; pick values with exact representations
    ASSERT(json_encode(1.5) == "1.5");
    ASSERT(json_encode(-0.25) == "-0.25");
    ASSERT(json_encode(0.0) == "0");
  END_TEST();

  TEST("json_encode empty string");
    ASSERT(json_encode("") == "\"\"");
  END_TEST();

  TEST("json_encode simple string");
    ASSERT(json_encode("hello") == "\"hello\"");
    ASSERT(json_encode("a b c") == "\"a b c\"");
  END_TEST();

  TEST("json_encode escapes quote and backslash");
    ASSERT(json_encode("a\"b") == "\"a\\\"b\"");
    ASSERT(json_encode("a\\b") == "\"a\\\\b\"");
  END_TEST();

  TEST("json_encode escapes whitespace controls");
    ASSERT(json_encode("\n") == "\"\\n\"");
    ASSERT(json_encode("\r") == "\"\\r\"");
    ASSERT(json_encode("\t") == "\"\\t\"");
    ASSERT(json_encode("\b") == "\"\\b\"");
    ASSERT(json_encode("\f") == "\"\\f\"");
  END_TEST();

  TEST("json_encode emits \\uXXXX for other control chars");
    // 0x01 is below 0x20 and not one of the named escapes
    ASSERT(json_encode(chr(1)) == "\"\\u0001\"");
    ASSERT(json_encode(chr(0x1f)) == "\"\\u001f\"");
  END_TEST();

  TEST("json_encode passes UTF-8 bytes through verbatim");
    // "ñ" is C3 B1, must appear unmodified inside the JSON string
    ASSERT(json_encode("ñ") == "\"ñ\"");
    ASSERT(json_encode("canción") == "\"canción\"");
  END_TEST();

  TEST("json_encode empty array");
    ASSERT(json_encode(({ })) == "[]");
  END_TEST();

  TEST("json_encode flat array");
    ASSERT(json_encode(({ 1, 2, 3 })) == "[1,2,3]");
    ASSERT(json_encode(({ "a", "b" })) == "[\"a\",\"b\"]");
    ASSERT(json_encode(({ 1, "x", nil })) == "[1,\"x\",null]");
  END_TEST();

  TEST("json_encode nested array");
    ASSERT(json_encode(({ ({ 1, 2 }), ({ 3, 4 }) })) ==
           "[[1,2],[3,4]]");
  END_TEST();

  TEST("json_encode empty mapping");
    ASSERT(json_encode(([ ])) == "{}");
  END_TEST();

  TEST("json_encode flat mapping (single key — deterministic)");
    // single-entry mapping has a deterministic order
    ASSERT(json_encode(([ "a" : 1 ])) == "{\"a\":1}");
    ASSERT(json_encode(([ "x" : "y" ])) == "{\"x\":\"y\"}");
  END_TEST();

  TEST("json_encode mapping decodes back to the same shape");
    // multi-key order is implementation-defined; verify by round-trip
    m = ([ "a" : 1, "b" : "two", "c" : nil ]);
    s = json_encode(m);
    ASSERT(EQUALS(json_decode(s), m));
  END_TEST();

  TEST("json_encode nested mapping in array");
    s = json_encode(({ ([ "k" : "v" ]), 42 }));
    ASSERT(EQUALS(json_decode(s), ({ ([ "k" : "v" ]), 42 })));
  END_TEST();

  TEST("json_encode raises on mapping with non-string key");
    ASSERT(catch(json_encode(([ 1 : "a" ]))) != nil);
  END_TEST();

  TEST("json_encode raises on object value");
    ASSERT(catch(json_encode(this_object())) != nil);
  END_TEST();

  TEST("json_encode raises on object nested inside structure");
    ASSERT(catch(json_encode(({ this_object() }))) != nil);
    ASSERT(catch(json_encode(([ "k" : this_object() ]))) != nil);
  END_TEST();

  // ============================================================
  // json_decode
  // ============================================================

  TEST("json_decode null → nil");
    ASSERT(json_decode("null") == nil);
  END_TEST();

  TEST("json_decode boolean → 1 / 0");
    ASSERT(json_decode("true") == 1);
    ASSERT(json_decode("false") == 0);
  END_TEST();

  TEST("json_decode integer");
    ASSERT(json_decode("0") == 0);
    ASSERT(json_decode("42") == 42);
    ASSERT(json_decode("-7") == -7);
    ASSERT(json_decode("123456") == 123456);
  END_TEST();

  TEST("json_decode float (presence of '.' or exponent)");
    ASSERT(json_decode("1.5") == 1.5);
    ASSERT(json_decode("-0.25") == -0.25);
    ASSERT(json_decode("1e3") == 1000.0);
    ASSERT(json_decode("2.5e2") == 250.0);
  END_TEST();

  TEST("json_decode empty string");
    ASSERT(json_decode("\"\"") == "");
  END_TEST();

  TEST("json_decode simple string");
    ASSERT(json_decode("\"hello\"") == "hello");
    ASSERT(json_decode("\"a b c\"") == "a b c");
  END_TEST();

  TEST("json_decode unescapes named escapes");
    ASSERT(json_decode("\"a\\\"b\"") == "a\"b");
    ASSERT(json_decode("\"a\\\\b\"") == "a\\b");
    ASSERT(json_decode("\"a\\/b\"") == "a/b");
    ASSERT(json_decode("\"\\n\"") == "\n");
    ASSERT(json_decode("\"\\r\"") == "\r");
    ASSERT(json_decode("\"\\t\"") == "\t");
    ASSERT(json_decode("\"\\b\"") == "\b");
    ASSERT(json_decode("\"\\f\"") == "\f");
  END_TEST();

  TEST("json_decode \\uXXXX for ASCII range");
    // A → 'A'
    ASSERT(json_decode("\"\\u0041\"") == "A");
    // / → '/'
    ASSERT(json_decode("\"\\u002f\"") == "/");
  END_TEST();

  TEST("json_decode \\uXXXX for 2-byte UTF-8");
    // ñ → "ñ" (UTF-8: C3 B1)
    ASSERT(json_decode("\"\\u00f1\"") == "ñ");
  END_TEST();

  TEST("json_decode empty array");
    ASSERT(EQUALS(json_decode("[]"), ({ })));
  END_TEST();

  TEST("json_decode flat array");
    ASSERT(EQUALS(json_decode("[1,2,3]"), ({ 1, 2, 3 })));
    ASSERT(EQUALS(json_decode("[\"a\",\"b\"]"), ({ "a", "b" })));
    ASSERT(EQUALS(json_decode("[1,\"x\",null]"), ({ 1, "x", nil })));
  END_TEST();

  TEST("json_decode nested array");
    ASSERT(EQUALS(json_decode("[[1,2],[3,4]]"),
               ({ ({ 1, 2 }), ({ 3, 4 }) })));
  END_TEST();

  TEST("json_decode empty object");
    ASSERT(EQUALS(json_decode("{}"), ([ ])));
  END_TEST();

  TEST("json_decode flat object");
    ASSERT(EQUALS(json_decode("{\"a\":1}"), ([ "a" : 1 ])));
    ASSERT(EQUALS(json_decode("{\"x\":\"y\"}"), ([ "x" : "y" ])));
  END_TEST();

  TEST("json_decode multi-key object");
    m = json_decode("{\"a\":1,\"b\":2,\"c\":3}");
    ASSERT(m["a"] == 1);
    ASSERT(m["b"] == 2);
    ASSERT(m["c"] == 3);
  END_TEST();

  TEST("json_decode nested object inside array");
    arr = json_decode("[{\"k\":\"v\"},42]");
    ASSERT(sizeof(arr) == 2);
    ASSERT(arr[0]["k"] == "v");
    ASSERT(arr[1] == 42);
  END_TEST();

  TEST("json_decode tolerates whitespace");
    ASSERT(json_decode("   42   ") == 42);
    ASSERT(EQUALS(json_decode("\n[\n1\n,\n2\n]\n"), ({ 1, 2 })));
    ASSERT(EQUALS(json_decode("{ \"a\" : 1 , \"b\" : 2 }"),
               ([ "a" : 1, "b" : 2 ])));
  END_TEST();

  TEST("json_decode rejects trailing garbage");
    ASSERT(catch(json_decode("1 2")) != nil);
    ASSERT(catch(json_decode("[1,2]extra")) != nil);
  END_TEST();

  TEST("json_decode rejects unterminated string");
    ASSERT(catch(json_decode("\"abc")) != nil);
  END_TEST();

  TEST("json_decode rejects unterminated array");
    ASSERT(catch(json_decode("[1,2")) != nil);
  END_TEST();

  TEST("json_decode rejects unterminated object");
    ASSERT(catch(json_decode("{\"a\":1")) != nil);
  END_TEST();

  TEST("json_decode rejects unknown literal");
    ASSERT(catch(json_decode("nope")) != nil);
    ASSERT(catch(json_decode("truthy")) != nil);
  END_TEST();

  TEST("json_decode rejects bare comma");
    ASSERT(catch(json_decode(",")) != nil);
  END_TEST();

  TEST("json_decode rejects empty input");
    ASSERT(catch(json_decode("")) != nil);
  END_TEST();

  TEST("json_decode rejects nil input");
    ASSERT(catch(json_decode(nil)) != nil);
  END_TEST();

  // ============================================================
  // round-trips
  // ============================================================

  TEST("round-trip: scalars");
    ASSERT(json_decode(json_encode(nil)) == nil);
    ASSERT(json_decode(json_encode(0)) == 0);
    ASSERT(json_decode(json_encode(42)) == 42);
    ASSERT(json_decode(json_encode(-7)) == -7);
    ASSERT(json_decode(json_encode("hello")) == "hello");
    ASSERT(json_decode(json_encode("")) == "");
  END_TEST();

  TEST("round-trip: strings with escapes");
    value = "a\"b\\c\nd\te";
    ASSERT(json_decode(json_encode(value)) == value);
  END_TEST();

  TEST("round-trip: UTF-8 string");
    value = "canción ñ áéíóú";
    ASSERT(json_decode(json_encode(value)) == value);
  END_TEST();

  TEST("round-trip: arrays");
    value = ({ 1, 2, 3 });
    ASSERT(EQUALS(json_decode(json_encode(value)), value));

    value = ({ "a", nil, 42, ({ "nested", 1 }) });
    ASSERT(EQUALS(json_decode(json_encode(value)), value));
  END_TEST();

  TEST("round-trip: mappings");
    value = ([ "a" : 1, "b" : "two", "c" : nil ]);
    ASSERT(EQUALS(json_decode(json_encode(value)), value));
  END_TEST();

  TEST("round-trip: deeply nested");
    value = ([
      "users" : ({
        ([ "name" : "alice", "level" : 10 ]),
        ([ "name" : "bob",   "level" : 5  ]),
      }),
      "count" : 2,
      "next"  : nil,
    ]);
    ASSERT(EQUALS(json_decode(json_encode(value)), value));
  END_TEST();
}
