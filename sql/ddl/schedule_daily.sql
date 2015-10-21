DROP TABLE schedule_daily;
CREATE TABLE schedule_daily
(
ID CHAR(32),
aID INT,
act_time VARCHAR(16),
PRIMARY KEY(ID,act_time,aID)
);
