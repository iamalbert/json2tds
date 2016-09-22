JSON = require './torch-cjson'
print(JSON)

obj = JSON.loads '[2, [34, [9, 0] ], null, 1.2, 302.00, true, false, "hi", "\\\\" ]'

print("size", #obj)
print(obj)
print(JSON.totable(obj))
for k,v in ipairs(obj) do
    print(k, v)
end

obj = JSON.loads [[
{
    "__len": "asdf;zxcv test",
    "glossary": {
        "title": "example glossary",
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
                        "GlossSeeAlso": ["GML", "XML"]
                    },
                    "Glo\nssSee": "markup"
                }
            }
        }
    }
}
]]
-- print(obj)
-- print(JSON.keys(obj))
-- print(JSON.keys(obj.glossary.GlossDiv.GlossList.GlossEntry))
print("size", #obj)
print(obj)
for k,v in pairs(obj) do
    print(k, v)
end
