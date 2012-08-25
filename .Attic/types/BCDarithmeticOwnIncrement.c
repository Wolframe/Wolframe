// see module types_bcdArithmetic.cpp: These functions here do not work yet

static boost::uint32_t increment( boost::uint32_t a)
{
	boost::uint32_t t1,t2,t3,t4,t5;
	t1 = a + 0x06666667;
	// convert 0 bytes to 0x6 bytes
	t2 = t1 ^ 0xffffFFFF;
	t3 = t2 & (t2 >> 1);				//... t3 has now bit 0100 set 's where t1 has bits 0011 set
	t4 = t3 & 0x44444444;				//... t4 has 0x4 set for nibbles in t1 in range 0x0 to 0x3 (=0 for a valid bcd a)
	t5 = t1 | (t4 | (t4 >> 1));			//... t5 has 0x6 set for the 0 nibbles in t1
	return (t5 - 0x06666666);
}

static boost::uint32_t decrement( boost::uint32_t a)
{
	boost::uint32_t t1,t2,t3,t4,t5,t6;
	t1 = a - 1;
	// convert 0xF bytes to 0x9 bytes
	t2 = t1 & 0xeeeeEEEE;				//... delete bit 0001 of each nibble
	t3 = t2 & (t2 >> 1);				//... t3 has 0010 set for nibbles in t1 having bit 0110 set
	t3 = t3 & (t3 >> 1);				//... t3 has 0010 set for nibbles in t1 having bits 1110 set
	return t1 - (t3 | (t3 << 1) | (t3 >> 1));	//... subtract 7 from each elememt in t1 equals 0xF
}
