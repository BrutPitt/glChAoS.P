Configuru
===============================================================================
Configuru, an experimental JSON config library for C++, by Emil Ernerfeldt.


License
-------------------------------------------------------------------------------
This software is in the public domain. Where that dedication is not
recognized, you are granted a perpetual, irrevocable license to copy
and modify this file as you see fit.

That being said, I would appreciate credit!
If you find this library useful, send a tweet to [@ernerfeldt](https://twitter.com/ernerfeldt) or mail me at emil.ernerfeldt@gmail.com.


Documentation
-------------------------------------------------------------------------------
You can read the documentation here: https://emilk.github.io/Configuru/html/index.html


Overview
-------------------------------------------------------------------------------
Configuru is a JSON parser/writer for C++11. Configuru was written for human created/edited config files and therefore prioritizes helpful error messages over parse speed.


Goals
-------------------------------------------------------------------------------
* **Debugable**:
	* Find typos most config libs miss (like typos in keys).
	* Easily find source of typos with file name, line numbers and helpful error messages.
	* Cleverly help to point out mismatched braces in the right place.
* **Configurable**:
	* Configure the format to allow relaxed and extended flavors of JSON.
	* Extensible with custom conversions.
	* Control how the `configuru::Config` behaves in your code via compile time constants:
		* Override `CONFIGURU_ONERROR` to add additional debug info (like stack traces) on errors.
		* Override `CONFIGURU_ASSERT` to use your own asserts.
		* Override `CONFIGURU_ON_DANGLING` to customize how non-referenced/dangling keys are reported.
		* Set `CONFIGURU_IMPLICIT_CONVERSIONS` to allow things like `float f = some_config;`
		* Set `CONFIGURU_VALUE_SEMANTICS` to have `Config` behave like a value type rather than a reference type.
* **Easy to use**:
	* Smooth C++11 integration for reading and creating config values.
* **JSON compliant**:
	* Configuru has one of the highest conformance ratings on the [Native JSON Benchmark](https://github.com/miloyip/nativejson-benchmark)
* **Beautiful output** (pretty printing)
* **Reversible with comments**:
	* If comments are turned on in `FormatOptions` they will be parsed and written together with the right key/value.

### Non-goals
* Low overhead


Error messages
===============================================================================
Configuru prides itself on great error messages both for parse errors and for value errors (expecting one thing and getting another). All error messages come with file name a line number. When parsing from a string (rather than a file) the user can specify an identifier to be used in lieu of a file name.


Parse errors
-------------------------------------------------------------------------------

	equal_in_object.json:1:16: Expected : after object key
	{ "is_this_ok" = true }
	               ^

	bad_escape.json:1:9: Unknown escape character 'x'
	{"42":"\x42"}
	        ^

	no_quotes.json:2:22: Expected value
	   "forgotten_quotes": here
	                       ^

	trucated_key.json:1:2: Unterminated string
	{"X
	 ^

	single_line_comment.json:1:4: Single line comments forbidden.
	{} // Blah-bla
	   ^

	unary_plus.json:1:1: Prefixing numbers with + is forbidden.
	+42
	^

Note how all errors mention follow the standard `filename:line:column` structure (most errors above happen on line `1` since they are from small unit tests).


Value errors
-------------------------------------------------------------------------------
Similarly, using a parsed Config value in the wrong way produces nice error messages. Take the following file (`config.json`):

```
 1: {
 2: 	"pi":    3.14,
 3: 	"array": [ 1, 2, 3, 4 ],
 4: 	"obj":   {
 5: 		"nested_value": 42
 6: 	}
 7: }
```

Here's some use errors and their error messages:

``` C++
auto b = (bool)config["pi"];
```

`config.json:2: Expected bool, got float`. Note that the file:line points to where the value is defined.

``` C++
std::cout << config["obj"]["does_not_exist"];
```

`config.json:4: Failed to find key 'does_not_exist'`. Here the file and line of the owner (`"obj"`) of the missing value is referenced.

``` C++
std::cout << config["pi"][5];
```

`config.json:2: Expected array, got float`.

``` C++
std::cout << config["array"][5];
```

`config.json:3: Array index out of range`

``` C++
Config cfg;
cfg["hello"] = 42;
```

`Expected object, got uninitialized. Did you forget to call Config::object()?`. The first line should read `Config cfg = Config::object();`.

``` C++
Config cfg;
cfg.push_back("hello");
```

`Expected array, got uninitialized. Did you forget to call Config::array()?`. The first line should read `Config cfg = Config::array();`.


Unused keys
-------------------------------------------------------------------------------
Configuru has a novel mechanism for detecting subtle typos in object keys. Suppose you have a Config that looks like this:

	{
		"colour": "red",
		...
	}

Here's how it could be used:

``` C++
auto cfg = configuru::parse_file("config.json", configuru::JSON);
auto color = cfg.get_or("color", DEFAULT_COLOR);
cfg.check_dangling();
```

The call to `check_dangling` will print a warning:

	config.json:2: Key 'colour' never accessed

This is akin to a compiler warning about unused variables and it's an effective way of finding mistakes that would otherwise go undetected.

The call to `check_dangling` is recursive, so you only need to call it once for every config file. If you want to mute this warning for some key (which you may intentionally be ignoring, or saving for later) you can call `cfg.mark_accessed(true)`. This will recursively mark each `Config` as having been accessed.


Usage
===============================================================================
For using:
	`#include <configuru.hpp>`

And in one .cpp file:

``` C++
#define CONFIGURU_IMPLEMENTATION 1
#include <configuru.hpp>
```


Usage (parsing)
-------------------------------------------------------------------------------
``` C++
Config cfg = configuru::parse_file("input.json", JSON);
float alpha = (float)cfg["alpha"];
std::cout << "alpha = " << alpha << std::endl;
if (cfg.has_key("beta")) {
	std::string beta = (std::string)cfg["beta"];
	std::cout << "beta = " << beta << std::endl;
}
float pi = cfg.get_or("pi", 3.14f);
std::cout << "pi = " << pi << std::endl;

if (cfg["array"].is_array()) {
	std::cout << "array:" << std::endl;
	for (const Config& element : cfg["array"].as_array()) {
		std::cout << "\t" << element << std::endl;
	}
}

std::cout << "object" << std::endl;
for (auto& p : cfg["object"].as_object()) {
	std::cout << "\tKey: " << p.key() << std::endl;
	std::cout << "\tValue: " << p.value() << std::endl;
	p.value() = "new value";
}

try {
	cfg.check_dangling(); // Make sure we haven't forgot reading a key!
} catch (const std::exception &e) {
	std::cerr << e.what() << std::endl;
}
// You can modify the read config:
cfg["message"] = "goodbye";

dump_file("output.json", cfg, JSON);
```


Usage (writing)
-------------------------------------------------------------------------------

``` C++
Config cfg = Config::object();
cfg["pi"]     = 3.14;
cfg["array"]  = Config::array{ 1, 2, 3 };
cfg["object"] = Config::object({
	{ "key1", "value1" },
	{ "key2", "value2" },
});
```

Alternative form:

``` C++
Config cfg{
	{"pi",     3.14},
	{"array",  Config::array{ 1, 2, 3 }},
	{"object", {
		{ "key1", "value1" },
		{ "key2", "value2" },
	}},
};
```

``` C++
std::string json = dump_string(cfg, JSON);
dump_file("output.json", cfg, JSON);
```


Usage (visit_struct.hpp)
-------------------------------------------------------------------------------
If you include visit_struct.hpp from https://github.com/cbeck88/visit_struct *before* including configuru.hpp you will enable the following:

``` C++
#include <visit_struct/visit_struct.hpp>
#include <configuru.hpp>

struct Foo
{
	float bar;
	std::string baz;
};
VISITABLE_STRUCT(Foo, bar, baz);

void error_reporter(std::string str)
{
	std::cerr << str << std::endl; // or throw or ignore
}

int main()
{
	Foo foo{42, "fortytwo"};
	configur::Config cfg = configuru::serialize(foo);
	// Save/load cfg
	configuru::deserialize(&foo, cfg, error_reporter);
}
```

The `serialize/deserialize` functions supports numbers, `bool`, `std::string`, `std::vector` and `struct`s annotated with `VISITABLE_STRUCT`.
It is recursive, so a `struct` can contain an `std::vector` of other `struct`s if both types of `struct`s are annotated with `VISITABLE_STRUCT`.


Reference semantics vs value semantics
-------------------------------------------------------------------------------
By default, Config objects acts like reference types, e.g. like a `std::shared_ptr`:

``` C++
Config cfg{{"message", "original"}};
auto shallow_copy = cfg;
cfg["message"] = "changed!";
std::cout << shallow_copy["message"]; // Will print "changed!";

auto deep_clone = cfg.deep_clone(); // Deep clones have to be explicit.
```

You can control this behavior with `#define CONFIGURU_VALUE_SEMANTICS 1`:

``` C++
#define CONFIGURU_VALUE_SEMANTICS 1
#include <configuru.hpp>
...
Config cfg{{"message", "original"}};
auto deep_clone = cfg;
cfg["message"] = "changed!";
std::cout << deep_clone["message"]; // Will print "original";
```


Errors
-------------------------------------------------------------------------------
The default behavior of Configuru is to throw an `std::runtime_error` on any error. You can change this behavior by overriding `CONFIGURU_ONERROR`.


CFG format
===============================================================================
In addition to JSON, Configuru also has native support for a format I simply call *CFG*. CFG is a superset of JSON with some simplifications and extensions. Example file:

	values: [1 2 3 4 5 6]
	object: {
		nested_key: +inf
	}
	python_style: """This is a string
	                 which spans many lines."""
	"C# style": @"Also nice for \ and stuff"

* Top-level can be key-value pairs (no need for {} surrounding entire document).
* Keys need not be quoted if identifiers.
* Commas optional for arrays and objects.
* Trailing , allowed in arrays and objects.

`"""` starts a verbatim multi-line string

`@"` starts a C# style verbatim string which ends on next quote (except `""` which is a single-quote).

Numbers can be represented in any common form:
`-42`, `1e-32`, `0xCAFE`, `0b1010`

`+inf`, `-inf`, `+NaN` are valid numbers.

Indentation is enforced, and must be done with tabs. Tabs anywhere else is not allowed.

You can also allow selective parts of the above extensions to create your own dialect of JSON. Look at the members of `configuru::FormatOptions` for details.


Beautiful output
===============================================================================
One of the great things about JSON is that it is human readable (as opposed to XML). Configuru goes to great lengths to make the output as beautiful as possible. Here's an example structure (as defined in C++):

``` C++
Config cfg = Config::object{
	{"float",       3.14f},
	{"double",      3.14},
	{"short_array", Config::array({1, 2, 3})},
	{"long_array",  Config::array({
		"one",
		Config::array({"two", "things"}),
		"three",
	})},
};
```

Here's how the output turns out in most JSON encoders (this one produced by the excellent [nlohmann json library](https://github.com/nlohmann/json)):

``` JSON
{
    "double": 3.14,
    "float": 3.14000010490417,
    "long_array": [
        "one",
        [
            "two",
            "things"
        ],
        "three"
    ],
    "short_array": [
        1,
        2,
        3
    ]
}
```

In contrast, here's how the output looks in Configuru:

``` JSON
{
	"float":       3.14,
	"double":      3.14,
	"short_array": [ 1, 2, 3 ],
	"long_array":  [
		"one",
		[ "two", "things" ],
		"three"
	]
}
```

Note how Configuru refrains from unnecessary line breaks on short arrays and does not write superfluous (and ugly!) trailing decimals. Configuru also writes the keys of the objects in the same order as it was given (unless the `sort_keys` option is explicitly set). The aligned values is just a preference of mine, inspired by [how id software does it](http://kotaku.com/5975610/the-exceptional-beauty-of-doom-3s-source-code) (turn off with `object_align_values=false`). Writing the same data in the CFG format makes it turn out like this:

	float:       3.14
	double:      3.14
	short_array: [ 1 2 3 ]
	long_array:  [
		"one"
		[ "two" "things" ]
		"three"
	]
