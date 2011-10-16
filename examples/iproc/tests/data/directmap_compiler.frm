struct PLANT
	string COMMON []
	string BOTANICAL
	uint ZONE
	string LIGHT
	string PRICE
	uint AVAILABILITY
end

struct PHONE
	string NUMBER
	string MOBILE
end

struct ADDRESS
	int COUNTRY
	string STREET
	string CITY
	ref PHONE
end

struct GARDEN
	string NAME
	ref ADDRESS
	ref PLANT []
end

struct PLACES
	ref GARDEN
end
