**
**requires:SQLITE3
**requires:DISABLED
**input
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < a s s i g n m e n t l i s t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   1 < / t i t l e > < k e y > A 1 2 3 < / k e y > < c u s t o m e r n u m b e r > 3 2 4 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   2 < / t i t l e > < k e y > V 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J u l i a < / f i r s t n a m e > < s u r n a m e > T e g e l - S a c h e r < / s u r n a m e > < p h o n e > 0 9 8   7 6 5   4 3   2 1 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < a s s i g n m e n t > < t a s k > < t i t l e > j o b   3 < / t i t l e > < k e y > A 4 5 6 < / k e y > < c u s t o m e r n u m b e r > 5 6 7 < / c u s t o m e r n u m b e r > < / t a s k > < t a s k > < t i t l e > j o b   4 < / t i t l e > < k e y > V 7 8 9 < / k e y > < c u s t o m e r n u m b e r > 8 9 0 < / c u s t o m e r n u m b e r > < / t a s k > < e m p l o y e e > < f i r s t n a m e > J a k o b < / f i r s t n a m e > < s u r n a m e > S t e g e l i n < / s u r n a m e > < p h o n e > 0 1 2   3 4 5   6 7   8 9 < / p h o n e > < / e m p l o y e e > < i s s u e d a t e > 1 3 . 5 . 2 0 0 6 < / i s s u e d a t e > < / a s s i g n m e n t > < / a s s i g n m e n t l i s t >**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl --directmap 'f=xml,c=test_transaction,n=run' run

**file: DBDATA
CREATE TABLE task
(
	title STRING,
	id INTEGER,
	start DATE,
	end DATE
);

INSERT INTO task (title,id,start,end) VALUES ('bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14');
INSERT INTO task (title,id,start,end) VALUES ('bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34');
INSERT INTO task (title,id,start,end) VALUES ('blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31');
INSERT INTO task (title,id,start,end) VALUES ('ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44');
**file:DBPRG.tdl
STATEMENT get_task SELECT * FROM task WHERE id=$1;

TRANSACTION test_transaction
BEGIN
	INTO doc/task WITH //task DO get_task( id);
END
**outputfile:DBDUMP
**output
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < t a s k > < i d > 1 1 < / i d > < t a s k > b l a   b l a < / t a s k > < s t a r t > 1 2 : 0 4 : 1 9   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 4 1 : 3 4   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 1 2 < / i d > < t a s k > b l i   b l u < / t a s k > < s t a r t > 0 7 : 1 4 : 2 3   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 0 8 : 0 1 : 5 1   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 2 1 < / i d > < t a s k > g a r d e n i n g < / t a s k > < s t a r t > 0 9 : 2 4 : 2 8   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 1 : 1 1 : 0 7   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 2 2 < / i d > < t a s k > h e l o < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 1 < / i d > < t a s k > h u l a   h o p < / t a s k > < s t a r t > 1 9 : 1 4 : 3 8   1 / 4 / 2 0 1 2 < / s t a r t > < e n d > 2 0 : 0 1 : 1 2   1 / 4 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 2 < / i d > < t a s k > h u l a   h i p < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < t a s k > < i d > 3 3 < / i d > < t a s k > h u l a   h u p < / t a s k > < s t a r t > 1 1 : 3 1 : 0 1   1 / 3 / 2 0 1 2 < / s t a r t > < e n d > 1 2 : 0 7 : 5 5   1 / 3 / 2 0 1 2 < / e n d > < / t a s k > < / d o c > 
**end
