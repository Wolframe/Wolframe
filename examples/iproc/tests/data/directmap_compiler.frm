form PLANT
	string COMMON []
	string BOTANICAL
	uint ZONE
	string LIGHT
	string PRICE
	uint AVAILABILITY
end

form PHONE
	string NUMBER
	string MOBILE
end

form ADDRESS
	int COUNTRY
	string STREET
	string CITY
	form PHONE
end

form GARDEN
	string NAME
	form ADDRESS
	form PLANT []
end

form PLACES
	form GARDEN
end
