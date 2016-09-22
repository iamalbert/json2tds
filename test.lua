p = require './cjson'
print(p)

obj = p.loads '[2, [34, [9, 0] ], 1.2, 302.00, true, false, "hi", "\\\\" ]'
print(obj)

print(#obj)
for i = 1, #obj do
    print(i , obj[i] )
end

