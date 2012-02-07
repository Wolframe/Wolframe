CREATE TABLE employee (
	firstname TEXT,
	surname TEXT,
	number SERIAL PRIMARY KEY,
	phone TEXT,
	email TEXT,
	jobtitle TEXT
)

CREATE TABLE customer (
	name TEXT,
	number SERIAL PRIMARY KEY,
	addresshdr TEXT,
	street TEXT,
	streetnumber INTEGER,
	state TEXT,
	postalcode INTEGER,
	country TEXT
)

CREATE TABLE customercontact (
	firstname TEXT,
        surname TEXT,
	department VARCHAR(15), 
        number SERIAL PRIMARY KEY,
        phone TEXT,
        email TEXT,
	customernumber REFERENCES customer(number) 
)

CREATE TABLE task (
	title TEXT NOT NULL,
	key VARCHAR(15) PRIMARY KEY,
	customernumber REFERENCES customer(number),
	opendate TIMESTAMP,
	requireddate TIMESTAMP,
	closedate TIMESTAMP,
	timebudget INTERVAL,
	status VARCHAR(15)
)

CREATE TABLE timerecording (
	taskkey VARCHAR(15) REFERENCES task(key),
	start TIMESTAMP,
	stop TIMESTAMP,
	employeenumber SERIAL REFERENCES employee(number)	
)

CREATE TABLE assignement (
	taskkey VARCHAR(15) REFERENCES task(key),
	employeenumber SERIAL REFERENCES employee(number),
	issuedate TIMESTAMP,
	reportto SERIAL REFERENCES employee(number)
)


		


			
