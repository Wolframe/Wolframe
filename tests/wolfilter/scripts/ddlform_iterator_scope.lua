function print_tree( itr)
	for v,t in itr do
		output:print( v,t)
	end
	output:closetag()
end

function run_employee( itr)
	for v,t in itr do
		if t == "firstname" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "phone" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "surname" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		else
			logger.print( "ERROR", "unknown tag ", t, " ", v)
		end
	end
end

function run_task( itr)
	for v,t in itr do
		logger.print( "INFO", "task element ", t, " ", v)
		if t == "title" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "key" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "customernumber" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		else
			logger.print( "ERROR", "unknown tag ", t, " ", v)
		end
	end
end

function run_assignment( itr)
	for v,t in itr do
		if t == "task" then
			output:opentag( t)
			run_task( scope( itr))
			output:closetag()
		elseif t == "issuedate" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "employee" then
			output:opentag( t)
			run_employee( scope( itr))
			output:closetag()
		else
			logger.print( "ERROR", "unknown element ", t, " ", v)
		end
	end
end

function run_assignmentlist( itr)
	for v,t in itr do
		if t == "assignment" then
			output:opentag( t)
			run_assignment( scope( itr))
			output:closetag()
		else
			logger.print( "ERROR", "unknown tag ", t, " ", v)
		end
	end
end

function run()
	r = form("employee_assignment_print")
	r:fill( input:table())
	itr = r:get()
	for v,t in itr do
		if t == "assignmentlist" then
			output:opentag( "assignmentlist")
			run_assignmentlist( scope( itr))
			output:closetag()
		else
			logger.print( "ERROR", "unknown tag ", t, " ", v)
		end
	end
end

