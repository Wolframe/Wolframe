local function addPictureInfo( picture)
	info = formfunction( "imageInfo" )( { data = picture["image"] } ):table()
	picture["width"] = info.width
	picture["height"] = info.height
	thumb = formfunction( "imageThumb" )( { [ "image" ] = { [ "data" ] = picture["image"] }, [ "size" ] = 50 } ):table( )
	picture["thumbnail"] = thumb.data
	return picture;
end

function InsertCustomerPicture( inp)
	return formfunction("DoInsertCustomerPicture")( addPictureInfo( inp:table()))
end

