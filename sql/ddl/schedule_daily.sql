DROP TABLE schedule_daily;
CREATE TABLE schedule_daily
(
ID CHAR(32),
aID INT,
act_time DATETIME,
PRIMARY KEY(ID,act_time,aID)
);
