
function insertNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("insertNumber")(inp)
end

function updateNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("updateNumber")(inp)
end

function selectNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("selectNumber")(inp)
	output:print( res:table())
end

function deleteNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("deleteNumber")(inp)
end

