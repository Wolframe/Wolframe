local function isAtomic( typename)
	if typename == "string" then
		return true
	elseif typename == "int" then
		return true
	elseif typename == "uint" then
		return true
	elseif typename == "bool" then
		return true
	elseif typename == "double" then
		return true
	else
		return false
	end
end

local function mapFormElements( elements)
	local rt = ""
	local function separator()
		if rt ~= "" then rt = rt .. "; " end
	end
	for i,element in pairs( elements) do
		logger.printc( "VISIT ", element)
		local ee
		if element.attribute == "yes" then
			separator()
			ee = element.name .. "=" .. "{?}"
		elseif isAtomic( element.type) then
			separator()
			ee = element.name .. "{{?}}"
		elseif element.type == "indirection" then
			separator()
			if element.backref then
				if element.backref == element.name then
					ee = "^" .. element.name
				else
					ee = "^" .. element.name .. ":" .. element.backref
				end
			else
				ee = element.name .. "{?}"
			end
		elseif element.type == "array" then
			separator()
			if element.elemtype == "struct" then
				if element.element then
					ee = element.name .. "[] {" .. mapFormElements( element.element ) .. "}"
				end
			elseif element.elemtype == "indirection" then
				if element.backref then
					if element.backref == element.name then
						ee = "^" .. element.name .. "[]"
					else
						ee = "^" .. element.name .. ":" .. element.backref  .. "[]"
					end
				else
					ee = element.name .. "[] {?}"
				end
			else
				ee = element.name .. " " .. element.elemtype .. "[]"
			end
		elseif element.type == "struct" then
			separator()
			if element.element then			
				ee = element.name .. " {" .. mapFormElements( element.element ) .. "}"
			else
				ee = element.name .. " {}"
			end
		else
			ee = ""
		end
		rt = rt .. ee
	end
	return rt
end

function filterRequests()
	filter().empty = false
	output:as( filter("line"))

	inp = input:table()
	for i,form in pairs( inp.forms.form) do
		if form.xmlroot then
			if form.element then
				output:print( form.name .. " " .. mapFormElements( form.element))
			else
				output:print( form.name .. " " .. form.xmlroot)
			end
		end
	end
end

