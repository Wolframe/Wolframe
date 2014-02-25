function run()
	for v,t in input:get()
	do
		if not t and v then
			output:print( provider.normalizer("name")(v),t)
		else
			output:print( v,t)
		end
	end
end

