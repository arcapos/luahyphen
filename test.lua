-- The hyphen module provides hyphenation via libhyphen

local hyphen = require 'hyphen'

-- Load a hyphenation pattern file
local de_ch = hyphen.load('de_ch.dic')

-- The load_file() function is not available on all platforms
if type(hyphen.load_file) == 'function' then
	local file = io.open('de_ch.dic')
	local dehypht = hyphen.load_file(file)
	dehypt:free()
end

-- Two ways of calling the hyphenation function
print(de_ch:hyphenate('Museumspublikation'))
print(hyphen.hyphenate(de_ch, 'Museumspublikation'))

-- Hyphenate words passed as arguments
for _, v in ipairs(arg) do
	print(v, de_ch:hyphenate(v))
end

-- Using the hyphenation function to break up lines
local function hyphenate(dict, word, limit)
	local p = {}
	local s = {}
	local l = 0

	local h = dict:hyphenate(word)
	for part in string.gmatch(h, '([^=]+)') do
		l = l + #part
		p[#p + 1] = part

		if l >= limit then
			s[#s + 1] = table.concat(p)
			p = {}
			l = 0
		end
	end
	if l > 0 then
		s[#s + 1] = table.concat(p)
	end
	return ipairs(s)
end

for k, v in hyphenate(de_ch, 'Museumspublikation', 5) do
	if k > 1 then
		io.write('-<br>\n')
	end
	io.write(v)
end
io.write('\n')

-- Hyphenate text and replace the marker with the HTML &shy entity

local function shy(text)
	for word in string.gmatch(text, '([^ ]+)') do
		local h = string.gsub(de_ch:hyphenate(word), '=',
		    '&shy;')
		io.write(h .. ' ')
	end
	io.write('\n')
end

shy('Museumspublikationen sind fester Bestandteil des Angebots')
