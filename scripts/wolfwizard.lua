local function mapFormElements( elements)
	local rt = ""
	local function separator()
		if rt ~= "" then rt = rt .. "; " end
	end
	for i,element in pairs( elements) do
		local ee
		if element.attribute == "yes" then
			separator()
			ee = element.name .. "=" .. "{?}"
		elseif element.class == "atomic" then
			separator()
			ee = element.name .. "{{?}}"
		elseif tonumber(element.size) ~= 0 then
			separator()
			if element.class == "vector" then
				ee = element.name .. " {" .. mapFormElements( element.element ) .. "}"
			else
				ee = element.name .. "[] {" .. mapFormElements( element.element ) .. "}"
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
				output:print( form.name .. " " .. form.xmlroot .. " {" .. mapFormElements( form.element) .. "}")
			else
				output:print( form.name .. " " .. form.xmlroot)
			end
		end
	end
end

