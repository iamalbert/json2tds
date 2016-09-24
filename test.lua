JSON = require 'torch-cjson'
print(JSON)

--io.read()
print "start"

obj = JSON.load "e.json"
print(#obj)
--io.read()
obj = nil; collectgarbage();
--io.read()

obj = JSON.loads '[2, [34, [9, 0] ], null, 1.2, 302.00, true, false, "hi", "\\\\" ]'
print(obj)
print("#obj", #obj)
print(JSON.totable(obj))
for k,v in ipairs(obj) do
    print(k, v)
end

--io.read()
obj = nil; collectgarbage();
--io.read()

obj = JSON.loads [[
{
    "__len": "asdf;zxcv test",
    "glossary": {
        "title": "exa_mple gl_ossary",
        "GlossDiv": {
            "title": "S",
            "GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
                    "SortAs": "SGML",
                    "GlossTerm": "Sta\\ndard Generalized Markup Language",
                    "Acronym": "SGML",
                    "Abbrev": "ISO 8879:1986",
                    "name": null,
                    "Glos\"sDef": {
                        "para": "A meta-markup\tlan\nguage, used to",
                        "GlossSeeAlso": [
                            "GML", "XML", ["asdfzxcv3"],
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3",
                            "GML", "XML", "asdfzxcv3"
                        ]
                    },
                    "Glo\nssSee": "markup\u4e2d\u6587"
                }
            }
        },
        "title23": "",
        "ti2tle": "exa_mple gl_o我是誰ssary0"
    }
}
]]
print(obj)
--print(JSON.keys(obj))
-- print(obj.glossary) --.GlossDiv.GlossList.GlossEntry))
-- print("size", #obj)
-- print(obj)
-- for k,v in pairs(obj) do print(k, v) end

--io.read()
obj = nil; collectgarbage();
--io.read()

obj = JSON.loads[[
{"a":1}
{"a":2, "obj": {} }
{"a":3, "list":[] }
]]
print(obj)

