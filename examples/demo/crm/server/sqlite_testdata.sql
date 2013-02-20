BEGIN;

-- Employee:
INSERT INTO Employee (name,status,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Friedrich Muller', 'manager', 'Seerosenweg 3', NULL, '3045', 'Bumplitz', 'Schweiz');
INSERT INTO Employee (name,status,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Susanne Tobler', 'project leader', 'Kreuzstrasse 12', NULL, '5012', 'Luzern', 'Schweiz');
INSERT INTO Employee (name,status,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Tobias Gruber', 'software engineer', 'Henkersbrunnli 5', NULL, '3031', 'Bern', 'Schweiz');

-- Customer:
INSERT INTO Customer (name,description,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Area AG', NULL, 'Fernweg 3', NULL, '4014', 'Oberhofen', 'Schweiz');
INSERT INTO Customer (name,description,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Bitora Gmbh', 'Zulieferer fur B', 'Planetenstrasse 3', NULL, '7012', 'Lausanne', 'Schweiz');
INSERT INTO Customer (name,description,addr_line1,addr_line2,addr_postalcode,addr_district,addr_country) VALUES ('Turbocom AG', 'Motoren (T)', 'Gerechtigkeitsgasse 41', NULL, '6088', 'Solothurn', 'Schweiz');

-- CustomerNote:
INSERT INTO CustomerNote (customerid,content,link) VALUES ('1', 'Note 1 for Customer 1', 'http://link.com/1.1');
INSERT INTO CustomerNote (customerid,content,link) VALUES ('2', 'Note 1 for Customer 2', 'http://link.com/2.1');
INSERT INTO CustomerNote (customerid,content,link) VALUES ('2', 'Note 2 for Customer 2', 'http://link.com/2.2');
INSERT INTO CustomerNote (customerid,content,link) VALUES ('3', 'Note 1 for Customer 3', 'http://link.com/3.1');

-- Project:
INSERT INTO Project (customerid,name,description) VALUES ('1', 'Project 1.1', 'Project 1 for Customer 1');
INSERT INTO Project (customerid,name,description) VALUES ('2', 'Project 2.1', 'Project 1 for Customer 2');
INSERT INTO Project (customerid,name,description) VALUES ('2', 'Project 2.1', 'Project 2 for Customer 2');
INSERT INTO Project (customerid,name,description) VALUES ('3', 'Project 3.1', 'Project 1 for Customer 3');

-- ProjectNote:
INSERT INTO ProjectNote (projectid,content,link) VALUES ('1', 'Note 1 for Project 1', 'http://link.com/1.1');
INSERT INTO ProjectNote (projectid,content,link) VALUES ('2', 'Note 1 for Project 2', 'http://link.com/2.1');
INSERT INTO ProjectNote (projectid,content,link) VALUES ('2', 'Note 2 for Project 2', 'http://link.com/2.2');
INSERT INTO ProjectNote (projectid,content,link) VALUES ('3', 'Note 1 for Project 3', 'http://link.com/3.1');

-- ActivityCategory:
INSERT INTO ActivityCategory (name,description) VALUES ('Bugfixing', 'Fixing bugs');
INSERT INTO ActivityCategory (name,description) VALUES ('Implementation', 'Implementing something');
INSERT INTO ActivityCategory (name,description) VALUES ('Meeting', 'Meeting');
INSERT INTO ActivityCategory (name,description) VALUES ('Training', 'Training for customers');
INSERT INTO ActivityCategory (name,description) VALUES ('Offer', 'Wrining an offer');

-- ActivityCharging:
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/1/1',0,30,15,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/1/1',0,3,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/5/1',0,4,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/1/1',0,3,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/5/1',0,4,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/1/1',200,3,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/5/1',200,4,1,6);
INSERT INTO ActivityCharging (categoryid,start,fixum,timedep,timeunit,taxes) VALUES ('1','2013/1/1',0,0,0,0);

-- Activity:
INSERT INTO Activity (categoryid,projectid,employeeid,chargingid,description,start,duration) VALUES ('1','1','1','1','activity (catg 1) for project 1 by employee 1', '2013/2/10 09:12', 120);
INSERT INTO Activity (categoryid,projectid,employeeid,chargingid,description,start,duration) VALUES ('2','1','1','1','activity (catg 2) for project 1 by employee 1', '2013/2/11 10:13', 120);
INSERT INTO Activity (categoryid,projectid,employeeid,chargingid,description,start,duration) VALUES ('3','1','1','1','activity (catg 3) for project 1 by employee 1', '2013/2/13 14:31', 80);

-- ActivityNote:
INSERT INTO ActivityNote (activityid,content,link) VALUES ('1', 'Note 1 for Activity 1', 'http://link.com/1.1');
INSERT INTO ActivityNote (activityid,content,link) VALUES ('2', 'Note 1 for Activity 2', 'http://link.com/2.1');
INSERT INTO ActivityNote (activityid,content,link) VALUES ('2', 'Note 2 for Activity 2', 'http://link.com/2.2');
INSERT INTO ActivityNote (activityid,content,link) VALUES ('3', 'Note 1 for Activity 3', 'http://link.com/3.1');

COMMIT;
