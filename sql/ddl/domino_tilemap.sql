CREATE TABLE domino_tilemap
(
ID CHAR(32),
tileid INT,
tilemap BLOB,
PRIMARY KEY(ID,tileid)
);

